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
// Copyright 2010 Martin Billinger, Christoph Eibel

#include "GDF/EventHeader.h"
#include "GDF/Exceptions.h"
#include <algorithm>
#include <iostream>
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

        if( getMode() == 1 )
        {
            for( size_t i=0; i<nev; i++ )
            {
                Mode1Event e;
                e.position = positions[i];
                e.type = types[i];
                addEvent( e );
            }
        }
        else if( getMode() == 3 )
        {
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
        }
        else
            throw exception::invalid_eventmode( boost::lexical_cast<std::string>( getMode() ) );
    }

    void EventHeader::setMode( uint8 mode )
    {
        if( m_mode1.size() > 0 || m_mode3.size() > 0 )
            throw exception::illegal_eventmode_change( "EventHeader::setMode called, but header already contains events" );
        m_mode = mode;
    }

    void EventHeader::setSamplingRate( float32 fs )
    {
        m_efs = fs;
    }

    uint32 EventHeader::getNumEvents( )
    {
        switch( m_mode )
        {
        case 1: return m_mode1.size( ); break;
        case 3: return m_mode3.size( ); break;
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
    void EventHeader::getEvent( uint32 index, Mode1Event &ev )
    {
        if( m_mode != 1 )
            throw exception::wrong_eventmode( "Expecting mode 1" );
        ev = m_mode1[index];
    }

    void EventHeader::getEvent( uint32 index, Mode3Event &ev )
    {
        if( m_mode != 3 )
            throw exception::wrong_eventmode( "Expecting mode 3" );
        ev = m_mode3[index];
    }

    void EventHeader::addEvent( const Mode1Event &ev )
    {
        if( m_mode != 1 )
            throw exception::wrong_eventmode( "Expecting mode 1" );
        m_mode1.push_back( ev );
    }

    void EventHeader::addEvent( const Mode3Event &ev )
    {
        if( m_mode != 3 )
            throw exception::wrong_eventmode( "Expecting mode 3" );
        m_mode3.push_back( ev );
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
