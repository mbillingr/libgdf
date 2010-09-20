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
#include "matlab_tools/InstanceManager.h"
#include "matlab_tools/Commands.h"
#include "matlab_tools/mxStructAccess.h"
#include <GDF/Writer.h>
#include <mex.h>

using namespace std;

// ===================================================================================================
//      Object Interface to mex
// ===================================================================================================

class CmexObject
{
public:
    CmexObject( );

    ~CmexObject( );

    void execute( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] );

    static CmexObject &getInstance( ) { return instance; }

    InstanceManager<gdf::Writer> writers;
    CommandManager commands;

private:
    static CmexObject instance;
};

// ===================================================================================================
//      Commands
// ===================================================================================================

class CMD_init : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_clear : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_clearall : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_getheader : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_setheader : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_createsignal : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_recduration : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_eventconfig : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_addsample : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_addrawsample : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_addsamplevector : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_blitsamples : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_blitrawsamples : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_mode1event : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_mode3event : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_open : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_close : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };

// ===================================================================================================
//      mexFunction
// ===================================================================================================

CmexObject CmexObject::instance;

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    CmexObject::getInstance( ).execute( nlhs, plhs, nrhs, prhs );
}

// ===================================================================================================
//      CmexObject Definition
// ===================================================================================================


CmexObject::CmexObject( )
{
    commands.registerCommand( "Init", new CMD_init( ), 1, 0 );
    commands.registerCommand( "Clear", new CMD_clear( ), 0, 1 );
    commands.registerCommand( "Clear All", new CMD_clearall( ), 0, 0 );
    commands.registerCommand( "CreateSignal", new CMD_createsignal( ), 0, 2 );
    commands.registerCommand( "Open", new CMD_open( ), 0, 2 );
    commands.registerCommand( "Close", new CMD_close( ), 0, 1 );
    commands.registerCommand( "GetHeader", new CMD_getheader( ), 1, 1 );
    commands.registerCommand( "SetHeader", new CMD_setheader( ), 0, 2 );
    commands.registerCommand( "RecordDuration", new CMD_recduration( ), 0, 2 );
    commands.registerCommand( "EventConfig", new CMD_eventconfig( ), 0, 3 );
    commands.registerCommand( "AddSample", new CMD_addsample( ), 0, 3 );
    commands.registerCommand( "AddRawSample", new CMD_addrawsample( ), 0, 3 );
    commands.registerCommand( "AddVecSample", new CMD_addsamplevector( ), 0, 3 );
    commands.registerCommand( "BlitSamples", new CMD_blitsamples( ), 0, 3 );
    commands.registerCommand( "BlitRawSamples", new CMD_blitrawsamples( ), 0, 3 );
    commands.registerCommand( "Mode1Ev", new CMD_mode1event( ), 0, 3 );
    commands.registerCommand( "Mode3Ev", new CMD_mode3event( ), 0, 5 );
}

CmexObject::~CmexObject( )
{
}

void CmexObject::execute( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] )
{
    if( nrhs < 1 )
        throw std::invalid_argument( "Expecting at least one argument." );

    if( !mxIsChar(prhs[0]) )
        throw std::invalid_argument( "First argument must be a command (string)." );

    char *command;
    command = new char[32];
    if( mxGetString( prhs[0], command, 32 ) != 0 )
        throw std::invalid_argument( "Unknown problem with command string." );

    commands.execute( command, nlhs, plhs, nrhs-1, prhs+1 );
    delete[] command;
}

// ===================================================================================================
//      Command Definitions
// ===================================================================================================

void CMD_init::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = CmexObject::getInstance().writers.newInstance( );
    plhs[0] = mxCreateNumericMatrix( 1, 1, mxUINT64_CLASS, mxREAL );
    *reinterpret_cast<size_t*>(mxGetData( plhs[0] )) = handle;
}

void CMD_clear::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    CmexObject::getInstance().writers.remove( handle );
}

void CMD_clearall::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    CmexObject::getInstance().writers.clear( );
}

void CMD_getheader::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    plhs[0] = constructHeaderStruct( w );
    Header2Struct( plhs[0], &w->getHeaderAccess_readonly() );
}

void CMD_setheader::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    Struct2Header( prhs[1], &w->getHeaderAccess( ) );
}

void CMD_createsignal::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel = mx::getNumeric<size_t>( prhs[1] );
    if( channel < 1 )
        throw std::invalid_argument( "Channel index must be >= 1." );
    w->createSignal( channel - 1, true );
}

void CMD_recduration::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    double recdur = mx::getNumeric<double>( prhs[1] );
    if( recdur == 0 || !isfinite(recdur) )
        w->getHeaderAccess( ).enableAutoRecordDuration( );
    else
    {
        gdf::uint32 num = boost::numeric_cast<gdf::uint32>( trunc(recdur) );
        gdf::uint32 den = 1;
        while( recdur*den - num != 0 )
        {
            cout << num << "/" << den << endl;
            den *= 10;
            num = boost::numeric_cast<gdf::uint32>( trunc(recdur*den) );
        }
        w->getHeaderAccess( ).setRecordDuration( num, den );
    }
}

void CMD_eventconfig::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    gdf::uint8 mode = mx::getNumeric<gdf::uint8>( prhs[1] );
    gdf::float32 fs = mx::getNumeric<gdf::float32>( prhs[2] );
    w->setEventMode( mode );
    w->setEventSamplingRate( fs );
}

void CMD_addsample::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel_idx = mx::getNumeric<size_t>( prhs[1] );
    double value = mx::getNumeric<double>( prhs[2] );
    w->addSamplePhys( channel_idx-1, value );
}

void CMD_addrawsample::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel_idx = mx::getNumeric<size_t>( prhs[1] );
    double value = mx::getNumeric<double>( prhs[2] );
    w->addSampleRaw( channel_idx-1, value );
}

void CMD_addsamplevector::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t first_channel = mx::getNumeric<size_t>( prhs[1] ) - 1;
    std::vector<double> vals = mx::getNumericArray<double>( prhs[2] );
    for( size_t i=0; i<vals.size(); i++ )
        w->addSamplePhys( first_channel+i, vals[i] );
}

void CMD_blitsamples::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel_idx = mx::getNumeric<size_t>( prhs[1] );
    std::vector<double> vals = mx::getNumericArray<double>( prhs[2] );
    w->blitSamplesPhys( channel_idx-1, vals );
}

void CMD_blitrawsamples::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel_idx = mx::getNumeric<size_t>( prhs[1] );
    std::vector<double> vals = mx::getNumericArray<double>( prhs[2] );
    w->blitSamplesRaw( channel_idx-1, vals );
}

void CMD_mode1event::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    gdf::uint32 pos = mx::getNumeric<gdf::uint32>( prhs[1] );
    gdf::uint16 type = mx::getNumeric<gdf::uint16>( prhs[2] );
    w->addEvent( pos, type );
}

void CMD_mode3event::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    gdf::uint32 pos = mx::getNumeric<gdf::uint32>( prhs[1] );
    gdf::uint16 type = mx::getNumeric<gdf::uint16>( prhs[2] );
    gdf::uint16 chan = mx::getNumeric<gdf::uint16>( prhs[3] );
    gdf::uint32 dur = mx::getNumeric<gdf::uint32>( prhs[4] );
    w->addEvent( pos, type, chan-1, dur );
}

void CMD_open::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    std::string filename = mx::getString( prhs[1] );
    w->open( filename );
}

void CMD_close::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    w->close( );
}

