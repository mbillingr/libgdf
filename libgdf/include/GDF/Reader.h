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

#ifndef __READER_H_INCLUDED__
#define __READER_H_INCLUDED__

#include "GDF/Record.h"
#include "GDF/EventHeader.h"
#include "GDF/GDFHeaderAccess.h"
#include "GDF/Types.h"
#include "GDF/tools.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>

namespace gdf
{
    /// Class for reading GDF files to disc.
    /** Data Records are only read on demand and stay in memory until another file is opened or the Reader object is destroyed.
        This cache can be disabled. Then each data record is loaded from disk everytime it is accessed. This saves memory, but may
        severly decrease performance.
      */
    class Reader
    {
    public:
        /// Constructor
        Reader( );

        /// Destructor
        virtual ~Reader( );

        /// Opens file for reading
        void open( const std::string filename );

        /// Close file
        void close( );

        /// Enable or disable cache
        void enableCache( bool b );

        /// Reset cache to empty state
        virtual void resetCache( );

        /// Find Record index to which a sample belongs
        size_t findRecord( uint16 channel_idx, size_t sample_idx );

        /// Read Signals from file into buffer (physical units)
        /** Sample values are converted to physical units.
            @param[out] buffer vector; each element is a channel.
            @param[in] start_time samples with n >= start_time*fs are loaded.
            @param[in] end_time samples with n < end_time*fs are loaded. end_time = -1 loads the complete signal.
            @param[in] signal_indices vector with signal indices that should be loaded. If empty, all signals are loaded.
          */
        void getSignals( std::vector< std::vector<double> > &buffer, double start_time = 0, double end_time = -1, std::vector<uint16> signal_indices = std::vector<uint16>() );

        /// Read a single channel from file into buffer.
        /** The buffer must be allocated by the user, who is also responsible that enough memory is allocated.
            @param[in] channel_idx index of channel to read
            @param[out] buffer pointer to double array
            @param[in] start_time samples with n >= start are loaded.
            @param[in] end_time samples with n < end are loaded. end <= start loads the complete signal.
          */
        void getSignal( uint16 channel_idx, double *buffer, size_t start = 0, size_t end = 0 );

        /// Read a single Sample (physical units)
        /** @param[in] channel_idx channel index
            @param[in] sample_idx sample index
          */
        double getSample( uint16 channel_idx, size_t sample_idx );

        /// Returns a reference to Record
        Record *getRecordPtr( size_t index );

        /// Precache a range of Records
        void precacheRecords( size_t start, size_t end );

        /// get reference to event header
        EventHeader *getEventHeader( );

    protected:
        void readEvents( );

        std::string m_filename;
        GDFHeaderAccess m_header;
        EventHeader *m_events;
        std::vector< boost::shared_ptr<Record> > m_record_cache;
        std::ifstream m_file;
        bool m_cache_enabled;

        size_t m_record_length; /// Record length in bytes
        size_t m_record_offset; /// Where data records start in the file
        size_t m_event_offset;  /// Where the event table starts in the file
    };
}

#endif // __READER_H_INCLUDED__
