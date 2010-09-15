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
        set_lowpass( std::numeric_limits<float32>::quiet_NaN() );
        set_highpass( std::numeric_limits<float32>::quiet_NaN() );
        set_notch( std::numeric_limits<float32>::quiet_NaN() );
        set_samples_per_record( 0 );
        set_datatype( INVALID_TYPE );
        sensor_pos[0] = 0;
        sensor_pos[1] = 0;
        sensor_pos[2] = 0;
        set_sensor_info( 0 );
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
}
