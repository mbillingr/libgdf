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

#ifndef __RECORD_H_INCLUDED__
#define __RECORD_H_INCLUDED__

#include "GDF/Channel.h"
#include "pointerpool.h"
#include <vector>
#include <list>

namespace gdf {

    class GDFHeaderAccess;

    /// A Record is a block of data that contains a short time period of samples from all channels.
    class Record
    {
    private:
        /// Constructor
        Record( const GDFHeaderAccess *hdr );

    public:

        /// Copy Constructor
        Record( const Record &other );

        /// Destructor
        virtual ~Record( );

        /// clear Record (revert to initial state)
        void clear( );

        /// copy from another Record
        void operator=( const Record &other );

        /// Fills free samples in all channels with defined values.
        /** This function is used to fill unfinished records before writing them to disc. For now
            we fill with NaNs if supported by the channel's data type and (physical) 0 otherwise.
            @todo What values should we fill in? NaN would be nice for floating point, but what
            about other data types?
        */
        void fill( );

        /// Returns true if all channels in the record have no free samples.
        bool isFull( ) const;

        /// Returns true if all channels in the record are empty.
        bool isEmpty( ) const;

        /// Returns reference to channel chan_idx.
        Channel *getChannel( const size_t chan_idx );

        friend std::ostream &operator<<( std::ostream &out, const Record &c );
        friend std::istream &operator>>( std::istream &in, Record &c );

        friend class RecordBuffer;
        friend class Reader;
        //friend class PointerPool<Record>;

    private:
        std::vector< Channel* > channels;
    };

    /// Record Serializer
    std::ostream &operator<<( std::ostream &out, const Record &r );

    /// Record Deserializer
    std::istream &operator>>( std::istream &in, Record &r );
}

#endif
