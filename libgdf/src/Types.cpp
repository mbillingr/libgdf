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

#include "GDF/Types.h"
#include <boost/lexical_cast.hpp>

namespace gdf
{
    size_t datatype_size( uint32 t )
    {
        switch( t )
        {
        case INT8: return sizeof( int8 ); break;
        case INT16: return sizeof( int16 ); break;
        case INT32: return sizeof( int32 ); break;
        case INT64: return sizeof( int64 ); break;
        case UINT8: return sizeof( uint8 ); break;
        case UINT16: return sizeof( uint16 ); break;
        case UINT32: return sizeof( uint32 ); break;
        case UINT64: return sizeof( uint64 ); break;
        case FLOAT32: return sizeof( float32 ); break;
        case FLOAT64: return sizeof( float64 ); break;
        default:
            throw exception::invalid_type_id( boost::lexical_cast<std::string>(t) );
        }
    }
}
