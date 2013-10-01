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
// Copyright 2010, 2013 Martin Billinger, Owen Kelly

#ifndef __WRITER_H_INCLUDED__
#define __WRITER_H_INCLUDED__

#include "RecordBuffer.h"
#include "RecordFullHandler.h"
#include "EventHeader.h"
#include "GDFHeaderAccess.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace gdf
{
    enum WriterFlags
    {
        writer_ev_file      = 0,
        writer_ev_memory    = 1,
        writer_overwrite    = 2
    };

    /// Class for writing GDF files to disc.
    /** Events are buffered and appended to the file when calling close( ). By default events are buffered
        in a separate file named \e filename.events. Thus, information may be recovered after computer
        crashes during long online recordings. The user may chose to buffer events in memory instead (see open()).
    */
    class Writer : public RecordFullHandler
    {
    public:
        /// Constructor
        Writer( );

        /// Destructor
        virtual ~Writer( );

        /// Opens file for writing and writes Header
        /** Prior to opening the file, GDFHeaderAccess::sanitize() is called. Exceptions thrown by
            GDFHeaderAccess::sanitize() are checked if there are errors and/or warnings. In the presence
            of errors the exception is forwarded immediately, and in case of warnings the file is opened
            but the exception is still forwarded. If the file exists, it is not opened unless the file_overwrite
            flag is set.
            @param[in] flags Flags...
            @throws exception::header_issues
            @throws exception::file_exists
        */
        void open( const int flags = writer_ev_file );

        /// Opens file for writing and writes Header
        /** Convenience function. Calls setFilename prior to opening the file
            @param[in] filename Full path name to the file.
            @param[in] flags Flags...
            @throws exception::header_issues
            @throws exception::file_exists
        */
        void open( const std::string filename, const int flags = writer_ev_file );

        /// Close file.
        /** File is closed if open. Prior to closing, events are written to the file. */
        void close( );

        /// Check if file is open
        bool isOpen( );

        /// set filename for later opening
        /** @param[in] filename Full path name to the file
          */
        void setFilename( std::string filename );

        /// Set number of full records that are kept in memory before the buffer is flushed.
        /** @param[in] num number of records */
        void setMaxFullRecords( size_t num );

        /// Create a signal.
        /** Signals have to be created before they can be configured and stored.
            @param[in] index index of the signal
            @param[in] throwexc if true, exception::signal_exists may be thrown
            @returns true on success
          */
        bool createSignal( size_t index, bool throwexc = false );

        /// Swap to signals
        /** Both signals must exist.
            @param[in] a index of first signal
            @param[in] b index of second signal
          */
        void swapSignals( size_t a, size_t b );

        /// Change signal index
        /** The new index must not exist
            @param[in] src old index of first signal
            @param[in] dst new index of second signal
          */
        void relocateSignal( size_t src, size_t dst );

        /// get lowest signal index that can be created
        size_t getFirstFreeSignalIndex( );

        /// Blit data from a serial buffer
        /** Instead of streaming samples as they come, the complete data is provided in an array of type double.
            In the buffer channels have to be arranged sequentially. I.e. all samples from channel 1 are
            followed by all samles from channel 2, and so on. This function attempts to keep memory overhead low
            by filling record by record whenever possible.
            @param[in] buf Buffer
            @param[in] samples_per_channel A vector containing the number of samples in each channel.
         */
        void blitFromSerialBufferPhys( const double *buf, const std::vector<size_t> &samples_per_channel );

        /// Add sample in physical units to a channel
        /**
            The sample value is converted from the channels [physmin,physmax] to the range of [digmin,digmax] and
            then cast to the correct data type.
            @param[in] channel_idx index of the channel written to
            @param[in] value sample value in physical units
        */
        void addSamplePhys( const size_t channel_idx, const float64 value );

        /// Add a raw sample to channel
        /**
            The sample value is cast to the correct data type, but no range checking is performed
            @param[in] channel_idx index of the channel written to
            @param[in] value raw sample value in physical units
        */
        template<typename T> void addSampleRaw( const size_t channel_idx, const T value )
        {
            m_recbuf.addSampleRaw<T>( channel_idx, value );
        }

        /// Blit a number of samples in physical units to channel
        /**
            The sample values are converted from the channels [physmin,physmax] to the range of [digmin,digmax] and
            then cast to the correct data type.
            @param[in] channel_idx index of the channel written to
            @param[in] values array of sample values in physical units
            @param[in] num number of samples to blit
        */
        void blitSamplesPhys( const size_t channel_idx, const float64 *values, size_t num );

        /// Blit a number of samples in physical units to channel
        /**
            The sample values are converted from the channels [physmin,physmax] to the range of [digmin,digmax] and
            then cast to the correct data type.
            @param[in] channel_idx index of the channel written to
            @param[in] values vector of sample values in physical units
        */
        void blitSamplesPhys( const size_t channel_idx, const std::vector<float64> &values );

        /// Blit a number of raw samples  to channel
        /**
            The sample values are cast to the correct data type, but no range checking is performed
            @param[in] channel_idx index of the channel written to
            @param[in] values array of raw sample value
            @param[in] num number of samples to blit
        */
        template<typename T> void blitSamplesRaw( const size_t channel_idx, const T *values, size_t num )
        {
            m_recbuf.blitSamplesRaw<T>( channel_idx, values, num );
        }

        /// Blit a number of raw samples  to channel
        /**
            The sample values are cast to the correct data type, but no range checking is performed
            @param[in] channel_idx index of the channel written to
            @param[in] values vector of raw sample value
        */
        template<typename T> void blitSamplesRaw( const size_t channel_idx, const std::vector<T> &values )
        {
            m_recbuf.blitSamplesRaw<T>( channel_idx, &values[0], values.size() );
        }

        /// Add a complete Record
        void addRecord( Record *r );

        /// Get pointer to a fresh Record
        Record *acquireRecord( );

        /// writes record to disc
        void writeRecordDirect( Record *r );

        /// writes all full records from buffer to disc
        void flush( );

        /// Set Event Mode
        /** mode can be 1 or 3
              1: (default) Events are stored as position,type pairs
              3: Events are stored with position and type, associated to a channel and have a duration
                 or value.
            @param[in] mode event mode
          */
        void setEventMode( uint8 mode );

        /// Set Sampling Rate associated with event positions
        /** Events are not actually sampled, but their position is stored in samples rather than seconds.
            In order to convert event positions between time and sample, this sampling rate is used.
            If Sampling rate is not set (or set to <= 0 ), fs is set to the highest signal sampling rate.
            @param[in] fs sampling rate
        */
        void setEventSamplingRate( float32 fs = -1 );

        /// Add a Mode 1 Event
        void addEvent( const Mode1Event &ev );

        /// Add a Mode 1 Event
        void addEvent( uint32 position, uint16 type );

        /// Add a Mode 3 Event
        void addEvent( const Mode3Event &ev );

        /// Add a Mode 3 Event
        void addEvent( uint32 position, uint16 type, uint16 channel, uint32 duration );

        /// Add a Mode 3 Event
        void addEvent( uint32 position, uint16 type, uint16 channel, float32 value );

        /// get Constant reference to header access
        const GDFHeaderAccess &getHeaderAccess_readonly( ) const { return m_header; }

        /// get reference to main header
        GDFHeaderAccess &getHeaderAccess( ) { return m_header; }

        /// get Constant reference to main header
        const MainHeader &getMainHeader_readonly( ) const { return m_header.getMainHeader_readonly( ); }

        /// get reference to main header
        MainHeader &getMainHeader( ) { return m_header.getMainHeader( ); }

        /// get constant reference to a signal's header
        const SignalHeader &getSignalHeader_readonly( size_t idx ) const { return m_header.getSignalHeader_readonly(idx); }

        inline size_t getNumSignals( ) const { return m_header.getNumSignals( ); }

        /// get reference to a signal's header
        SignalHeader &getSignalHeader( size_t idx ) { return m_header.getSignalHeader(idx); }

		/// Converts from sample to event
		/** @throws exception::event_conversion_error
        */
		void sampleToEvent( const double sample_time_sec, const double sample_physical_value, const uint16 channel, Mode3Event& ev );

        /// Format an existing Mode 1 event (TYP, POS) to represent the given string (TYP) at the given time (POS).
        void makeFreeTextEvent( double noteTimeSec, const std::string str, EventDescriptor & ev_desc, Mode1Event & e );

        /// Format an existing Mode 3 event (TYP, POS, CHN, DUR) to represent the given string (TYP) at the given time (POS).
        //  Separately, the string may be associated to a particular channel using event.CHN.
        void makeFreeTextEvent( double noteTimeSec, const std::string str, EventDescriptor & ev_desc, Mode3Event & e );

    private:

        /// write first full record from record buffer to disk
        void writeRecord( );

        /// write events from buffer to file
        void writeEvents( );

        /// record full handler
        virtual void triggerRecordFull( Record *rec );

        RecordBuffer m_recbuf;
        GDFHeaderAccess m_header;
        std::fstream m_file;
        std::iostream m_eventbuffer;
        std::fstream m_evbuf_file;
        std::stringstream m_evbuf_memory;
        int m_eventbuffermemory;
        std::string m_filename;
        int64 m_num_datarecords;
        size_t max_full_records;
    };
}

#endif // __WRITER_H_INCLUDED__
