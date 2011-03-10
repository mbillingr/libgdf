//
// This file is part of libGDF.
//
// libGDF is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// libGDF is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Martin Billinger

#include "gdf_mex.h"
#include "matlab_tools/mxStructAccess.h"
#include <GDF/Reader.h>
#include <mex.h>

#include <boost/lexical_cast.hpp>


using namespace std;

// Calling options:
//   [s,h,e] = mex_reader( 'file.gdf', 'option', value, ... );

#define OPTION_MULTIRATESIGNALS             "DATAFORMAT"
#define OPTION_MULTIRATESIGNALS_UPSAMPLE    "MATRIX"
#define OPTION_MULTIRATESIGNALS_GROUOP      "GROUP"
#define OPTION_MULTIRATESIGNALS_SINGLE      "SINGLE"

#define OPTION_UPSAMPLEMODE             "UPSAMPLEMODE"
#define OPTION_UPSAMPLEMODE_NEAREST     "NEAREST"
#define OPTION_UPSAMPLEMODE_LINEAR      "LINEAR"

#define OPTION_DATAFORMAT       "DATAORIENTATION"
#define OPTION_DATAFORMAT_ROW   "ROW"
#define OPTION_DATAFORMAT_COL1  "COL"
#define OPTION_DATAFORMAT_COL2  "COLUMN"

enum eMultirateMode
{
    MR_UPSAMPLE,
    MR_GROUP,
    MR_SINGLE
};

enum eDataOrientation
{
    DO_ROW,
    DO_COL
};

// ===========================================================================
// ===========================================================================

class Interpolator
{
public:
    void expand( double *data, size_t stride, size_t num_src_samples, size_t num_dst_samples );
    virtual double interpolate( double d1, double d2, double ratio ) = 0;
};

class InterpolatorNearest : public Interpolator
{
public:
    virtual double interpolate( double d1, double d2, double ratio );
};

class InterpolatorLinear : public Interpolator
{
public:
    virtual double interpolate( double d1, double d2, double ratio );
};

class InterpolatorDummy : public Interpolator
{
public:
    virtual double interpolate( double, double, double ) { throw std::invalid_argument( "Attempting to load multirate data in a matrix without upsampling. Either set UPSAMPLEMODE, or choose GROUP or SINGLE data output." ); }
};

// ===========================================================================
// ===========================================================================

class CmexObject
{
public:
    CmexObject( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] );

    ~CmexObject( );

    void execute( );
    void getUpsampleData( gdf::Reader &reader );
    void getGroupData( gdf::Reader &reader );
    void getSingleData( gdf::Reader &reader );
    void loadEvents( gdf::Reader &reader );
    void constructHeader( gdf::Reader &reader );

    void parseInputArguments( );

private:

    size_t nlhs_, nrhs_;
    mxArray **plhs_;
    const mxArray **prhs_;

    string filename;
    eMultirateMode multirate_mode;
    Interpolator *interpolator;
    eDataOrientation data_orientation;

    gdf::uint16 num_signals;
    gdf::uint64 num_records;
    gdf::uint32 max_rate;
    size_t num_samplerates;

    map< gdf::uint32, vector<gdf::uint16> > signals_by_samplerate;
    vector<gdf::uint32> samples_per_record;
};

// ===========================================================================
// ===========================================================================

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    CmexObject mxo( nlhs, plhs, nrhs, prhs );
    mxo.execute();
}

// ===========================================================================
// ===========================================================================

CmexObject::CmexObject( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] )
{
    using boost::numeric_cast;

    interpolator = NULL;

    if (nrhs == 0)
        throw invalid_argument( "No input argument supplied :(" );

    if( nlhs == 0 )
        throw invalid_argument( "Nothing to do (no output arguments)" );

    if( nlhs > 3 )
        throw invalid_argument( "Don't know what to do (too many output arguments)" );

    if( !mxIsChar(prhs[0]) )
        throw invalid_argument( "Please supply a valid file name as first argument." );

    // set defaults
    multirate_mode = MR_SINGLE;
    interpolator = new InterpolatorDummy( );
    data_orientation = DO_COL;

    nlhs_ = nlhs;
    plhs_ = plhs;
    nrhs_ = nrhs;
    prhs_ = prhs;

    parseInputArguments( );
}

// ===========================================================================

CmexObject::~CmexObject( )
{
    if( interpolator ) delete interpolator;
}

// ===========================================================================

void CmexObject::execute( )
{
    gdf::Reader reader;

    reader.enableCache( false );
    reader.open( filename );

    num_signals = reader.getMainHeader_readonly().get_num_signals();
    num_records = reader.getMainHeader_readonly().get_num_datarecords();

    if( num_signals > 0 )
    {

        // === order signals by samplerate ===

        samples_per_record.resize( num_signals );
        for( gdf::uint16 i = 0; i < num_signals; i++ )
        {
            gdf::uint32 sr = reader.getSignalHeader_readonly( i ).get_samples_per_record( );
            samples_per_record[i] = sr;
            signals_by_samplerate[sr].push_back( i );
        }

        num_samplerates = signals_by_samplerate.size( );
        max_rate = (signals_by_samplerate.rbegin())->first;  // map is supposed to be ordered

        //if( num_samplerates == 1 )
        //    multirate_mode = MR_UPSAMPLE;   // override mode for unirate data

        // === get data ===

        switch( multirate_mode )
        {
        default: throw invalid_argument( " invalid multirate mode" ); break;
        case MR_UPSAMPLE: getUpsampleData( reader ); break;
        case MR_GROUP: getGroupData( reader ); break;
        case MR_SINGLE: getSingleData( reader ); break;
        }
    }
    else
    {
        plhs_[0] = mxCreateNumericMatrix( 0, 0, mxDOUBLE_CLASS, mxREAL );
    }

    // === load events ===
    if( nlhs_ > 2 )
    {
        loadEvents( reader );
    }

    // === construct header ===
    if( nlhs_ > 1 )
    {
        try {
            constructHeader( reader );
        } catch( std::exception &e )
        {
            mexPrintf( "Exception while constructing the header:\n%s\nThe header structure may be incomplete!\n", e.what() );
        }
    }

    reader.close( );
}

// ===========================================================================

void CmexObject::getUpsampleData( gdf::Reader &reader )
{
    // construct output structure
    if( data_orientation == DO_COL )
        plhs_[0] = mxCreateNumericMatrix( num_signals, num_records*max_rate, mxDOUBLE_CLASS, mxREAL );
    else if( data_orientation == DO_ROW )
        plhs_[0] = mxCreateNumericMatrix( num_records*max_rate, num_signals, mxDOUBLE_CLASS, mxREAL );
    else
        throw invalid_argument( "Invalid data orientation in CmexObject::getUpsampleData()." );

    double *data = mxGetPr( plhs_[0] );

    // fill output structure with data
    for( gdf::uint64 r=0; r<num_records; r++ )
    {
        gdf::Record *rec = reader.getRecordPtr( r );
        for( gdf::uint16 s=0; s<num_signals; s++ )
        {
            // signals with lower sampling rate only fill the beginning of the channel
            for( gdf::uint32 n=0; n<samples_per_record[s]; n++ )
            {
                if( data_orientation == DO_COL )
                {
                    size_t column = n + r * samples_per_record[s];
                    data[s+column*num_signals] = rec->getChannel( s )->getSamplePhys( n );
                }
                else
                {
                    size_t row = n + r * samples_per_record[s];
                    data[row+s*num_records*max_rate] = rec->getChannel( s )->getSamplePhys( n );
                }
            }
        }
    }

    // interpolate signals with lower sampling rate by expanding partially
    // filled channels to the entire length
    for( gdf::uint16 s=0; s<num_signals; s++ )
    {
        if( samples_per_record[s] != max_rate )
        {
            if( data_orientation == DO_COL )
                interpolator->expand( &data[s], num_signals, samples_per_record[s]*num_records, max_rate*num_records );
            else
                interpolator->expand( &data[s*num_records*max_rate], 1, samples_per_record[s]*num_records, max_rate*num_records );
        }
    }
}

// ===========================================================================

void CmexObject::getGroupData( gdf::Reader &reader )
{
    // construct output structure

    plhs_[0] = mxCreateStructMatrix( num_samplerates, 1, 0, NULL );
    mxAddField( plhs_[0], "channels" );
    mxAddField( plhs_[0], "data" );

    map< gdf::uint32, vector<gdf::uint16> >::iterator it = signals_by_samplerate.begin( );
    for( gdf::uint16 g=0; g<num_samplerates; g++, it++ )
    {
        gdf::uint16 signal = it->second.front( );   // just one of the signals in this group
        size_t num_samples = samples_per_record[signal] * num_records;

        mxArray *datablock, *channel_table;

        if( data_orientation == DO_COL )
        {
            datablock = mxCreateNumericMatrix( it->second.size(), num_samples, mxDOUBLE_CLASS, mxREAL );
            channel_table = mxCreateNumericMatrix( 1, it->second.size(), mxDOUBLE_CLASS, mxREAL );
        }
        else if( data_orientation == DO_ROW)
        {
            datablock = mxCreateNumericMatrix( num_samples, it->second.size(), mxDOUBLE_CLASS, mxREAL );
            channel_table = mxCreateNumericMatrix( it->second.size(), 1, mxDOUBLE_CLASS, mxREAL );
        }
        else
            throw invalid_argument( "Invalid data orientation in CmexObject::getGroupData()." );

        mx::setField( plhs_[0], datablock, "data", g+1 );
        mx::setField( plhs_[0], channel_table, "channels", g+1 );

        for( size_t c=0; c<it->second.size(); c++ )
            mxGetPr( channel_table )[c] = it->second[c] + 1;
    }

    // fill output structure with data
    for( gdf::uint64 r=0; r<num_records; r++ )
    {
        gdf::Record *rec = reader.getRecordPtr( r );
        it = signals_by_samplerate.begin( );
        for( gdf::uint16 g=0; g<num_samplerates; g++, it++ )
        {
            if( it->first == 0 )
                continue;
            double *data = mxGetPr( mx::getField( plhs_[0], "data", g+1 ) );
            size_t rows = it->second.size( );
            for( size_t s=0; s<rows; s++ )
            {
                gdf::uint16 signal = it->second[s];
                for( gdf::uint32 n=0; n<samples_per_record[signal]; n++ )
                {
                    if( data_orientation == DO_COL )
                    {
                        size_t column = n + r * samples_per_record[signal];
                        data[s+column*rows] = rec->getChannel(signal)->getSamplePhys( n );
                    }
                    else
                    {
                        size_t row = n + r * samples_per_record[s];
                        data[row+s*num_records*samples_per_record[signal]] = rec->getChannel( s )->getSamplePhys( n );
                    }
                }
            }
        }
    }
}

// ===========================================================================

void CmexObject::getSingleData( gdf::Reader &reader )
{
    // construct output structure
    plhs_[0] = mxCreateCellMatrix( num_signals, 1 );
    for( gdf::uint16 s=0; s<num_signals; s++ )
    {
        mxArray *signal;
        if( data_orientation == DO_COL)
            signal = mxCreateNumericMatrix( 1, samples_per_record[s] * num_records, mxDOUBLE_CLASS, mxREAL );
        else if( data_orientation == DO_ROW)
            signal = mxCreateNumericMatrix( samples_per_record[s] * num_records, 1, mxDOUBLE_CLASS, mxREAL );
        else
            throw invalid_argument( "Invalid data orientation in CmexObject::getSingleData()." );

        mxSetCell( plhs_[0], s, signal );
    }

    // fill output structure with data
    for( gdf::uint64 r=0; r<num_records; r++ )
    {
        gdf::Record *rec = reader.getRecordPtr( r );
        for( gdf::uint16 s=0; s<num_signals; s++ )
        {
            mxArray *cell = mxGetCell( plhs_[0], s );
            for( gdf::uint32 n=0; n<samples_per_record[s]; n++ )
            {
                double *data = mxGetPr( cell );
                data[n+r*samples_per_record[s]] = rec->getChannel(s)->getSamplePhys( n );
            }
        }
    }
}

// ===========================================================================

void CmexObject::loadEvents( gdf::Reader &reader )
{
    plhs_[2] = mxCreateStructMatrix( 1, 1, 0, NULL );
    gdf::EventHeader *evh = reader.getEventHeader( );
    gdf::uint32 num_ev = evh->getNumEvents( );

    switch( evh->getMode() )
    {
    default: throw invalid_argument( " Invalid Event Mode." );
    case 1: {
            mx::setField( plhs_[2], NULL, GDFE_MODE );
            mx::setField( plhs_[2], NULL, GDFE_FS );
            mx::setField( plhs_[2], NULL, GDFE_POS );
            mx::setField( plhs_[2], NULL, GDFE_TYP );

            mx::setFieldNumeric( plhs_[2], evh->getSamplingRate( ), GDFE_FS );
            mx::setFieldNumeric( plhs_[2], evh->getMode( ), GDFE_MODE );

            mxArray *mxpos = mxCreateNumericMatrix( 1, num_ev, mxUINT32_CLASS, mxREAL );
            mx::setField( plhs_[2], mxpos, GDFE_POS );
            gdf::uint32 *positions= reinterpret_cast<gdf::uint32*>( mxGetData( mxpos ) );

            mxArray *mxtyp = mxCreateNumericMatrix( 1, num_ev, mxUINT16_CLASS, mxREAL );
            mx::setField( plhs_[2], mxtyp, GDFE_TYP );
            gdf::uint16 *types= reinterpret_cast<gdf::uint16*>( mxGetData( mxtyp ) );

            for( gdf::uint32 e=0; e<num_ev; e++ )
            {
                gdf::Mode1Event event;
                evh->getEvent( e, event );
                positions[e] = event.position;
                types[e] = event.type;
            }
        } break;
    case 3: {
            mx::setField( plhs_[2], NULL, GDFE_MODE );
            mx::setField( plhs_[2], NULL, GDFE_FS );
            mx::setField( plhs_[2], NULL, GDFE_POS );
            mx::setField( plhs_[2], NULL, GDFE_TYP );
            mx::setField( plhs_[2], NULL, GDFE_3_CHA );
            mx::setField( plhs_[2], NULL, GDFE_3_DUR );

            mx::setFieldNumeric( plhs_[2], evh->getSamplingRate( ), GDFE_FS );
            mx::setFieldNumeric( plhs_[2], evh->getMode( ), GDFE_MODE );

            mxArray *mxpos = mxCreateNumericMatrix( 1, num_ev, mxUINT32_CLASS, mxREAL );
            mx::setField( plhs_[2], mxpos, GDFE_POS );
            gdf::uint32 *positions= reinterpret_cast<gdf::uint32*>( mxGetData( mxpos ) );

            mxArray *mxtyp = mxCreateNumericMatrix( 1, num_ev, mxUINT16_CLASS, mxREAL );
            mx::setField( plhs_[2], mxtyp, GDFE_TYP );
            gdf::uint16 *types= reinterpret_cast<gdf::uint16*>( mxGetData( mxtyp ) );

            mxArray *mxcha = mxCreateNumericMatrix( 1, num_ev, mxUINT16_CLASS, mxREAL );
            mx::setField( plhs_[2], mxcha, GDFE_3_CHA );
            gdf::uint16 *channels= reinterpret_cast<gdf::uint16*>( mxGetData( mxcha ) );

            mxArray *mxdur = mxCreateNumericMatrix( 1, num_ev, mxUINT32_CLASS, mxREAL );
            mx::setField( plhs_[2], mxdur, GDFE_3_DUR );
            gdf::uint32 *durations= reinterpret_cast<gdf::uint32*>( mxGetData( mxdur ) );

            for( gdf::uint32 e=0; e<num_ev; e++ )
            {
                gdf::Mode3Event event;
                evh->getEvent( e, event );
                positions[e] = event.position;
                types[e] = event.type;
                channels[e] = event.channel;
                durations[e] = event.duration;
            }
        } break;
    }
}

// ===========================================================================

void CmexObject::constructHeader( gdf::Reader &reader )
{
    plhs_[1] = constructHeaderStruct( num_signals );

    Header2Struct( plhs_[1], &reader.getHeaderAccess_readonly() );
}

// ===========================================================================

void CmexObject::parseInputArguments( )
{
    using boost::lexical_cast;

    filename = mx::getString( prhs_[0] );

    size_t n = 0;
    while( ++n < nrhs_ ) // yes, it's meant to start with 1
    {
        try {
            string opt = mx::getString( prhs_[n], mx::TOUPPER );
            if( opt == OPTION_UPSAMPLEMODE )
            {
                n++;
                if( n >= nrhs_ )
                    throw invalid_argument( " No Upsamplemode specified." );
                string arg = mx::getString( prhs_[n], mx::TOUPPER );
                if( arg == OPTION_UPSAMPLEMODE_NEAREST )
                {
                    delete interpolator;
                    interpolator = new InterpolatorNearest( );
                }
                else if( arg == OPTION_UPSAMPLEMODE_LINEAR )
                {
                    delete interpolator;
                    interpolator = new InterpolatorLinear( );
                }
                else
                    throw invalid_argument( " Unknown Upsamplemode: '"+arg+"'" );
            }
            else if( opt == OPTION_MULTIRATESIGNALS )
            {
                n++;
                if( n >= nrhs_ )
                throw invalid_argument( " No Multirate mode specified." );
                    string arg = mx::getString( prhs_[n], mx::TOUPPER );
                if( arg == OPTION_MULTIRATESIGNALS_UPSAMPLE )
                    multirate_mode = MR_UPSAMPLE;
                else if( arg == OPTION_MULTIRATESIGNALS_GROUOP )
                    multirate_mode = MR_GROUP;
                else if( arg == OPTION_MULTIRATESIGNALS_SINGLE )
                    multirate_mode = MR_SINGLE;
                else
                    throw invalid_argument( " Unknown Multirate mode: '"+arg+"'" );
            }
            else if( opt == OPTION_DATAFORMAT )
            {
                n++;
                if( n >= nrhs_ )
                throw invalid_argument( " No Data Orientation specified." );
                    string arg = mx::getString( prhs_[n], mx::TOUPPER );
                if( arg == OPTION_DATAFORMAT_COL1 )
                    data_orientation = DO_COL;
                else if( arg == OPTION_DATAFORMAT_COL2 )
                    data_orientation = DO_COL;
                else if( arg == OPTION_DATAFORMAT_ROW )
                    data_orientation = DO_ROW;
                else
                    throw invalid_argument( " Unknown Data Orientation: '"+arg+"'" );
            }
        } catch( mx::Exception &e )
        {
            throw invalid_argument( " While parsing argument "+lexical_cast<string>(n+1)+": "+e.what() );
        }
    }
}

// ===========================================================================
// ===========================================================================

void Interpolator::expand( double *data, size_t stride, size_t num_src_samples, size_t num_dst_samples )
{
    using boost::numeric_cast;

    cout << "expanding: " << num_src_samples << " ==> " << num_dst_samples << endl;

    if( num_dst_samples == 0 )
        throw invalid_argument( "num_dst_samples is 0." );

    size_t pwrite = (num_dst_samples-1)*stride;
    size_t n=num_dst_samples;
    do
    {
        n--;
        double src = numeric_cast<double>(n)*numeric_cast<double>(num_src_samples)/numeric_cast<double>(num_dst_samples);

        if( src > n )
            throw logic_error( "Interpolation algorithm does not work!" );

        size_t m = numeric_cast<size_t>(floor(src));
        double ratio = src-m;

        double d1 = data[m*stride];
        double d2;
        if( m >= num_src_samples-1 )
            d2 = d1;    // boundary condition: assuming last sample repeats at the end of the signal
        else
            d2 = data[(m+1)*stride];

        data[pwrite] = interpolate( d1, d2, ratio );
        pwrite-=stride;
    } while( n>0 );

}

// ===========================================================================

double InterpolatorNearest::interpolate( double d1, double d2, double ratio )
{
    if( ratio >= 0.5 )
        return d2;
    return d1;
}

// ===========================================================================

double InterpolatorLinear::interpolate( double d1, double d2, double ratio )
{
    return d1+(d2-d1)*ratio;
}

// ===========================================================================
// ===========================================================================
