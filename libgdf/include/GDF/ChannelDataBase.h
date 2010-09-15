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

#ifndef __CHANNELDATABASE_H_INCLUDED
#define __CHANNELDATABASE_H_INCLUDED

#include "Types.h"
#include "Exceptions.h"
#include <stddef.h>

namespace gdf
{

    /// Base class that provides access to channels of different types
    /** Meant only for internal use.
        This class provides overloaded access functions for each data type supported in gdf. The actual implementation of
        ChannelData must reimplement access functions for it's data type.
    */
    class ChannelDataBase
    {
    public:
        ChannelDataBase( ) { }
        virtual ~ChannelDataBase( ) { }

        virtual void addSample( const int8 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const uint8 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const int16 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const uint16 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const int32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const uint32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const int64 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const uint64 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const float32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void addSample( const float64 ) { throw exception::bad_type_assigned_to_channel( ); }

        virtual void blitSamples( const int8*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const uint8*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const int16*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const uint16*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const int32*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const uint32*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const int64*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const uint64*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const float32*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void blitSamples( const float64*, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }

        virtual void fill( const int8, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const uint8, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const int16, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const uint16, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const int32, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const uint32, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const int64, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const uint64, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const float32, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void fill( const float64, const size_t ) { throw exception::bad_type_assigned_to_channel( ); }

        virtual void setSample( size_t, int8 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, uint8 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, int16 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, uint16 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, int32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, uint32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, int64 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, uint64 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, float32 ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual void setSample( size_t, float64 ) { throw exception::bad_type_assigned_to_channel( ); }

        virtual int8 getSample( size_t, int8 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual uint8 getSample( size_t, uint8 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual int16 getSample( size_t, int16 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual uint16 getSample( size_t, uint16 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual int32 getSample( size_t, int32 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual uint32 getSample( size_t, uint32 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual int64 getSample( size_t, int64 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual uint64 getSample( size_t, uint64 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual float32 getSample( size_t, float32 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }
        virtual float64 getSample( size_t, float64 /*dummy*/ ) { throw exception::bad_type_assigned_to_channel( ); }

        /// Get number of free samples.
        virtual size_t getFree( ) = 0;

        /// Get number of written samples.
        virtual size_t getWritten( ) = 0;

        /// Serializer
        virtual void tostream( std::ostream &out ) = 0;

        /// Deserializer
        virtual void fromstream( std::istream &in ) = 0;
    };

}

#endif
