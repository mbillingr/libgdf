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

#ifndef __TYPES_H_INCLUDED__
#define __TYPES_H_INCLUDED__

#include "Exceptions.h"
#include <boost/cstdint.hpp>
#include <boost/predef/other/endian.h>
#include <iostream>

namespace gdf
{
    typedef boost::int8_t int8;
    typedef boost::uint8_t uint8;
    typedef boost::int16_t int16;
    typedef boost::uint16_t uint16;
    typedef boost::int32_t int32;
    typedef boost::uint32_t uint32;
    typedef boost::int64_t int64;
    typedef boost::uint64_t uint64;
    typedef float float32;
    typedef double float64;

    enum type_id
    {
        INVALID_TYPE = 0,
        INT8 = 1,
        UINT8 = 2,
        INT16 = 3,
        UINT16 = 4,
        INT32 = 5,
        UINT32 = 6,
        INT64 = 7,
        UINT64 = 8,

        FLOAT32 = 16,
        FLOAT64 = 17
    };

    size_t datatype_size( uint32 t );

    template<typename T>
    T switch_endian( const T &source )
    {
        T dest;
        const char *A = reinterpret_cast<const char*>(&source);
        char *B = reinterpret_cast<char*>(&dest);

        int a = 0;
        int b = sizeof(T)-1;

        while( b >= 0 )
        {
            B[b] = A[a];
        }
    }

    template<typename T>
    void writeLittleEndian( std::ostream &out, T item )
    {
#if BOOST_ENDIAN_LITTLE_BYTE
        out.write( reinterpret_cast<const char*>(&item), sizeof(item) );
#elif BOOST_ENDIAN_BIG_BYTE
        const char* p = reinterpret_cast<const char*>(&item) + sizeof(item)-1;
        for( size_t i=0; i<sizeof(item); i++ )
            out.write( p--, 1 );
#else
    #error "Unable to determine system endianness."
#endif
    }

    template<typename T>
    void readLittleEndian( std::istream &in, T &item )
    {
#if BOOST_ENDIAN_LITTLE_BYTE
        in.read( reinterpret_cast<char*>(&item), sizeof(item) );
#elif BOOST_ENDIAN_BIG_BYTE
        char* p = reinterpret_cast<char*>(&item) + sizeof(item)-1;
        for( size_t i=0; i<sizeof(item); i++ )
    in.read( p--, 1 );
#else
    #error "Unable to determine system endianness."
#endif
    }

}

#endif
