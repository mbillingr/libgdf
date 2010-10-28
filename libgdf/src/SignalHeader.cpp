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

#include "GDF/SignalHeader.h"
#include <limits>
#include <memory.h>
//#include <iostream>

namespace gdf {

    SignalHeader::SignalHeader( )
    {
        setDefaultValues( );
    }

    //===================================================================================================
    //===================================================================================================

    SignalHeader::~SignalHeader( )
    {
        //std::cout << "~SignalHeader( )" << std::endl;
    }

    //===================================================================================================
    //===================================================================================================

    void SignalHeader::setDefaultValues( )
    {
        set_label( "                " );
        memset( transducer_type.item, 32, transducer_type.len );
        set_physical_dimension( "      " );
        set_physical_dimension_code( 0x0000 );
        set_physmin( 0.0 );
        set_physmax( 0.0 );
        set_digmin( 0.0 );
        set_digmax( 0.0 );
        memset( reserved_1.item, 0, reserved_1.len );
        set_lowpass( std::numeric_limits<float32>::quiet_NaN() );
        set_highpass( std::numeric_limits<float32>::quiet_NaN() );
        set_notch( std::numeric_limits<float32>::quiet_NaN() );
        set_samples_per_record( 0 );
        set_datatype( INVALID_TYPE );
        sensor_pos[0] = 0;
        sensor_pos[1] = 0;
        sensor_pos[2] = 0;
        set_sensor_info( 0 );
        memset( reserved_2.item, 0, reserved_2.len );
    }

    //===================================================================================================
    //===================================================================================================

    double SignalHeader::phys_to_raw( const double phy ) const
    {
        double digmin = get_digmin( );
        double digmax = get_digmax( );
        double physmin = get_physmin( );
        double physmax = get_physmax( );
        return (phy-physmin)*(digmax-digmin)/(physmax-physmin)+digmin;
    }

    //===================================================================================================
    //===================================================================================================

    double SignalHeader::raw_to_phys( const double raw ) const
    {
        double digmin = get_digmin( );
        double digmax = get_digmax( );
        double physmin = get_physmin( );
        double physmax = get_physmax( );
        return (raw-digmin)*(physmax-physmin)/(digmax-digmin)+physmin;
    }

    //===================================================================================================
    //===================================================================================================

    void SignalHeader::copyFrom( const SignalHeader &other )
    {
        set_label( other.get_label() );
        memcpy( transducer_type.item, other.transducer_type.item, transducer_type.len );
        set_physical_dimension( other.get_physical_dimension() );
        set_physical_dimension_code( other.get_physical_dimension_code() );
        set_physmin( other.get_physmin() );
        set_physmax( other.get_physmax() );
        set_digmin( other.get_digmin() );
        set_digmax( other.get_digmax() );
        set_lowpass( other.get_lowpass() );
        set_highpass( other.get_highpass() );
        set_notch( other.get_notch() );
        set_samples_per_record( other.get_samples_per_record() );
        set_datatype( other.get_datatype() );
        sensor_pos[0] = other.sensor_pos[0];
        sensor_pos[1] = other.sensor_pos[1];
        sensor_pos[2] = other.sensor_pos[2];
        set_sensor_info( other.get_sensor_info() );
        set_samplerate( other.get_samplerate() );
    }
}
