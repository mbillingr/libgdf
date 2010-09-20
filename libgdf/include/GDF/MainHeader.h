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

#ifndef __MAINHEADER_H_INCLUDED__
#define __MAINHEADER_H_INCLUDED__

#include "HeaderItem.h"
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace gdf
{
    class GDFHeaderAccess;

    /// Contains all information required to construct the GDF fixed header
    class MainHeader
    {
        GDF_DECLARE_HEADERSTRING( version_id, 0, 8 )
        GDF_DECLARE_HEADERSTRING( patient_id, 8, 66 )
        GDF_DECLARE_RESERVED( reserved_1, 74, 10 )
        GDF_DECLARE_HEADERITEM( patient_drugs, uint8, 84 )
        GDF_DECLARE_HEADERITEM( patient_weight, uint8, 85 )
        GDF_DECLARE_HEADERITEM( patient_height, uint8, 86 )
        GDF_DECLARE_HEADERITEM( patient_flags, uint8, 87 )
        GDF_DECLARE_HEADERSTRING( recording_id, 88, 64 )
        GDF_DECLARE_HEADERARRAY( recording_location, uint32, 152, 4 )
        GDF_DECLARE_HEADERITEM( recording_start, uint64, 168 )     // defined as uint32[2] in the spec
        GDF_DECLARE_HEADERITEM( patient_birthday, uint64, 176 )    // defined as uint32[2] in the spec
        GDF_DECLARE_HEADERITEM( header_length, uint16, 184 )
        GDF_DECLARE_HEADERSTRING( patient_ICD, 186, 6 )       // defined as byte[6] in the spec
        GDF_DECLARE_HEADERITEM( equipment_provider_classification, uint64, 192 )
        GDF_DECLARE_RESERVED( reserved_2, 200, 6 )
        GDF_DECLARE_HEADERARRAY( patient_headsize, uint16, 206, 3 )
        GDF_DECLARE_HEADERARRAY( pos_reference, float32, 212, 3 )
        GDF_DECLARE_HEADERARRAY( pos_ground, float32, 224, 3 )
        GDF_DECLARE_HEADERITEM( num_datarecords, int64, 236 )
        GDF_DECLARE_HEADERARRAY( datarecord_duration, uint32, 244, 2 )
        GDF_DECLARE_HEADERITEM_PRIVATE( num_signals, uint16, 252 )
        GDF_DECLARE_RESERVED( reserved_3, 254, 2 )

        /*GDF_BEGIN_HEADERMAP( )
            GDF_ASSIGN_HEADERARRAY( version_id )
            GDF_ASSIGN_HEADERARRAY( patient_id )
            GDF_ASSIGN_RESERVED( reserved_1 )
            GDF_ASSIGN_HEADERITEM( patient_drugs )
            GDF_ASSIGN_HEADERITEM( patient_weight )
            GDF_ASSIGN_HEADERITEM( patient_height )
            GDF_ASSIGN_HEADERITEM( patient_flags )
            GDF_ASSIGN_HEADERARRAY( recording_id )
            GDF_ASSIGN_HEADERARRAY( recording_location )
            GDF_ASSIGN_HEADERITEM( recording_start )
            GDF_ASSIGN_HEADERITEM( patient_birthday )
            GDF_ASSIGN_HEADERITEM( header_length )
            GDF_ASSIGN_HEADERARRAY( patient_ICD )
            GDF_ASSIGN_HEADERITEM( equipment_provider_classification )
            GDF_ASSIGN_RESERVED( reserved_2 )
            GDF_ASSIGN_HEADERARRAY( patient_headsize )
            GDF_ASSIGN_HEADERARRAY( pos_reference )
            GDF_ASSIGN_HEADERARRAY( pos_ground )
            GDF_ASSIGN_HEADERITEM( num_datarecords )
            GDF_ASSIGN_HEADERARRAY( datarecord_duration )
            GDF_ASSIGN_HEADERITEM( num_signals )
            GDF_ASSIGN_RESERVED( reserved_3 )
        GDF_END_HEADERMAP( )*/

    public:

        /// Constructor
        MainHeader( );

        /// set header items to default values
        void setDefaultValues( );

        /// copy another Main Header
        void copyFrom( const MainHeader &other );

        /// Assign a string valued item
        void setString( std::string item, std::string value )
        {
            if( item == "version_id" ) set_version_id( value );
            else if( item == "patient_id" ) set_patient_id( value );
            else if( item == "recording_id" ) set_recording_id( value );
            else if( item == "patient_ICD" ) set_patient_ICD( value );
            else throw exception::general( "Bad assignment of a string to " + item );
        }

        /// get value of a string item
        std::string getString( std::string item )
        {
            if( item == "version_id" ) return get_version_id( );
            else if( item == "patient_id" ) return get_patient_id( );
            else if( item == "recording_id" ) return get_recording_id( );
            else if( item == "patient_ICD" ) return get_patient_ICD( );
            else throw exception::general( "Bad attempt to read "+item+" as string" );
        }

        /// Assign a numeric item
        template<typename T> void setNumeric( std::string item, T value )
        {
            using boost::numeric_cast;
            if( item == "patient_drugs" ) set_patient_drugs( numeric_cast<uint8>(value) );
            else if( item == "patient_weight" ) set_patient_weight( numeric_cast<uint8>(value) );
            else if( item == "patient_height" ) set_patient_height( numeric_cast<uint8>(value) );
            else if( item == "patient_flags" ) set_patient_flags( numeric_cast<uint8>(value) );
            else if( item == "recording_start" ) set_recording_start( numeric_cast<uint64>(value) );
            else if( item == "patient_birthday" ) set_patient_birthday( numeric_cast<uint64>(value) );
            else if( item == "header_length" ) set_header_length( numeric_cast<uint16>(value) );
            else if( item == "equipment_provider_classification" ) set_equipment_provider_classification( numeric_cast<uint64>(value) );
            else if( item == "num_datarecords" ) set_num_datarecords( numeric_cast<int64>(value) );
            else throw exception::general( "Bad assignment to " + item );
        }

        /// get value of a numeric item
        template<typename T> T getNumeric( std::string item, T )
        {
            using boost::numeric_cast;
            if( item == "patient_drugs" ) return numeric_cast<T>(get_patient_drugs( ));
            else if( item == "patient_weight" ) return numeric_cast<T>(get_patient_weight( ));
            else if( item == "patient_height" ) return numeric_cast<T>(get_patient_height( ));
            else if( item == "patient_flags" ) return numeric_cast<T>(get_patient_flags( ));
            else if( item == "recording_start" ) return numeric_cast<T>(get_recording_start( ));
            else if( item == "patient_birthday" ) return numeric_cast<T>(get_patient_birthday( ));
            else if( item == "header_length" ) return numeric_cast<T>(get_header_length( ));
            else if( item == "equipment_provider_classification" ) return numeric_cast<T>(get_equipment_provider_classification( ));
            else if( item == "num_datarecords" ) return numeric_cast<T>(get_num_datarecords( ));
            else throw exception::general( item + " is not known as a numeric item" );
        }

        /// Assign a numeric array item
        template<typename T> void setArray( std::string item, size_t array_index, T value )
        {
            using boost::numeric_cast;
            if( item == "recording_location" ) recording_location[array_index] = numeric_cast<uint32>(value);
            else if( item == "patient_headsize" ) patient_headsize[array_index] = numeric_cast<uint16>(value);
            else if( item == "pos_reference" ) pos_reference[array_index] = numeric_cast<float32>(value);
            else if( item == "pos_ground" ) pos_ground[array_index] = numeric_cast<float32>(value);
            else if( item == "datarecord_duration" ) datarecord_duration[array_index] = numeric_cast<uint32>(value);
            else throw exception::general( "Bad array access to " + item );
        }

        /// get a numeric array item
        template<typename T> T getArray( std::string item, size_t array_index, T )
        {
            using boost::numeric_cast;
            if( item == "recording_location" ) return numeric_cast<T>(recording_location[array_index]);
            else if( item == "patient_headsize" ) return numeric_cast<T>(patient_headsize[array_index]);
            else if( item == "pos_reference" ) return numeric_cast<T>(pos_reference[array_index]);
            else if( item == "pos_ground" ) return numeric_cast<T>(pos_ground[array_index]);
            else if( item == "datarecord_duration" ) return numeric_cast<T>(datarecord_duration[array_index]);
            else throw exception::general( "Bad array access to " + item );
        }

        friend class GDFHeaderAccess;
        friend class Writer;
        friend class Reader;
        friend std::ostream& operator<< (std::ostream& out, const GDFHeaderAccess& hdr);
        friend std::istream& operator>> (std::istream& in, GDFHeaderAccess& hdr);
    };
}

#endif
