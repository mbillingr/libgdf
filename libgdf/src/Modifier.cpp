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

#include "GDF/Modifier.h"
#include "GDF/Record.h"
#include "GDF/tools.h"
#include <boost/numeric/conversion/cast.hpp>
#include <fstream>
#include <iostream>

namespace gdf
{
    Modifier::Modifier( )
    {
        m_cache_enabled = true;
        m_events_changed = false;
    }

    //===================================================================================================
    //===================================================================================================

    Modifier::~Modifier( )
    {
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::open( std::string filename )
    {
        Reader::open( filename );

        initCache( );
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::close( )
    {
        m_file.close( );
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::saveChanges( )
    {
        std::fstream ofile( m_filename.c_str(), std::ios_base::out | std::ios_base::in | std::ios::binary );

        size_t R = boost::numeric_cast<size_t>( m_header.getMainHeader_readonly().get_num_datarecords( ) );

        for( size_t i=0; i<R; i++ )
        {
            if( m_record_changed[i] )
            {
                ofile.seekp( m_record_offset + m_record_length * i );
                ofile << *m_record_cache[i];
            }
        }

        if( m_events_changed )
        {
            ofile.seekp( m_event_offset );
            m_events->toStream( ofile );
        }

        ofile.close( );
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::initCache( )
    {
        Reader::initCache( );
        m_record_changed.resize( boost::numeric_cast<size_t>( m_header.getMainHeader_readonly().get_num_datarecords() ), false );
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::resetCache( )
    {

        for( size_t i=0; i<m_record_cache.size(); i++ )
        {
            if( m_record_cache[i] != NULL )
            {
                delete m_record_cache[i];
                m_record_cache[i] = NULL;
                m_record_changed[i] = false;
            }
        }
    }

    //===================================================================================================
    //===================================================================================================

    double Modifier::getSample( uint16 channel_idx, size_t sample_idx )
    {
        Record *r = getRecordPtr( findRecord( channel_idx, sample_idx ) );
        size_t spr = m_header.getSignalHeader_readonly( channel_idx ).get_samples_per_record( );
        return r->getChannel( channel_idx )->getSamplePhys( sample_idx % spr );
    }

    //===================================================================================================
    //===================================================================================================

    void Modifier::setSample( uint16 channel_idx, size_t sample_idx, double value )
    {
        size_t record = findRecord( channel_idx, sample_idx );
        Record *r = getRecordPtr( record );
        size_t spr = m_header.getSignalHeader_readonly( channel_idx ).get_samples_per_record( );
        r->getChannel( channel_idx )->setSamplePhys( sample_idx % spr, value );
        m_record_changed[record] = true;
    }

    //===================================================================================================
    //===================================================================================================

    EventHeader *Modifier::getEventHeader( )
    {
        if( m_events == NULL )
        {
            if( m_file.is_open() )
            {
                m_events = new EventHeader( );
                m_file.seekg( m_event_offset );
                readEvents( );
            }
            else
                throw exception::file_not_open( "when attempting to read events" );
        }
        m_events_changed = true;
        return m_events;
    }

    //===================================================================================================
    //===================================================================================================

    const EventHeader *Modifier::getEventHeader_readonly( )
    {
        if( m_events == NULL )
        {
            if( m_file.is_open() )
            {
                m_events = new EventHeader( );
                m_file.seekg( m_event_offset );
                readEvents( );
            }
            else
                throw exception::file_not_open( "when attempting to read events" );
        }
        return m_events;
    }
}
