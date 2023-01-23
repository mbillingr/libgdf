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
// Copyright 2010, 2013 Martin Billinger, Christoph Eibel, Owen Kelly

#include "GDF/EventHeader.h"
#include "GDF/Exceptions.h"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>

namespace gdf
{
    EventHeader::EventHeader( ) : m_mode1(), m_mode3()
    {
        m_mode = 1;
        m_efs = -1;
    }

    EventHeader::~EventHeader( )
    {
    }

    void EventHeader::toStream( std::ostream &stream )
    {
        stream.write( reinterpret_cast<const char*>(&m_mode), 1 );

        // convert number of events to 24bit little endian representation
        uint32 numev = getNumEvents( );
        uint8 tmp[3];
        tmp[0] = numev % 256;
        tmp[1] = (numev / 256) % 256;
        tmp[2] = (numev / 65536) % 256;
        stream.write( reinterpret_cast<const char*>(tmp), 3 );

        writeLittleEndian( stream, getSamplingRate( ) );

        if( getMode() == 1 )
        {
            Mode1Event e;
            // write event positions
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.position );
            }
            // write event types
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.type );
            }
        }
        else if( getMode() == 3 )
        {
            Mode3Event e;
            // write event positions
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.position );
            }
            // write event types
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.type );
            }
            // write event channels
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.channel );
            }
            // write event durations
            for( size_t i=0; i<getNumEvents(); i++)
            {
                getEvent( i, e );
                writeLittleEndian( stream, e.duration );
            }
        }
    }

    void EventHeader::fromStream( std::istream &stream )
    {
        clear( );

        uint8 mode;
        stream.read( reinterpret_cast<char*>(&mode), sizeof(mode) );
        setMode( mode );

        uint8 tmp[3];
        stream.read( reinterpret_cast<char*>(tmp), 3 );

        uint32 nev = tmp[0] + tmp[1]*256 + tmp[2]*65536;

        float32 efs;
        readLittleEndian( stream, efs );
        setSamplingRate( efs );

        std::vector<uint32> positions;
        std::vector<uint16> types;
        positions.resize( nev );
        types.resize( nev );
        for( size_t i=0; i<nev; i++ )
            readLittleEndian( stream, positions[i] );
        for( size_t i=0; i<nev; i++ )
            readLittleEndian( stream, types[i] );

        const bool has_time_stamps = mode&0x04;
        if (has_time_stamps)
            std::cerr << "Warning non equidistant sampling is not yet implemented." << std::endl;

        //  At this point mode have been validated.
        //  mode&3 can be only 1 or 3.

        switch (mode&0x03) {
            case 1: {
                for( size_t i=0; i<nev; i++ )
                {
                    Mode1Event e;
                    e.position = positions[i];
                    e.type = types[i];
                    addEvent( e );
                }
                break;
            }
            case 3: {
                std::vector<uint16> channels;
                std::vector<uint32> durations;
                channels.resize( nev );
                durations.resize( nev );
                for( size_t i=0; i<nev; i++ )
                    readLittleEndian( stream, channels[i] );
                for( size_t i=0; i<nev; i++ )
                    readLittleEndian( stream, durations[i] );
                for( size_t i=0; i<nev; i++ )
                {
                    Mode3Event e;
                    e.position = positions[i];
                    e.type = types[i];
                    e.channel = channels[i];
                    e.duration = durations[i];
                    addEvent( e );
                }
                break;
            }
            default:
                throw exception::invalid_eventmode( boost::lexical_cast<std::string>( getMode() ) );
        }
    }

    void EventHeader::setMode( uint8 mode )
    {
        if( m_mode1.size() > 0 || m_mode3.size() > 0 )
            throw exception::illegal_eventmode_change( "EventHeader::setMode called, but header already contains events" );
        if ((mode&0x1)!=1 && (mode>>1)>3)
            throw exception::wrong_eventmode(std::to_string(mode));
        m_mode = mode;
    }

    void EventHeader::setSamplingRate( float32 fs )
    {
        m_efs = fs;
    }

    uint32 EventHeader::getNumEvents( )
    {
        switch( m_mode&0x03 )
        {
        case 1: return m_mode1.size( );
        case 3: return m_mode3.size( );
        default: throw exception::invalid_eventmode( boost::lexical_cast<std::string>( m_mode ) );
        }
    }

    //-------------------------------------------------------------------------
    std::vector<Mode1Event> EventHeader::getMode1Events () const
    {
        return m_mode1;
    }

    //-------------------------------------------------------------------------
    std::vector<Mode3Event> EventHeader::getMode3Events () const
    {
        return m_mode3;
    }

	//-------------------------------------------------------------------------
	std::vector<uint32> EventHeader::getSparseSamples (const size_t chan_idx)
	{
		std::vector<uint32> index_list;
		unsigned int num_events = this->getNumEvents();
		// loop over all mode 3 events
		for(unsigned int m = 0; m < num_events; m++)
		{
			if( (m_mode3[m].channel == chan_idx) && (m_mode3[m].type == 0x7fff) )
			{
				index_list.push_back( m );
			}
		}
		return index_list;
	}

    //-------------------------------------------------------------------------
    void EventHeader::getEvent( uint32 index, Mode1Event &ev )
    {
        if( (m_mode&0x03) != 1 )
            throw exception::wrong_eventmode( "Expecting mode 1" );
        ev = m_mode1[index];
    }

    void EventHeader::getEvent( uint32 index, Mode3Event &ev )
    {
        if( (m_mode&0x03) != 3 )
            throw exception::wrong_eventmode( "Expecting mode 3" );
        ev = m_mode3[index];
    }

    void EventHeader::addEvent( const Mode1Event &ev )
    {
        if( (m_mode&0x03) != 1 )
            throw exception::wrong_eventmode( "Expecting mode 1" );
        m_mode1.push_back( ev );
    }

    void EventHeader::addEvent( const Mode3Event &ev )
    {
        if( (m_mode&0x03) != 3 )
            throw exception::wrong_eventmode( "Expecting mode 3" );
        m_mode3.push_back( ev );
    }

    uint32 EventHeader::secToPos( const double sample_time_sec )
    {
        if( m_efs <= 0 )
            throw exception::invalid_operation( "Event table m_efs not set or not valid." );
        int32 raw_pos = (int32) ceil(sample_time_sec * m_efs);
        if( raw_pos < 0 ) {
            throw exception::invalid_operation( "Event time < 0 [sec] not allowed." );
        }
        // GDF 2.20 item 32: "Then the position of all events is saved in 
        // 32-bit integers using a one-based indexing (position of first sample
        // is 1, not 0)"
        uint32 pos = 1 + raw_pos;
        return pos;
    }

    double EventHeader::posToSec( const uint32 event_pos ) const
    {
        if( m_efs <= 0 )
            throw exception::invalid_operation( "Event table m_efs not set or not valid." );
        int32 raw_pos = event_pos - 1; // "-1" is for "one-based indexing" see EventHeader::secToPos
        if( raw_pos < 0 )
            throw exception::invalid_operation( "Event time < 0 [sec] not allowed." );
        double event_time_sec = raw_pos * 1.0 / m_efs; 
        return event_time_sec;
    }

    void EventHeader::sort( )
    {
        std::sort( m_mode1.begin(), m_mode1.end() );
        std::sort( m_mode3.begin(), m_mode3.end() );
    }

    void EventHeader::clear( )
    {
        m_mode1.clear( );
        m_mode3.clear( );
    }

}
