#ifndef __GDF_MEX_H__
#define __GDF_MEX_H__

#include "gdf_headerstruct.h"
#include "matlab_tools/mxStructAccess.h"
#include <GDF/Writer.h>
#include <GDF/tools.h>
#include <stdexcept>
#include <map>
#include "mex.h"

#define STR_DRUGS_00 "UNKNOWN"
#define STR_DRUGS_01 "NO"
#define STR_DRUGS_10 "YES"
#define STR_DRUGS_11 "INVALID"

#define STR_GENDR_00 "UNKNOWN"
#define STR_GENDR_01 "MALE"
#define STR_GENDR_10 "FEMALE"
#define STR_GENDR_11 "INVALID"

#define STR_HANDS_00 "UNKNOWN"
#define STR_HANDS_01 "RIGHT"
#define STR_HANDS_10 "LEFT"
#define STR_HANDS_11 "EQUAL"

#define STR_VIS_00 "UNKNOWN"
#define STR_VIS_01 "NO"
#define STR_VIS_10 "YES"
#define STR_VIS_11 "YES (CORRECTED)"

#define STR_HEART_00 "UNKNOWN"
#define STR_HEART_01 "NO"
#define STR_HEART_10 "YES"
#define STR_HEART_11 "PACEMAKER"

#define DATE_CORRECTION_FACTOR 4294967296.0

// =================================================================================================
// =================================================================================================

void warn( const std::string &str )
{
    mexPrintf( (str+"\n").c_str() );
}

// =================================================================================================
// =================================================================================================

void bitDecode( char *dest, gdf::uint8 data, int bit1, int bit2, const char *str1, const char *str2, const char *str3, const char *str4 )
{
        bit1 = 1 << bit1;
        bit2 = 1 << bit2;

        bool b1 = data & bit1;
        bool b2 = data & bit2;

        if( !b1 && !b2 )
                strcpy( dest, str1 );
        if( b1 && !b2 )
                strcpy( dest, str2 );
        if( !b1 && b2 )
                strcpy( dest, str3 );
        if( b1 && b2 )
                strcpy( dest, str4 );
}

// =================================================================================================
// =================================================================================================

void bitEncode( gdf::uint8 &data, const char *src, int bit1, int bit2, const char *str1, const char *str2, const char *str3, const char *str4 )
{
        bit1 = 1 << bit1;
        bit2 = 1 << bit2;

        if( strcmp( src, str1 ) == 0 )
        {
                data = data & ~bit1;
                data = data & ~bit2;
        }
        else if( strcmp( src, str2 ) == 0 )
        {
                data = data | bit1;
                data = data & ~bit2;
        }
        else if( strcmp( src, str3 ) == 0 )
        {
                data = data & ~bit1;
                data = data | bit2;
        }
        else if( strcmp( src, str4 ) == 0 )
        {
                data = data | bit1;
                data = data | bit2;
        }
        else
            throw std::invalid_argument( "Unknown value: " + std::string( src ) );
}

// =================================================================================================
// =================================================================================================

// encoding of location as described in http://www.faqs.org/rfcs/rfc1876.html
void uint32_to_RFC( const gdf::uint32 loc, gdf::uint8 &version, double &size, double &hprec, double &vprec )
{
        version = ((char*)&loc)[0];

        if( version != 0 )
                throw std::invalid_argument( "RFC1876 expects 'version' to be set to 0." );

        gdf::uint8 mantissa = ((char*)&loc)[1];
        gdf::uint8 exponent = ((char*)&loc)[1];
        mantissa = mantissa >> 4;
        exponent = exponent << 4;
        exponent = exponent >> 4;
        size = mantissa * pow( 10.0, exponent );

        mantissa = ((char*)&loc)[2];
        exponent = ((char*)&loc)[2];
        mantissa = mantissa >> 4;
        exponent = exponent << 4;
        exponent = exponent >> 4;
        hprec = mantissa * pow( 10.0, exponent );

        mantissa = ((char*)&loc)[3];
        exponent = ((char*)&loc)[3];
        mantissa = mantissa >> 4;
        exponent = exponent << 4;
        exponent = exponent >> 4;
        vprec = mantissa * pow( 10.0, exponent );
}

// =================================================================================================
// =================================================================================================

void RFC_to_uint32( const gdf::uint32 &loc, gdf::uint8 version, double size, double hprec, double vprec )
{
    using boost::numeric_cast;

    try {
        ((char*)&loc)[0] = version;

        if( version != 0 )
                throw std::invalid_argument( "RFC1876 expects 'version' to be set to 0." );

        if( size < 1 )
            ((char*)&loc)[1] = 0;
        else
        {
            gdf::uint8 exponent = numeric_cast<gdf::uint8>( floor( log10( size ) ) );
            gdf::uint8 mantissa = numeric_cast<gdf::uint8>( floor( size/pow( 10.0,  exponent ) ) );
            mantissa = mantissa << 4;
            ((char*)&loc)[1] = mantissa | exponent;
        }

        if( hprec < 1 )
            ((char*)&loc)[2] = 0;
        else
        {
            gdf::uint8 exponent = numeric_cast<gdf::uint8>( floor( log10( hprec ) ) );
            gdf::uint8 mantissa = numeric_cast<gdf::uint8>( floor( hprec/pow( 10.0,  exponent ) ) );
            mantissa = mantissa << 4;
            ((char*)&loc)[2] = mantissa | exponent;
        }

        if( vprec < 1 )
            ((char*)&loc)[3] = 0;
        else
        {
            gdf::uint8 exponent = numeric_cast<gdf::uint8>( floor( log10( vprec ) ) );
            gdf::uint8 mantissa = numeric_cast<gdf::uint8>( floor( vprec/pow( 10.0,  exponent ) ) );
            mantissa = mantissa << 4;
            ((char*)&loc)[3] = mantissa | exponent;
        }
    }
    catch( boost::bad_numeric_cast &e )
    {
        throw std::invalid_argument( std::string("Converting Location: ")+e.what() );
    }
}

// =================================================================================================
// =================================================================================================

mxArray *constructHeaderStruct( gdf::Writer *w )
{
        mxArray *h;

        size_t num_signals = w->getNumSignals( );

        // most of the following commands are not really neccessary
        // but pre-creating the structure allows to determine the order
        // of elements in one place.

        h = mxCreateStructMatrix( 1, 1, 0, NULL );
        mx::setField( h, NULL, GDFH_STRUCT_FILE_VersionID );
        mx::setField( h, NULL, GDFH_STRUCT_FILE_HeaderLength );
        mx::setField( h, NULL, GDFH_STRUCT_FILE_NumDatarecords );
        mx::setField( h, NULL, GDFH_STRUCT_FILE_DatarecordsDuration );
        mx::setField( h, NULL, GDFH_STRUCT_FILE_NumSignals );

        mx::setField( h, NULL, GDFH_STRUCT_REC_ID );
        mx::setField( h, NULL, GDFH_STRUCT_REC_LOC );
        mx::setField( h, NULL, GDFH_STRUCT_REC_START );
        mx::setField( h, NULL, GDFH_STRUCT_REC_EQUIPMENT );
        mx::setField( h, NULL, GDFH_STRUCT_REC_PosRef );
        mx::setField( h, NULL, GDFH_STRUCT_REC_PosGND );

        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_IdentificationCode );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Name );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Classification );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Weight );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Height );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Flags );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Drugs );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_Birthdate );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_ICD );
        mx::setField( h, NULL, GDFH_STRUCT_PATIENT_HeadSize );

        if( num_signals > 0 )
        {
            mx::setField( h, mxCreateStructMatrix( num_signals, 1 , 0, NULL ), GDFH_STRUCT_SIGNAL );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Label );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Transducer );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_PhysDim );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_PhysCode );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_PhysMin );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_PhysMax );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_DigMin );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_DigMax );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Lowpass );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Highpass );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Notch );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_RecSamples );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_SampleRate );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Datatype );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_SensorPos );
            mx::setField( h, NULL, GDFH_STRUCT_SIGNAL+std::string(".")+GDFH_STRUCT_SIGNAL_Sensinf );
        }

        mx::setField( h, NULL, GDFH_STRUCT_TLV );

        return h;
}

// =================================================================================================
// =================================================================================================

void MainHeader2Struct( mxArray *h, const gdf::MainHeader *mainhdr )
{
    mx::setFieldString( h, mainhdr->get_version_id(), GDFH_STRUCT_FILE_VersionID );
    mx::setFieldNumeric( h, mainhdr->get_header_length(), GDFH_STRUCT_FILE_HeaderLength );
    mx::setFieldNumeric( h, mainhdr->get_num_datarecords(), GDFH_STRUCT_FILE_NumDatarecords );
    mx::setFieldNumeric( h, static_cast<double>(mainhdr->get_datarecord_duration(0)) / static_cast<double>(mainhdr->get_datarecord_duration(1)), GDFH_STRUCT_FILE_DatarecordsDuration );
    mx::setFieldNumeric( h, mainhdr->get_num_signals(), GDFH_STRUCT_FILE_NumSignals );

    std::stringstream ss( mainhdr->get_patient_id() );
    std::string str;
    ss >> str;
    mx::setFieldString( h, str, GDFH_STRUCT_PATIENT_IdentificationCode );
    ss >> str;
    mx::setFieldString( h, str, GDFH_STRUCT_PATIENT_Name );
    ss >> str;
    mx::setFieldString( h, str, GDFH_STRUCT_PATIENT_Classification );

    char tmp[128];

    bitDecode( tmp, mainhdr->get_patient_drugs(), 0, 1, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Drugs_Smoking );

    bitDecode( tmp, mainhdr->get_patient_drugs(), 2, 3, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Drugs_Alcohol );

    bitDecode( tmp, mainhdr->get_patient_drugs(), 4, 5, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Drugs_Drugs );

    bitDecode( tmp, mainhdr->get_patient_drugs(), 6, 7, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Drugs_Medics );

    mx::setFieldNumeric( h, mainhdr->get_patient_weight(), GDFH_STRUCT_PATIENT_Weight );
    mx::setFieldNumeric( h, mainhdr->get_patient_height(), GDFH_STRUCT_PATIENT_Height );

    bitDecode( tmp, mainhdr->get_patient_flags(), 0, 1, STR_GENDR_00, STR_GENDR_01, STR_GENDR_10, STR_GENDR_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Flags_Gender );

    bitDecode( tmp, mainhdr->get_patient_flags(), 2, 3, STR_HANDS_00, STR_HANDS_01, STR_HANDS_10, STR_HANDS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Flags_Handed );

    bitDecode( tmp, mainhdr->get_patient_flags(), 4, 5, STR_VIS_00, STR_VIS_01, STR_VIS_10, STR_VIS_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Flags_Visual );

    bitDecode( tmp, mainhdr->get_patient_flags(), 6, 7, STR_HEART_00, STR_HEART_01, STR_HEART_10, STR_HEART_11 );
    mx::setFieldString( h, tmp, GDFH_STRUCT_PATIENT_Flags_Hearty );

    mx::setFieldNumeric( h, boost::numeric_cast<double>(mainhdr->get_patient_birthday()) / DATE_CORRECTION_FACTOR, GDFH_STRUCT_PATIENT_Birthdate );
    mx::setFieldString( h, mainhdr->get_patient_ICD(), GDFH_STRUCT_PATIENT_ICD );

    mxArray *mx_headsize = mxCreateNumericMatrix( 1, 3, mxUINT16_CLASS, mxREAL );
    reinterpret_cast<gdf::uint16*>( mxGetData(mx_headsize) )[0] = mainhdr->get_patient_headsize(0);
    reinterpret_cast<gdf::uint16*>( mxGetData(mx_headsize) )[1] = mainhdr->get_patient_headsize(1);
    reinterpret_cast<gdf::uint16*>( mxGetData(mx_headsize) )[2] = mainhdr->get_patient_headsize(2);
    mx::setField( h, mx_headsize, GDFH_STRUCT_PATIENT_HeadSize );

    mx::setFieldString( h, mainhdr->get_recording_id(), GDFH_STRUCT_REC_ID );

    gdf::uint8 loc_version;
    double loc_size;
    double loc_hp, loc_vp;
    uint32_to_RFC( mainhdr->get_recording_location(0), loc_version, loc_size, loc_hp, loc_vp );

    mx::setFieldNumeric( h, loc_version, GDFH_STRUCT_REC_LOC_VER );
    mx::setFieldNumeric( h, loc_size, GDFH_STRUCT_REC_LOC_SIZE );
    mx::setFieldNumeric( h, loc_hp, GDFH_STRUCT_REC_LOC_HRPEC );
    mx::setFieldNumeric( h, loc_vp, GDFH_STRUCT_REC_LOC_VRPEC );
    mx::setFieldNumeric( h, mainhdr->get_recording_location(1), GDFH_STRUCT_REC_LOC_LAT );
    mx::setFieldNumeric( h, mainhdr->get_recording_location(2), GDFH_STRUCT_REC_LOC_LON );
    mx::setFieldNumeric( h, mainhdr->get_recording_location(3), GDFH_STRUCT_REC_LOC_ALT );

    mx::setFieldNumeric( h, boost::numeric_cast<double>(mainhdr->get_recording_start()) / DATE_CORRECTION_FACTOR, GDFH_STRUCT_REC_START );
    mx::setFieldNumeric( h, mainhdr->get_equipment_provider_classification(), GDFH_STRUCT_REC_EQUIPMENT );

    mxArray *mx_pos = mxCreateNumericMatrix( 1, 3, mxDOUBLE_CLASS, mxREAL );
    mxGetPr(mx_pos)[0] = mainhdr->get_pos_reference(0);
    mxGetPr(mx_pos)[1] = mainhdr->get_pos_reference(1);
    mxGetPr(mx_pos)[2] = mainhdr->get_pos_reference(2);
    mx::setField( h, mx_pos, GDFH_STRUCT_REC_PosRef );

    mx_pos = mxCreateNumericMatrix( 1, 3, mxDOUBLE_CLASS, mxREAL );
    mxGetPr(mx_pos)[0] = mainhdr->get_pos_ground(0);
    mxGetPr(mx_pos)[1] = mainhdr->get_pos_ground(1);
    mxGetPr(mx_pos)[2] = mainhdr->get_pos_ground(2);
    mx::setField( h, mx_pos, GDFH_STRUCT_REC_PosGND );
}

// =================================================================================================
// =================================================================================================

void SignalHeader2Struct( mxArray *h, const gdf::GDFHeaderAccess *headers )
{
    using boost::numeric_cast;

    size_t num_signals = headers->getNumSignals( );

    if( num_signals == 0 )
        return;

    mxArray *sh = mx::getField( h, GDFH_STRUCT_SIGNAL );

    for( size_t i=0; i<num_signals; i++ )
    {
        const gdf::SignalHeader *signal_header = &headers->getSignalHeader_readonly( i );
        mx::setFieldString( sh, signal_header->get_label(), GDFH_STRUCT_SIGNAL_Label, i+1 );
        mx::setFieldString( sh, signal_header->get_transducer_type(), GDFH_STRUCT_SIGNAL_Transducer, i+1 );

        mx::setFieldString( sh, signal_header->get_physical_dimension(), GDFH_STRUCT_SIGNAL_PhysDim, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_physical_dimension_code(), GDFH_STRUCT_SIGNAL_PhysCode, i+1 );

        mx::setFieldNumeric( sh, signal_header->get_physmin(), GDFH_STRUCT_SIGNAL_PhysMin, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_physmax(), GDFH_STRUCT_SIGNAL_PhysMax, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_digmin(), GDFH_STRUCT_SIGNAL_DigMin, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_digmax(), GDFH_STRUCT_SIGNAL_DigMax, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_lowpass(), GDFH_STRUCT_SIGNAL_Lowpass, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_highpass(), GDFH_STRUCT_SIGNAL_Highpass, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_notch(), GDFH_STRUCT_SIGNAL_Notch, i+1 );

        mx::setFieldNumeric( sh, signal_header->get_samples_per_record(), GDFH_STRUCT_SIGNAL_RecSamples, i+1 );
        double fs = numeric_cast<double>(signal_header->get_samples_per_record())
                    * numeric_cast<double>(headers->getMainHeader_readonly().get_datarecord_duration(1))
                    / numeric_cast<double>(headers->getMainHeader_readonly().get_datarecord_duration(0));
        mx::setFieldNumeric( sh, fs, GDFH_STRUCT_SIGNAL_SampleRate, i+1 );
        mx::setFieldNumeric( sh, signal_header->get_datatype(), GDFH_STRUCT_SIGNAL_Datatype, i+1 );

        mxArray *mx_pos = mxCreateNumericMatrix( 1, 3, mxDOUBLE_CLASS, mxREAL );
        mxGetPr(mx_pos)[0] = signal_header->get_sensor_pos(0);
        mxGetPr(mx_pos)[1] = signal_header->get_sensor_pos(1);
        mxGetPr(mx_pos)[2] = signal_header->get_sensor_pos(2);
        mx::setField( sh, mx_pos, GDFH_STRUCT_SIGNAL_SensorPos, i+1 );

        switch( signal_header->get_physical_dimension_code() & 0xFFE0 )
        {
            default : mx::setFieldString( sh, "Unknown", GDFH_STRUCT_SIGNAL_Sensinf_Info, i+1 ); break;
            case 4256 : mx::setFieldString( sh, "Electrode impedance [Ohm]", GDFH_STRUCT_SIGNAL_Sensinf_Info, i+1 ); break;
            case 4288 : mx::setFieldString( sh, "Probe frequency [Hertz]", GDFH_STRUCT_SIGNAL_Sensinf_Info, i+1 ); break;
        }

        mx::setFieldNumeric( sh, signal_header->get_sensor_info(), GDFH_STRUCT_SIGNAL_Sensinf_Value, i+1 );
    }
}

// =================================================================================================
// =================================================================================================

void Header2Struct( mxArray *h, const gdf::GDFHeaderAccess *headers )
{
    MainHeader2Struct( h, &headers->getMainHeader_readonly( ) );
    SignalHeader2Struct( h, headers );
}

// =================================================================================================
// =================================================================================================

void Struct2MainHeader( std::map<std::string,const mxArray*> &elements, gdf::GDFHeaderAccess *headers )
{
    using boost::numeric_cast;

    try
    {

        gdf::MainHeader *mainhdr = &headers->getMainHeader( );

        std::map<std::string,const mxArray*>::iterator it;
        /*it = elements.begin( );
        for( ; it!=elements.end(); it++ )
        {
            std::cout << it->first << std::endl;
        }*/

        it = elements.find( GDFH_STRUCT_FILE_VersionID );
        if( it != elements.end() )
        {
            warn( "Warning: Only GDF 2.10 is supported. ('"+it->first+"' ignored)" );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_FILE_HeaderLength );
        if( it != elements.end() )
        {
            warn( "Warning: '"+it->first+"' ignored." );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_FILE_NumDatarecords );
        if( it != elements.end() )
        {
            warn( "Warning: '"+it->first+"' ignored." );
            elements.erase( it );
        }

        it = elements.find(GDFH_STRUCT_FILE_DatarecordsDuration);
        if( it != elements.end() )
        {
            // convert datarecord duration from double to fraction
            double recduration = mx::getNumeric<double>( it->second );
            elements.erase( it );
            if( recduration != 0 )
            {
                gdf::uint32 num = numeric_cast<gdf::uint32>(recduration*1e6);
                gdf::uint32 den = 1e6;
                gdf::uint32 gcd = gdf::gcd( num, den );
                headers->setRecordDuration( num/gcd, den/gcd );
            }
            else
                headers->enableAutoRecordDuration( );
        }

        it = elements.find( GDFH_STRUCT_FILE_NumSignals );
        if( it != elements.end() )
        {
            warn( "Warning: '"+it->first+"' ignored." );
            elements.erase( it );
        }

        {   // *** Patient ID
            std::string idc, nam, cla;
            std::stringstream ss( mainhdr->get_patient_id() );
            std::string str;
            ss >> idc >> nam >> cla;

            it = elements.find( GDFH_STRUCT_PATIENT_IdentificationCode );
            if( it != elements.end() )
            {
                idc = mx::getString( it->second );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Name );
            if( it != elements.end() )
            {
                nam = mx::getString( it->second );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Classification );
            if( it != elements.end() )
            {
                cla = mx::getString( it->second );
                elements.erase( it );
            }

            ss.str( "" );
            ss << idc << " " << nam << " " << cla;
            mainhdr->set_patient_id( ss.str() );
            if( ss.str().length() > mainhdr->getLength_patient_id() )
                warn( "Warning: Total length of patient identification_code, name and classification is longer than " + boost::lexical_cast<std::string>(mainhdr->getLength_patient_id()-3)+". (truncating)" );
        }

        {   // *** Patient Drugs
            gdf::uint8 bits = mainhdr->get_patient_drugs( );

            it = elements.find( GDFH_STRUCT_PATIENT_Drugs_Smoking );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 0, 1, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Drugs_Alcohol );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 2, 3, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Drugs_Drugs );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 4, 5, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Drugs_Medics );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 6, 7, STR_DRUGS_00, STR_DRUGS_01, STR_DRUGS_10, STR_DRUGS_11 );
                elements.erase( it );
            }

            mainhdr->set_patient_drugs( bits );
        }

        it = elements.find( GDFH_STRUCT_PATIENT_Weight );
        if( it != elements.end() )
        {
            mainhdr->set_patient_weight( mx::getNumeric<gdf::uint8>( it->second ) );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_PATIENT_Height );
        if( it != elements.end() )
        {
            mainhdr->set_patient_height( mx::getNumeric<gdf::uint8>( it->second ) );
            elements.erase( it );
        }

        {   // *** Patient Flags
            gdf::uint8 bits = mainhdr->get_patient_flags( );

            it = elements.find( GDFH_STRUCT_PATIENT_Flags_Gender );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 0, 1, STR_GENDR_00, STR_GENDR_00, STR_GENDR_10, STR_GENDR_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Flags_Handed );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 2, 3, STR_HANDS_00, STR_HANDS_01, STR_HANDS_10, STR_HANDS_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Flags_Visual );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 4, 5, STR_VIS_00, STR_VIS_01, STR_VIS_10, STR_VIS_11 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_PATIENT_Flags_Hearty );
            if( it != elements.end() )
            {
                bitEncode( bits, mx::getString(it->second, mx::TOUPPER).c_str(), 6, 7, STR_HEART_00, STR_HEART_01, STR_HEART_10, STR_HEART_11 );
                elements.erase( it );
            }

            mainhdr->set_patient_flags( bits );
        }

        it = elements.find( GDFH_STRUCT_PATIENT_Birthdate );
        if( it != elements.end() )
        {
            mainhdr->set_patient_birthday(
                    boost::numeric_cast<gdf::uint64>(
                            mx::getNumeric<double>( it->second ) * DATE_CORRECTION_FACTOR
                    )
            );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_PATIENT_ICD );
        if( it != elements.end() )
        {
            mainhdr->set_patient_ICD( mx::getString( it->second ) );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_PATIENT_HeadSize );
        if( it != elements.end() )
        {
            gdf::uint16 headsize[3];
            mx::getNumericArray( headsize, 3, it->second );
            mainhdr->set_patient_headsize( headsize, 3 );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_REC_ID );
        if( it != elements.end() )
        {
            mainhdr->set_recording_id( mx::getString( it->second ) );
            elements.erase( it );
        }

        {   // Recording Location
            gdf::uint8 loc_version;
            double loc_size;
            double loc_hp, loc_vp;
            uint32_to_RFC( mainhdr->get_recording_location(0), loc_version, loc_size, loc_hp, loc_vp );

            it = elements.find( GDFH_STRUCT_REC_LOC_VER );
            if( it != elements.end() )
            {
                loc_version = mx::getNumeric<gdf::uint8>( it->second );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_REC_LOC_SIZE );
            if( it != elements.end() )
            {
                loc_size = mx::getNumeric<double>( it->second );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_REC_LOC_HRPEC );
            if( it != elements.end() )
            {
                loc_hp = mx::getNumeric<double>( it->second );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_REC_LOC_VRPEC );
            if( it != elements.end() )
            {
                loc_vp = mx::getNumeric<double>( it->second );
                elements.erase( it );
            }

            gdf::uint32 loc;
            RFC_to_uint32( loc, loc_version, loc_size, loc_hp, loc_vp );
            mainhdr->set_recording_location( loc, 0 );

            it = elements.find( GDFH_STRUCT_REC_LOC_LAT );
            if( it != elements.end() )
            {
                mainhdr->set_recording_location( mx::getNumeric<double>( it->second ), 1 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_REC_LOC_LON );
            if( it != elements.end() )
            {
                mainhdr->set_recording_location( mx::getNumeric<double>( it->second ), 2 );
                elements.erase( it );
            }

            it = elements.find( GDFH_STRUCT_REC_LOC_ALT );
            if( it != elements.end() )
            {
                mainhdr->set_recording_location( mx::getNumeric<double>( it->second ), 3 );
                elements.erase( it );
            }
        }

        it = elements.find( GDFH_STRUCT_REC_START );
        if( it != elements.end() )
        {
            mainhdr->set_recording_start(
                    boost::numeric_cast<gdf::uint64>(
                            mx::getNumeric<double>( it->second ) * DATE_CORRECTION_FACTOR
                    )
            );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_REC_EQUIPMENT );
        if( it != elements.end() )
        {
            mainhdr->set_equipment_provider_classification( mx::getNumeric<gdf::uint64>( it->second ) );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_REC_PosRef );
        if( it != elements.end() )
        {
            gdf::float32 pos[3];
            mx::getNumericArray( pos, 3, it->second );
            mainhdr->set_pos_reference( pos, 3 );
            elements.erase( it );
        }

        it = elements.find( GDFH_STRUCT_REC_PosGND );
        if( it != elements.end() )
        {
            gdf::float32 pos[3];
            mx::getNumericArray( pos, 3, it->second );
            mainhdr->set_pos_ground( pos, 3 );
            elements.erase( it );
        }
    }
    catch( ... )
    {
        // that's weird. if we don't catch and throw again here, matlab crashes.
        throw;
    }
}

// =================================================================================================
// =================================================================================================

void Struct2SignalHeader( std::map<std::string,const mxArray*> &elements, gdf::GDFHeaderAccess *headers )
{
    using boost::numeric_cast;
    using boost::lexical_cast;

    try
    {
        size_t pre_len = std::string(GDFH_STRUCT_SIGNAL).length();

        std::map<size_t,std::string> indexstrings;

        // find all x, in elements starting with signal(x)
        std::map<std::string,const mxArray*>::iterator it;
        it = elements.begin( );
        for( ; it!=elements.end(); it++ )
        {
            if( it->first.substr( 0, pre_len ) == GDFH_STRUCT_SIGNAL )
            {
                size_t index_begin = it->first.find('(')+1;
                size_t index_len = it->first.find(')') - index_begin;
                if( index_begin == 0 )
                    indexstrings[0] = it->first.substr( 0, pre_len+1 );
                else
                {
                    size_t index = lexical_cast<size_t>( it->first.substr(index_begin, index_len) );
                    indexstrings[index] = it->first.substr( 0, pre_len+index_len+3 );
                }
            }
        }

        std::map<size_t,std::string>::iterator pre_it = indexstrings.begin( );
        for( ; pre_it!=indexstrings.end(); pre_it++ )
        {
            size_t index = pre_it->first;

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Label );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_label( mx::getString( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Transducer );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_transducer_type( mx::getString( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_PhysDim );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_physical_dimension( mx::getString( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_PhysCode );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_physical_dimension_code( mx::getNumeric<gdf::uint16>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_PhysMin );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_physmin( mx::getNumeric<double>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_PhysMax );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_physmax( mx::getNumeric<double>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_DigMin );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_digmin( mx::getNumeric<double>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_DigMax );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_digmax( mx::getNumeric<double>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Lowpass );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_lowpass( mx::getNumeric<gdf::float32>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Highpass );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_highpass( mx::getNumeric<gdf::float32>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Notch );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_notch( mx::getNumeric<gdf::float32>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_RecSamples );
            if( it != elements.end() )
            {
                warn( "Warning: '"+pre_it->second + GDFH_STRUCT_SIGNAL_RecSamples+"' ignored." );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_SampleRate );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_samplerate(mx::getNumeric<gdf::uint32>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Datatype );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_datatype( mx::getNumeric<gdf::uint32>( it->second ) );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_SensorPos );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                gdf::float32 pos[3];
                mx::getNumericArray( pos, 3, it->second );
                h->set_sensor_pos( pos, 3 );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Sensinf_Info );
            if( it != elements.end() )
            {
                warn( "Warning: '"+pre_it->second + GDFH_STRUCT_SIGNAL_Sensinf_Info+"' ignored." );
                elements.erase( it );
            }

            it = elements.find( pre_it->second + GDFH_STRUCT_SIGNAL_Sensinf_Value );
            if( it != elements.end() )
            {
                gdf::SignalHeader *h = &headers->getSignalHeader( index );
                h->set_sensor_info( mx::getNumeric<gdf::float32>( it->second ) );
                elements.erase( it );
            }
        }
    }
    catch( ... )
    {
        // that's weird. if we don't catch and throw again here, matlab crashes.
        throw;
    }
}

// =================================================================================================
// =================================================================================================

void Struct2Header( const mxArray *h, gdf::GDFHeaderAccess *headers )
{
    try
    {
        std::map<std::string,const mxArray*> elements = mx::getFieldRecursive( h );

            /*std::map<std::string,const mxArray*>::iterator it;
            it = elements.begin( );
            for( ; it!=elements.end(); it++ )
            {
                std::cout << it->first << std::endl;
            }*/

        Struct2MainHeader( elements, headers );
        Struct2SignalHeader( elements, headers );

        // If there are any elements left, they were not recognized.
        if( elements.size() > 0 )
        {
            std::stringstream ss;
            ss << "Warning: " << boost::lexical_cast<std::string>( elements.size() ) << " invalid header elements:" << std::endl;
            std::map<std::string,const mxArray*>::iterator it = elements.begin( );
            for( ; it!=elements.end(); it++ )
            {
                ss << "  " << it->first << std::endl;
            }
            mexPrintf( ss.str().c_str() );
        }
    }
    catch( ... )
    {
        // that's weird. if we don't catch and throw again here, matlab crashes.
        throw;
    }
}

#endif // GDF_MEX_H
