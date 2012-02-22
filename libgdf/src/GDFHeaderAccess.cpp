//
// This file is part of libGDF.
//
// libGDF is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// libGDF is distributed in the hope that it will be useful,
// but WITHout ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Martin Billinger

#include "GDF/GDFHeaderAccess.h"
#include "GDF/tools.h"
#include "GDF/Exceptions.h"
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <list>
#include <string>
//#include <iostream>

const double epsilon_d = 1e-15;
const double epsilon_f = 1e-15;
const double epsilon_i = 1;


namespace gdf
{
    GDFHeaderAccess::GDFHeaderAccess( )
    {
        enableAutoRecordDuration( );
        setLock( false );
    }

    //===================================================================================================
    //===================================================================================================

    GDFHeaderAccess::~GDFHeaderAccess( )
    {
        //std::cout << "~GDFHeaderAccess( )" << std::endl;
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::clear( )
    {
        assert( !m_lock );
        m_mainhdr.setDefaultValues( );
        m_sighdr.clear( );
        m_events.clear( );
        enableAutoRecordDuration( );
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::sanitize( )
    {
        using boost::lexical_cast;

        std::list< std::string > warnings, errors;

        // ------------ Check number of signals ------------------
        if( m_sighdr.size() == 0 )
            warnings.push_back( "GDF will contain no signals." );

        m_mainhdr.set_num_signals( m_sighdr.size() );

        // ------------ Check signal indices ------------------
        std::map<uint16,SignalHeader>::iterator it = m_sighdr.begin( );
        std::string tmp;
        for( size_t i=0; it != m_sighdr.end(); it++, i++ )
            while( it->first != i )
                tmp = tmp + lexical_cast<std::string>(i++)+',';
        if( tmp.length() > 0 )
        {
            tmp = tmp.substr(0,tmp.length()-1);
            errors.push_back( "Signal indices not contiguous (Missing signals number "+tmp+")." );
        }

        // ------------ Gather Sampling rates ------------------
        std::vector<uint32> fs;
        for( it=m_sighdr.begin( ); it!=m_sighdr.end(); it++ )
        {
            uint32 rate = it->second.get_samplerate( );
            fs.push_back( rate );
        }

        uint32 ddur[2];
        if( fs.size() == 0 )
        {
            // ------------ No Signals: set record duration to 0 ------------------
            ddur[0] = 0;
            ddur[1] = 1;
        }
        else if( drd_den == 0 )
        {
            // ------------ Automatically Set datarecord duration ------------------
            ddur[0] = 1;
            ddur[1] =gcd( fs );

            if( ddur[1] == 0 )  // invalid sampling rates, set record duration to 0
            {
                ddur[0] = 0;
                ddur[1] = 1;
            }
        }
        else
        {
            // ------------ Manually Set datarecord duration ------------------
            if( (gcd(fs) * drd_num) % drd_den != 0 )
                errors.push_back( "(Manually set) record duration does not fit sample rates." );
            ddur[0] = drd_num;
            ddur[1] = drd_den;
        }
        m_mainhdr.set_datarecord_duration( ddur, 2 );

        // ------------ Signal specific information ------------------
        for( it=m_sighdr.begin( ); it!=m_sighdr.end(); it++ )
        {
            // ------------ Check data type ------------------
            if( it->second.get_datatype() == INVALID_TYPE )
                errors.push_back( "Signal "+lexical_cast<std::string>(it->first)+" has no valid data type." );

            // ------------ Check sampling rate ------------------
            it->second.set_samples_per_record( it->second.get_samplerate() * ddur[0] / ddur[1] );
            if( it->second.get_samples_per_record() == 0 )
                errors.push_back( "Signal "+lexical_cast<std::string>(it->first)+" has sample rate set to 0 or not set at all." );

            // ------------ Check ranges ------------------
            if( it->second.get_digmin() > it->second.get_digmax() )
                errors.push_back( "Signal "+lexical_cast<std::string>(it->first)+" has digmin > digmax." );

            if( it->second.get_digmin() == it->second.get_digmax() )
                errors.push_back( "Signal "+lexical_cast<std::string>(it->first)+" has digmin = digmax." );

            if( it->second.get_physmin() == it->second.get_physmax() )
                errors.push_back( "Signal "+lexical_cast<std::string>(it->first)+" has physmin = physmax." );
        }

        m_mainhdr.set_header_length( 1 + m_sighdr.size() + m_taghdr.getLength() );


        if( m_events.getMode() != 1 && m_events.getMode() != 3 )
            errors.push_back( "Event Mode "+lexical_cast<std::string>(m_events.getMode())+" is not valid.");

        if( m_events.getSamplingRate() < 0 )
        {
            if( fs.size() > 0)
                m_events.setSamplingRate( boost::numeric_cast<float32>( max(fs) ) );
            else
                errors.push_back( "Could not determine event sampling rate." );
        }

        if( warnings.size() > 0 || errors.size() > 0 )
            throw exception::header_issues( warnings, errors );
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::setRecordDuration( uint32 num, uint32 den )
    {
        uint32 ddur[2];
        drd_num = num;
        drd_den = den;
        ddur[0] = num;
        ddur[1] = den;
        m_mainhdr.set_datarecord_duration( ddur, 2 );
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::enableAutoRecordDuration( )
    {
        drd_num = 0;
        drd_den = 0;
    }

    //===================================================================================================
    //===================================================================================================

    const MainHeader &GDFHeaderAccess::getMainHeader_readonly( ) const
    {
        return m_mainhdr;
    }

    //===================================================================================================
    //===================================================================================================

    MainHeader &GDFHeaderAccess::getMainHeader( )
    {
        assert( !m_lock );
        return m_mainhdr;
    }

    //===================================================================================================
    //===================================================================================================

    const SignalHeader &GDFHeaderAccess::getSignalHeader_readonly( size_t idx ) const
    {
        std::map<uint16,SignalHeader>::const_iterator it = m_sighdr.find( idx );
        if( it == m_sighdr.end() )
            throw exception::signal_exists_not( boost::lexical_cast<std::string>( idx ) );
        return it->second;
    }

    //===================================================================================================
    //===================================================================================================

    SignalHeader &GDFHeaderAccess::getSignalHeader( size_t idx )
    {
        //throw exception::signal_exists_not( boost::lexical_cast<std::string>( idx ) );
        assert( !m_lock );
        std::map<uint16,SignalHeader>::iterator it = m_sighdr.find( idx );
        if( it == m_sighdr.end() )
            //return m_sighdr.begin()->second;
            throw exception::signal_exists_not( boost::lexical_cast<std::string>( idx ) );
        return it->second;
    }

    //===================================================================================================
    //===================================================================================================

    bool GDFHeaderAccess::createSignal( size_t index, bool throwexc )
    {
        assert( !m_lock );
        std::map<uint16,SignalHeader>::iterator it = m_sighdr.find( index );
        if( it != m_sighdr.end() )
        {
            if( throwexc )
                throw exception::signal_exists( boost::lexical_cast<std::string>( index ) );
            else
                return false;
        }
        m_sighdr[index];
        return true;
    }

    //===================================================================================================
    //===================================================================================================

    size_t GDFHeaderAccess::getFirstFreeSignalIndex( )
    {
        size_t index = 0;
        std::map<uint16,SignalHeader>::const_iterator it = m_sighdr.begin( );
        for( ; it!=m_sighdr.end(); it++ )
        {
            if( it->first != index )
                return index;
            index++;
        }
        return index;
    }

    //===================================================================================================
    //===================================================================================================

    size_t GDFHeaderAccess::getNumSignals( ) const
    {
        return m_sighdr.size( );
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::swapSignals( size_t a, size_t b )
    {
        assert( !m_lock );
        std::map<uint16,SignalHeader>::iterator ita = m_sighdr.find( a );
        std::map<uint16,SignalHeader>::iterator itb = m_sighdr.find( b );

        if( ita == m_sighdr.end() )
            throw exception::signal_exists_not( boost::lexical_cast<std::string>( a ) );
        if( itb == m_sighdr.end() )
            throw exception::signal_exists_not( boost::lexical_cast<std::string>( b ) );

        std::swap( ita->second, itb->second );
    }

    //===================================================================================================
    //===================================================================================================

    void GDFHeaderAccess::relocateSignal( size_t src, size_t dst )
    {
        assert( !m_lock );
        std::map<uint16,SignalHeader>::iterator ita = m_sighdr.find( src );
        std::map<uint16,SignalHeader>::iterator itb = m_sighdr.find( dst );

        if( ita == m_sighdr.end() )
            throw exception::signal_exists_not( boost::lexical_cast<std::string>( src ) );
        if( itb != m_sighdr.end() )
            throw exception::signal_exists( boost::lexical_cast<std::string>( dst ) );

        m_sighdr[dst] = ita->second;
        m_sighdr.erase( ita );
    }

    //===================================================================================================
    //===================================================================================================

    EventHeader &GDFHeaderAccess::getEventHeader( )
    {
        return m_events;
    }

    //===================================================================================================
    //===================================================================================================

    std::ostream& operator<< (std::ostream& out, const GDFHeaderAccess& hdr)
    {
        const MainHeader *mh = &hdr.m_mainhdr;
        mh->version_id.tostream( out );
        mh->patient_id.tostream( out );
        mh->reserved_1.tostream( out );
        mh->patient_drugs.tostream( out );
        mh->patient_weight.tostream( out );
        mh->patient_height.tostream( out );
        mh->patient_flags.tostream( out );
        mh->recording_id.tostream( out );
        mh->recording_location.tostream( out );
        mh->recording_start.tostream( out );
        mh->patient_birthday.tostream( out );
        mh->header_length.tostream( out );
        mh->patient_ICD.tostream( out );
        mh->equipment_provider_classification.tostream( out );
        mh->reserved_2.tostream( out );
        mh->patient_headsize.tostream( out );
        mh->pos_reference.tostream( out );
        mh->pos_ground.tostream( out );
        mh->num_datarecords.tostream( out );
        mh->datarecord_duration.tostream( out );
        mh->num_signals.tostream( out );
        mh->reserved_3.tostream( out );

        assert( out.tellp() == std::streampos(256) );

        size_t ns = mh->get_num_signals( );

        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).label.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).transducer_type.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).physical_dimension.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).physical_dimension_code.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).physmin.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).physmax.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).digmin.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).digmax.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).reserved_1.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).lowpass.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).highpass.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).notch.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).samples_per_record.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).datatype.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).sensor_pos.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).sensor_info.tostream( out );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader_readonly(i).reserved_2.tostream( out );

        assert( out.tellp() == std::streampos(256+256*ns) );

        //throw exception::feature_not_implemented( );

        return out;
    }

    //===================================================================================================
    //===================================================================================================

    std::istream& operator>> (std::istream& in, GDFHeaderAccess& hdr)
    {
        hdr.clear( );

        MainHeader *mh = &hdr.m_mainhdr;
        mh->version_id.fromstream( in );
        if (mh->get_version_id() != "GDF 2.10")
            throw exception::incompatible_gdf_version (mh->get_version_id ());
        mh->patient_id.fromstream( in );
        mh->reserved_1.fromstream( in );
        mh->patient_drugs.fromstream( in );
        mh->patient_weight.fromstream( in );
        mh->patient_height.fromstream( in );
        mh->patient_flags.fromstream( in );
        mh->recording_id.fromstream( in );
        mh->recording_location.fromstream( in );
        mh->recording_start.fromstream( in );
        mh->patient_birthday.fromstream( in );
        mh->header_length.fromstream( in );
        mh->patient_ICD.fromstream( in );
        mh->equipment_provider_classification.fromstream( in );
        mh->reserved_2.fromstream( in );
        mh->patient_headsize.fromstream( in );
        mh->pos_reference.fromstream( in );
        mh->pos_ground.fromstream( in );
        mh->num_datarecords.fromstream( in );
        mh->datarecord_duration.fromstream( in );
        mh->num_signals.fromstream( in );
        mh->reserved_3.fromstream( in );

        assert( in.tellg() == std::streampos(256) );

        size_t ns = mh->get_num_signals( );

        for( uint16 i=0; i<ns; i++ )
            hdr.createSignal( i );

        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).label.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).transducer_type.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).physical_dimension.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).physical_dimension_code.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).physmin.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).physmax.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).digmin.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).digmax.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).reserved_1.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).lowpass.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).highpass.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).notch.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).samples_per_record.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).datatype.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).sensor_pos.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).sensor_info.fromstream( in );
        for( uint16 i=0; i<ns; i++ ) hdr.getSignalHeader(i).reserved_2.fromstream( in );

        assert( in.tellg() == std::streampos(256+256*ns) );

        return in;
    }

    //===================================================================================================
    //===================================================================================================

    size_t GDFHeaderAccess::getNumberOfSamplesInSignal( size_t signal_index ) const
    {
        std::map<uint16,SignalHeader>::const_iterator it = m_sighdr.find( signal_index );
        if( it == m_sighdr.end() )
            return 0;

        return boost::numeric_cast<size_t>(m_mainhdr.get_num_datarecords()) * boost::numeric_cast<size_t>(it->second.get_samples_per_record());
    }
}
