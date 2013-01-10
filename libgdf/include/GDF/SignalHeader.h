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

#ifndef __SIGNALHEADER_H_INCLUDED__
#define __SIGNALHEADER_H_INCLUDED__

#include "HeaderItem.h"
#include <boost/numeric/conversion/cast.hpp>

namespace gdf
{
    class GDFHeaderAccess;

    /// Contains all information required to construct the GDF signal header
    class SignalHeader
    {
        GDF_DECLARE_HEADERSTRING( label, 0, 16 )
        GDF_DECLARE_HEADERSTRING( transducer_type, 16, 80)
        GDF_DECLARE_HEADERSTRING( physical_dimension, 96, 6)  // the spec defines this as a 6 byte long char[8]
        GDF_DECLARE_HEADERITEM( physical_dimension_code, uint16, 102 )
        GDF_DECLARE_HEADERITEM( physmin, float64, 104 )
        GDF_DECLARE_HEADERITEM( physmax, float64, 112 )
        GDF_DECLARE_HEADERITEM( digmin, float64, 120 )
        GDF_DECLARE_HEADERITEM( digmax, float64, 128 )
        GDF_DECLARE_RESERVED( reserved_1, 136, 68 )
        GDF_DECLARE_HEADERITEM( lowpass, float32, 204 )
        GDF_DECLARE_HEADERITEM( highpass, float32, 208 )
        GDF_DECLARE_HEADERITEM( notch, float32, 212 )
        GDF_DECLARE_HEADERITEM_PRIVATE( samples_per_record, uint32, 216 )
        GDF_DECLARE_HEADERITEM( datatype, uint32, 220 )
        GDF_DECLARE_HEADERARRAY( sensor_pos, float32, 224, 3 )
        GDF_DECLARE_HEADERITEM( sensor_info, float32, 236 )
        GDF_DECLARE_RESERVED( reserved_2, 240, 16 )

/*        GDF_BEGIN_HEADERMAP( )
            GDF_ASSIGN_HEADERARRAY( label )
            GDF_ASSIGN_HEADERARRAY( transducer_type )
            GDF_ASSIGN_HEADERARRAY( physical_dimension )
            GDF_ASSIGN_HEADERITEM( physical_dimension_code )
            GDF_ASSIGN_HEADERITEM( physmin )
            GDF_ASSIGN_HEADERITEM( physmax )
            GDF_ASSIGN_HEADERITEM( digmin )
            GDF_ASSIGN_HEADERITEM( digmax )
            GDF_ASSIGN_RESERVED( reserved_1 )
            GDF_ASSIGN_HEADERITEM( lowpass )
            GDF_ASSIGN_HEADERITEM( highpass )
            GDF_ASSIGN_HEADERITEM( notch )
            GDF_ASSIGN_HEADERITEM( samples_per_record )
            GDF_ASSIGN_HEADERITEM( datatype )
            GDF_ASSIGN_HEADERARRAY( sensor_pos )
            GDF_ASSIGN_HEADERITEM( sensor_info )
        GDF_END_HEADERMAP( )*/

    public:

        /// Constructor
        SignalHeader( );

        /// Destructor
        virtual ~SignalHeader( );

        /// Assign a string valued item
        void setString( std::string item, std::string value )
        {
            if( item == "label" ) set_label( value );
            else if( item == "transducer_type" ) set_transducer_type( value );
            else if( item == "physical_dimension" ) set_physical_dimension( value );
            else throw exception::general( "Bad assignment of a string to " + item );
        }

        /// get value of a string item
        std::string getString( std::string item ) const
        {
            if( item == "label" ) return get_label( );
            else if( item == "transducer_type" ) return get_transducer_type( );
            else if( item == "physical_dimension" ) return get_physical_dimension( );
            else throw exception::general( "Bad attempt to read "+item+" as string" );
        }

        /// Assign a numeric item
        template<typename T> void setNumeric( std::string item, T value )
        {
            using boost::numeric_cast;
            if( item == "physical_dimension_code" ) set_physical_dimension_code( numeric_cast<uint16>(value) );
            else if( item == "physmin" ) set_physmin( numeric_cast<float64>(value) );
            else if( item == "physmax" ) set_physmax( numeric_cast<float64>(value) );
            else if( item == "digmin" ) set_digmin( numeric_cast<float64>(value) );
            else if( item == "digmax" ) set_digmax( numeric_cast<float64>(value) );
            else if( item == "lowpass" ) set_lowpass( numeric_cast<float32>(value) );
            else if( item == "highpass" ) set_highpass( numeric_cast<float32>(value) );
            else if( item == "notch" ) set_notch( numeric_cast<float32>(value) );
            else if( item == "datatype" ) set_datatype( numeric_cast<uint32>(value) );
            else if( item == "sensor_info" ) set_sensor_info( numeric_cast<float32>(value) );
            else throw exception::general( "Bad assignment to " + item );
        }

        /// get value of a numeric item
        template<typename T> T getNumeric( std::string item ) const
        {
            using boost::numeric_cast;
            if( item == "physical_dimension_code" ) return numeric_cast<T>(get_physical_dimension_code( ));
            else if( item == "physmin" ) return numeric_cast<T>(get_physmin( ));
            else if( item == "physmax" ) return numeric_cast<T>(get_physmax( ));
            else if( item == "digmin" ) return numeric_cast<T>(get_digmin( ));
            else if( item == "digmax" ) return numeric_cast<T>(get_digmax( ));
            else if( item == "lowpass" ) return numeric_cast<T>(get_lowpass( ));
            else if( item == "highpass" ) return numeric_cast<T>(get_highpass( ));
            else if( item == "notch" ) return numeric_cast<T>(get_notch( ));
            else if( item == "datatype" ) return numeric_cast<T>(get_datatype( ));
            else if( item == "sensor_info" ) return numeric_cast<T>(get_sensor_info( ));
            else throw exception::general( item + " is not known as a numeric item" );
        }

        /// Assign a numeric array item
        template<typename T> void setArray( std::string item, size_t array_index, T value )
        {
            using boost::numeric_cast;
            if( item == "sensor_pos" ) sensor_pos[array_index] = numeric_cast<float32>(value);
            else throw exception::general( "Bad array access to " + item );
        }

        /// set header items to default values
        void setDefaultValues( );

        /// copy another Signal Header
        void copyFrom( const SignalHeader &other );

        /// Converts from physical units to raw digital representation
        double phys_to_raw( const double phy ) const;

        /// Converts from raw digital representation to physical units
        double raw_to_phys( const double raw ) const;

        void set_samplerate( uint32 fs ) { samplerate = fs; }
        uint32 get_samplerate( ) const { return samplerate; }

    private:
        uint32 samplerate;

        friend class GDFHeaderAccess;
        friend std::ostream& operator<< (std::ostream& out, const GDFHeaderAccess& hdr);
        friend std::istream& operator>> (std::istream& in, GDFHeaderAccess& hdr);
    };
}

#endif
