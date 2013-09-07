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

#include "GDF/Writer.h"
#include "GDF/Exceptions.h"
#include "GDF/Record.h"
#include "GDF/tools.h"
#include <iostream>

namespace gdf
{
    Writer::Writer( ) : m_recbuf( &m_header ), m_eventbuffer( NULL )
    {
        m_eventbuffermemory = writer_ev_file;
        setMaxFullRecords( 0 );
        m_recbuf.registerRecordFullCallback( this );
    }

    //===================================================================================================
    //===================================================================================================

    Writer::~Writer( )
    {
        //std::cout << "~Writer( )" << std::endl;
        if( m_file.is_open( ) )
            close( );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::open(const int flags )
    {
        if( m_file.is_open() )
            throw exception::file_open( "" );

        assert( !m_file.is_open() );
        m_header.setLock( true );

        bool warn = false;
        std::list< std::string > wmsg;
        try {
            m_header.sanitize( );
        } catch( exception::header_issues &e )
        {
            wmsg = e.warnings;
            if( e.errors.size() != 0 )
                throw e;
            if( e.warnings.size() != 0 )
                warn = true;
        }

        if( !(flags & writer_overwrite) )
        {
            m_file.open( m_filename.c_str(), std::ios_base::in );
            if( !m_file.fail() )
            {
                m_header.setLock( false );
                m_file.close( );
                throw exception::file_exists( m_filename );
            }
        }

        m_file.clear( );

        m_file.open( m_filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

        if( m_file.fail() )
        {
            throw std::invalid_argument( "Error opening file for writing." );
        }

        m_eventbuffermemory = flags & writer_ev_memory;
        if( m_eventbuffermemory )
        {
            m_eventbuffer.rdbuf( m_evbuf_memory.rdbuf() );
            m_evbuf_memory.clear( );
        }
        else
        {
            m_evbuf_file.open( (m_filename+".events").c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
            m_eventbuffer.rdbuf( m_evbuf_file.rdbuf() );
        }

        m_recbuf.reset( );
        m_num_datarecords = 0;

        m_file << m_header;
        m_file.flush( );

        if( warn )
            throw exception::header_issues( wmsg );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::open( const std::string filename, const int flags )
    {
        setFilename( filename );
        open( flags );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::close( )
    {
        if( !m_file.is_open() )
            return;
            //throw exception::file_not_open( "" );

        if( !m_eventbuffermemory )
            m_evbuf_file.close( );

        m_recbuf.flood( );

        flush( );

        if( !m_eventbuffermemory )
        {
            m_evbuf_file.open( (m_filename+".events").c_str(), std::ios_base::in | std::ios_base::binary );
            m_eventbuffer.rdbuf( m_evbuf_file.rdbuf() );
        }

        writeEvents( );

        if( !m_eventbuffermemory )
        {
            m_evbuf_file.close( );
            remove( (m_filename+".events").c_str() );
        }

        m_header.setLock( false );

        getMainHeader().set_num_datarecords( m_num_datarecords );

        m_file.seekp( getMainHeader_readonly().num_datarecords.pos );
        getMainHeader().num_datarecords.tostream( m_file );

        m_file.close( );
    }

    //===================================================================================================
    //===================================================================================================

    bool Writer::isOpen( )
    {
        return m_file.is_open( );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::setFilename( std::string filename )
    {
        m_filename = filename;
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::setMaxFullRecords( size_t num )
    {
        max_full_records = num;
    }

    //===================================================================================================
    //===================================================================================================

    bool Writer::createSignal( size_t index, bool throwexc )
    {
        return m_header.createSignal( index, throwexc );
    }

    //===================================================================================================
    //===================================================================================================

    void  Writer::swapSignals( size_t a, size_t b )
    {
        m_header.swapSignals( a, b );
    }

    //===================================================================================================
    //===================================================================================================

    void  Writer::relocateSignal( size_t src, size_t dst )
    {
        m_header.swapSignals( src, dst );
    }

    //===================================================================================================
    //===================================================================================================

    size_t Writer::getFirstFreeSignalIndex( )
    {
        return m_header.getFirstFreeSignalIndex( );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::blitFromSerialBufferPhys( const double *buf, const std::vector<size_t> &samples_per_channel )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );

        size_t M = samples_per_channel.size( );
        std::vector<size_t> samples_remaining = samples_per_channel;
        size_t total_samples_remaining = sum( samples_remaining );

        std::vector<size_t> readpos;
        readpos.resize( M );
        readpos[0] = 0;
        for( size_t i=1; i<M; i++ )
            readpos[i] = readpos[i-1] + samples_per_channel[i-1];

        while( total_samples_remaining > 0 )
        {
            for( size_t i=0; i<M; i++ )
            {
                size_t n = std::min( samples_remaining[i], m_recbuf.getNumFreeAlloc( i ) );
                m_recbuf.blitSamplesPhys( i, &buf[readpos[i]], n );
                total_samples_remaining -= n;
                samples_remaining[i] -= n;
                readpos[i] += n;
            }
        }
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addSamplePhys( const size_t channel_idx, const float64 value )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        m_recbuf.addSamplePhys( channel_idx, value );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::blitSamplesPhys( const size_t channel_idx, const float64 *values, size_t num )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        m_recbuf.blitSamplesPhys( channel_idx, values, num );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::blitSamplesPhys( const size_t channel_idx, const std::vector<float64> &values )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        m_recbuf.blitSamplesPhys( channel_idx, &values[0], values.size() );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addRecord( Record *r )
    {
        m_recbuf.addRecord( r );
    }

    //===================================================================================================
    //===================================================================================================

    Record *Writer::acquireRecord( )
    {
        return m_recbuf.acquireRecord( );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::writeRecord( )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        Record *r = m_recbuf.getFirstFullRecord( );
        if( r != NULL )
        {
            m_file << *r;
            m_recbuf.removeFirstFullRecord( );
            m_num_datarecords++;
        }
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::writeRecordDirect( Record *r )
    {
        m_file << *r;
        m_num_datarecords++;
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::flush( )
    {
        //std::cout << "Writer::flush( )" << std::endl;
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        size_t R = m_recbuf.getNumFullRecords();

        for( size_t r=0; r<R; r++ )
            writeRecord( );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::setEventMode( uint8 mode )
    {
        if( m_file.is_open() )
            throw exception::file_open( "" );
        m_header.getEventHeader( ).setMode( mode );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::setEventSamplingRate( float32 fs )
    {
        if( m_file.is_open() )
            throw exception::file_open( "" );
        m_header.getEventHeader( ).setSamplingRate( fs );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addEvent( const Mode1Event &ev )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        if( m_header.getEventHeader( ).getMode() != 1 )
            throw exception::wrong_eventmode( "Expected mode 1" );
        m_eventbuffer.write( reinterpret_cast<const char*>(&ev), sizeof(ev) );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addEvent( uint32 position, uint16 type )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        Mode1Event ev;
        ev.position = position;
        ev.type = type;
        addEvent( ev );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addEvent( const Mode3Event &ev )
    {
        if( !m_file.is_open() )
            throw exception::file_not_open( "" );
        if( m_header.getEventHeader( ).getMode() != 3 )
            throw exception::wrong_eventmode( "Expected mode 3" );
        m_eventbuffer.write( reinterpret_cast<const char*>(&ev), sizeof(ev) );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::addEvent( uint32 position, uint16 type, uint16 channel, uint32 duration )
    {
        Mode3Event ev;
        ev.position = position;
        ev.type = type;
        ev.channel = channel;
        ev.duration = duration;
        addEvent( ev );
    }

    //===================================================================================================
    //===================================================================================================

	void Writer::sampleToEvent( const double sample_time_sec,
		const double sample_physical_value,
		const uint16 channel,
		Mode3Event& ev )
	{
		// channel  is  1-based index as defined in GDF spec
		// chan_idx is 0-based index in signal header
		gdf::uint16 chan_idx = channel - 1;
		gdf::uint32 datatype = this->getSignalHeader_readonly( chan_idx ).get_datatype( );
		// format the event
		ev.position = this->getHeaderAccess( ).getEventHeader( ).secToPos(sample_time_sec);
		ev.type = 0x7fff;	// specific GDF flag for sparse sampling
		ev.channel = channel; // event uses 1-based channel indexing
		double raw_value = this->getSignalHeader_readonly( chan_idx ).phys_to_raw(sample_physical_value);
		if( datatype <= 6 ) {
			// small integer gdftype's as defined in GDF spec
			ev.duration = boost::numeric_cast<uint32>(raw_value);
		} else if( datatype==16 ) {
			// float32 from GDF spec
			ev.value = boost::numeric_cast<float32>(raw_value);
		} else {
			// invalid data type while writing sparse sample
            throw exception::event_conversion_error("Invalid data type for NEQS sample; sample not converted to event.");
		}
	}

	//===================================================================================================
    //===================================================================================================

    void Writer::addEvent( uint32 position, uint16 type, uint16 channel, float32 value )
    {
        Mode3Event ev;
        ev.position = position;
        ev.type = type;
        ev.channel = channel;
        ev.value = value;
        addEvent( ev );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::writeEvents( )
    {
        m_header.getEventHeader( ).clear( );
        while( !m_eventbuffer.eof() )
        {
            switch( m_header.getEventHeader( ).getMode() )
            {
            case 1:
                {
                    Mode1Event ev;
                    m_eventbuffer.read( reinterpret_cast<char*>(&ev), sizeof(ev) );
                    if( !m_eventbuffer.eof( ) )
                        m_header.getEventHeader( ).addEvent( ev );
                } break;
            case 3:
                {
                    Mode3Event ev;
                    m_eventbuffer.read( reinterpret_cast<char*>(&ev), sizeof(ev) );
                    if( !m_eventbuffer.eof( ) )
                        m_header.getEventHeader( ).addEvent( ev );
                } break;
            default: throw exception::invalid_eventmode( boost::lexical_cast<std::string>(m_header.getEventHeader( ).getMode()) ); break;
            }
        }
        m_header.getEventHeader( ).toStream( m_file );
    }

    //===================================================================================================
    //===================================================================================================

    void Writer::triggerRecordFull( Record* /*rec*/ )
    {
        if( m_recbuf.getNumFullRecords() > max_full_records )
        {
            flush( );
        }
    }

    //===================================================================================================
    //===================================================================================================
    void Writer::makeFreeTextEvent( double noteTimeSec, const std::string str, EventDescriptor & ev_desc, Mode1Event & e )
    {
        gdf::EventHeader & evh = m_header.getEventHeader( );
        e.position = evh.secToPos(noteTimeSec);
        e.type = ev_desc.getUserDescEventType(str);
    }

    //===================================================================================================
    //===================================================================================================
    void Writer::makeFreeTextEvent( double noteTimeSec, const std::string str, EventDescriptor & ev_desc, Mode3Event & e )
    {
        gdf::EventHeader & evh = m_header.getEventHeader( );
        e.position = evh.secToPos(noteTimeSec);
        e.type = ev_desc.getUserDescEventType(str);
    }

}
