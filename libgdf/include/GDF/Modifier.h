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

#ifndef __MODIFIER_H_INCLUDED__
#define __MODIFIER_H_INCLUDED__

#include "GDF/Reader.h"
#include <vector>
#include <string>
#include <fstream>

namespace gdf
{
    /// Class for reading, modifying and saving changes to GDF files.
    /** It is not possible to change the number of signals or their length.
        When a sample is set, the entire record is marked for rewriting. If the
        Event Header is retrieved using the non-const get function, it is marked
        for rewriting.
      */
    class Modifier : private Reader
    {
    public:
        /// Constructor
        Modifier( );

        /// Destructor
        virtual ~Modifier( );

        /// Opens file for modification
        void open( const std::string filename );

        /// Close file
        void close( );

        /// Write changes to disk
        void saveChanges( );

        /// Get a single Sample (physical units)
        /** @param[in] channel_idx channel index
            @param[in] sample_idx sample index
          */
        double getSample( uint16 channel_idx, size_t sample_idx );

        /// Set a single Sample (physical units)
        /** @param[in] channel_idx channel index
            @param[in] sample_idx sample index
            @param[in] value
          */
        void setSample( uint16 channel_idx, size_t sample_idx, double value );

        /// get writable reference to event header
        EventHeader *getEventHeader( );

        /// get const reference to event header
        const EventHeader *getEventHeader_readonly( );

    private:

        /// Initialize cache to correct size
        void initCache( );

        /// Reset cache to empty state
        void resetCache( );

        std::vector< bool > m_record_changed;
        bool m_events_changed;
    };
}

#endif // __READER_H_INCLUDED__
