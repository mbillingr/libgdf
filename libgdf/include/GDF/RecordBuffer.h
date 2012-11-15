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

#ifndef __RECORDBUFFER_H_INCLUDED__
#define __RECORDBUFFER_H_INCLUDED__

#include "GDF/Channel.h"
#include "GDF/pointerpool.h"
//#include "GDF/RecordFullHandler.h"

#include <list>
#include <vector>

namespace gdf
{
    class Record;
    class GDFHeaderAccess;
    class RecordFullHandler;

    /// Buffers incomplete records before they are written to disk.
    /** When saving data, one or more channels can be ahead of the others, and may even extend
        over multiple records. RecordBuffer takes care of this by creating a new record as soon
        a channel exceeds it's capacity. */
    class RecordBuffer
    {
    public:

        /// Constructor
        RecordBuffer( const GDFHeaderAccess *gdfh );

        /// Destructor
        virtual ~RecordBuffer( );

        /// Clear all buffers
        void clearBuffers( );

        /// Resets the buffer to a valid initial state
        void reset( );

        /// Called when a channel becomes full
        /** This function advances the write pointer m_channelhead for this channel to the next record. If
            there is no next record, a new one is created. Also checks if the current record is full and
            performs calls the respective handler. */
        void handleChannelFull( const size_t channel_idx );

        /// Called when a record becomes full
        /** */
        void handleRecordFull( );

        /// Register Callback for record full event
        void registerRecordFullCallback( RecordFullHandler *h );

        /// Unegister Callback for record full event
        void unregisterRecordFullCallback( RecordFullHandler *h );

        /// Add a physical sample to the channel specified by channel_idx.
        void addSamplePhys( const size_t channel_idx, const double value );

        /// Add a raw sample to the channel specified by channel_idx.
        template<typename T> void addSampleRaw( const size_t channel_idx, const T rawval )
        {
            Channel *ch = getValidChannel( channel_idx );
            ch->addSampleRaw<T>( rawval );
            if( ch->getFree( ) == 0 )
                handleChannelFull( channel_idx );
        }

        /// Blit a number of physical samples into channel specified by channel_idx.
        void blitSamplesPhys( const size_t channel_idx, const double *values, size_t num );

        /// Blit a number of raw samples into channel specified by channel_idx.
        template<typename T> void blitSamplesRaw( const size_t channel_idx, const T *values, size_t num )
        {
            size_t i = 0;
            while( i<num )
            {
                Channel *ch = getValidChannel( channel_idx );
                size_t n = std::min( num-i, ch->getFree( ) );
                ch->blitSamplesRaw( &values[i], n );
                if( ch->getFree( ) == 0 )
                    handleChannelFull( channel_idx );
                i += n;
            }
        }

        /// Fill a number of samples with the same physical value.
        void fillPhys( const size_t channel_idx, const double value, size_t num );

        /// Fill a number of samples with the same raw value.
        template<typename T> void fillRaw( const size_t channel_idx, const T value, size_t num );

        /// Add a complete Record
        /** There may not be any partial records in the buffer in order to add a complete record. */
        void addRecord( Record *r );

        /// Returns a reference to a free record.
        /** Use this instead of 'new Record()'. **/
        Record *acquireRecord( );

        /// Put a new record to the end of the list.
        std::list< Record* >::iterator createNewRecord( );

        /// Reference to first (oldest) full record in list. This is also the first record that gets filled.
        Record *getFirstFullRecord( );

        /// Remove first full record in list.
        void removeFirstFullRecord( );

        /// Get number of full records in the buffer.
        inline size_t getNumFullRecords( ) const { return m_num_full; }

        /// Get number of partially filled records currently in the list.
        inline size_t getNumPartialRecords( ) const { return m_records.size( ); }

        /// Returns reference to channel specified by channel_idx
        /** If channel does not exist gdf::nonexistent_channel_access::nonexistent_channel_access is thrown.
            A new record is created if the channelhead points to the end of the record list.*/
        Channel *getValidChannel( const size_t channel_idx );

        /// Gets number of free samples currently allocated for this channel
        size_t getNumFreeAlloc( const size_t channel_idx );

        /// Fills all partial records with default values
        void flood( );

    protected:

    private:
        const GDFHeaderAccess *m_gdfh;
        PointerPool<Record> *m_pool;
        std::list< Record* > m_records;
        std::list< Record* > m_records_full;
        size_t m_num_full;
        std::vector< std::list< Record* >::iterator > m_channelhead;
        std::list<RecordFullHandler*> m_recfull_callbacks;
    };
}

#endif
