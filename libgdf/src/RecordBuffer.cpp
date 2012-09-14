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

#include "GDF/RecordBuffer.h"
#include "GDF/Record.h"
#include "GDF/GDFHeaderAccess.h"
#include "GDF/tools.h"
//#include <iostream>

namespace gdf
{
    RecordBuffer::RecordBuffer( const GDFHeaderAccess *gdfh ) : m_gdfh(gdfh)
    {
        m_pool = NULL;
    }

    //===================================================================================================
    //===================================================================================================

    RecordBuffer::~RecordBuffer( )
    {
        clearBuffers( );
        if( m_pool )
            delete m_pool;
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::clearBuffers( )
    {
        if( m_pool )
        {
            std::list<Record*>::iterator it = m_records.begin( );
            for( ; it != m_records.end(); it++ )
                m_pool->push( *it );

            it = m_records_full.begin( );
            for( ; it != m_records_full.end(); it++ )
                m_pool->push( *it );
        }

        m_records.clear( );
        m_records_full.clear( );

        m_num_full = 0;
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::reset( )
    {
        size_t M = m_gdfh->getMainHeader_readonly( ).get_num_signals( );
        clearBuffers( );
        m_channelhead.resize( M );
        for( size_t i=0; i<M; i++ )
            m_channelhead[i] =m_records.begin( );
        if( m_pool )
            delete m_pool;
        m_pool = new PointerPool<Record>( Record(m_gdfh) );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::handleChannelFull( const size_t channel_idx )
    {
        //std::cout << "Channel Full" << std::endl;
        Record *r = *m_channelhead[channel_idx];
        m_channelhead[channel_idx]++;
        if( r->isFull() )
            handleRecordFull( );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::handleRecordFull( )
    {
        //std::cout << "Record Full" << std::endl;
        m_records_full.push_back( m_records.front() );
        m_records.pop_front( );
        m_num_full++;

        std::list<RecordFullHandler*>::iterator it = m_recfull_callbacks.begin( );
        for( ; it != m_recfull_callbacks.end(); it++ )
            (*it)->triggerRecordFull( m_records_full.back() );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::registerRecordFullCallback( RecordFullHandler *h )
    {
        m_recfull_callbacks.push_back( h );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::unregisterRecordFullCallback( RecordFullHandler *h )
    {
        m_recfull_callbacks.remove( h );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::addSamplePhys( const size_t channel_idx, const double value )
    {
        Channel *ch = getValidChannel( channel_idx );
        ch->addSamplePhys( value );
        if( ch->getFree( ) == 0 )
            handleChannelFull( channel_idx );
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::blitSamplesPhys( const size_t channel_idx, const double *values, size_t num )
    {
        size_t i = 0;
        while( i<num )
        {
            Channel *ch = getValidChannel( channel_idx );
            size_t n = std::min( num-i, ch->getFree( ) );
            ch->blitSamplesPhys( &values[i], n );
            if( ch->getFree( ) == 0 )
                handleChannelFull( channel_idx );
            i += n;
        }
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::addRecord( Record *r )
    {
        if( getNumPartialRecords( ) > 0 )
            throw exception::invalid_operation( "RecordBuffer::addRecord called, but buffer contains partial records." );
        m_records_full.push_back( r );
        m_num_full++;

        std::list<RecordFullHandler*>::iterator it = m_recfull_callbacks.begin( );
        for( ; it != m_recfull_callbacks.end(); it++ )
            (*it)->triggerRecordFull( m_records_full.back() );
    }

    //===================================================================================================
    //===================================================================================================

    Record *RecordBuffer::acquireRecord( )
    {
        return m_pool->pop( );
    }

    //===================================================================================================
    //===================================================================================================

    std::list< Record* >::iterator RecordBuffer::createNewRecord( )
    {
        Record *r = m_pool->pop( );
        r->clear( );
        m_records.push_back( r );
        std::list< Record* >::iterator it = m_records.end( );
        it--;
        return it;
    }

    //===================================================================================================
    //===================================================================================================

    Record *RecordBuffer::getFirstFullRecord( )
    {
        if( m_num_full > 0 )
            return m_records_full.front( );
        else
            return NULL;
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::removeFirstFullRecord( )
    {
        m_pool->push( m_records_full.front() );
        m_records_full.pop_front( );
        m_num_full--;
    }

    //===================================================================================================
    //===================================================================================================

    Channel *RecordBuffer::getValidChannel( const size_t channel_idx )
    {
        if( channel_idx >= m_channelhead.size( ) )
            throw exception::nonexistent_channel_access( "channel "+boost::lexical_cast<std::string>(channel_idx) +"does not exist" );

        if( m_channelhead[channel_idx] == m_records.end() )
        {
            if( m_records.size() > 0 )
            {
                if( m_records.back()->getChannel(channel_idx)->getFree( ) == 0 )
				{
					// Create a new record in m_records and inform all 
					// channels that are pointing beyond the end m_records.

					// capture the iter value that flags m_channelhead's that have no free space
					std::list< Record* >::iterator end_iter = m_records.end();  
					// get a clean record from m_pool, enlist it on m_records, and return an iterator
					std::list< Record* >::iterator newrec_iter = createNewRecord( );
					// broadcast the new record among all channels that need it
					for( size_t i=0; i<m_channelhead.size(); i++ )
					{
						if (m_channelhead[i] == end_iter) 
						{
							m_channelhead[i] = newrec_iter;
						}
					}
				}
                else
                    throw exception::corrupt_recordbuffer( "DOOM is upon us!" );
            }
            else
            {
                // list was empty: set all channel heads to the new record.
                std::list< Record* >::iterator r = createNewRecord( );
                for( size_t i=0; i<m_channelhead.size(); i++ )
                    m_channelhead[i] = r;
            }
        }

        return (*m_channelhead[channel_idx])->getChannel( channel_idx );
    }

    //===================================================================================================
    //===================================================================================================

    size_t RecordBuffer::getNumFreeAlloc( const size_t channel_idx )
    {
        size_t num = 0;
        getValidChannel( channel_idx );
        std::list< Record* >::iterator it = m_channelhead[channel_idx];
        while( it != m_records.end() )
        {
            num += (*it)->getChannel( channel_idx )->getFree( );
            it ++;
        }
        return num;
    }

    //===================================================================================================
    //===================================================================================================

    void RecordBuffer::flood( )
    {
        while( getNumPartialRecords( ) > 0 )
        {
            m_records.front( )->fill( );
            handleRecordFull( );
        }

        for( size_t i=0; i<m_channelhead.size(); i++ )
            m_channelhead[i] =m_records.begin( );
    }

    //===================================================================================================
    //===================================================================================================

}
