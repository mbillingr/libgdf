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

#include "GDF/Reader.h"
#include "GDF/tools.h"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
//#include <iostream>

namespace gdf
{
    Reader::Reader( )
    {
        m_record_nocache = NULL;
        m_cache_enabled = true;
        m_events = NULL;
        m_filename = "";
    }

    //===================================================================================================
    //===================================================================================================

    Reader::~Reader( )
    {
        resetCache( );
        if( m_record_nocache ) delete m_record_nocache;
        if( m_events ) delete m_events;
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::open( std::string filename )
    {
        assert( !m_file.is_open() );
        m_file.open( filename.c_str(), std::ios_base::in | std::ios::binary );
        if( m_file.fail() )
            throw exception::file_exists_not( filename );

        m_filename = filename;
        if( m_record_nocache ) delete m_record_nocache;
        m_record_nocache = NULL;

        if( m_events ) delete m_events;
        m_events = NULL;

        m_file >> m_header;

        // determine record length
        m_record_length = 0;
        for( size_t i=0; i<m_header.getMainHeader_readonly().get_num_signals(); i++ )
        {
            size_t samplesize = datatype_size( m_header.getSignalHeader_readonly( i ).get_datatype( ) );
            m_record_length += samplesize * m_header.getSignalHeader_readonly( i ).get_samples_per_record( );

            double fs = m_header.getSignalHeader( i ).get_samples_per_record( ) * m_header.getMainHeader_readonly().get_datarecord_duration(1) / m_header.getMainHeader_readonly().get_datarecord_duration(0);
            m_header.getSignalHeader( i ).set_samplerate( boost::numeric_cast<uint32>(fs) );
        }

        m_record_offset = m_header.getMainHeader_readonly().get_header_length( ) * 256;
        m_event_offset = boost::numeric_cast<size_t>( m_record_offset + m_header.getMainHeader_readonly().get_num_datarecords() * m_record_length );

        initCache( );
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::close( )
    {
        m_file.close( );
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::enableCache( bool b )
    {
        m_cache_enabled = b;
        resetCache( );
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::initCache( )
    {
        resetCache( );
        m_record_cache.clear( );
        size_t num_records = boost::numeric_cast<size_t>( m_header.getMainHeader_readonly().get_num_datarecords() );
        m_record_cache.resize( num_records, NULL );
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::resetCache( )
    {
        for( std::list<size_t>::iterator it = m_cache_entries.begin(); it != m_cache_entries.end(); it++ )
        {
            delete m_record_cache[*it];
            m_record_cache[*it] = NULL;
        }
        m_cache_entries.clear( );

        if( m_record_nocache ) delete m_record_nocache;
        m_record_nocache = new Record( &m_header );
    }

    //===================================================================================================
    //===================================================================================================

    size_t Reader::findRecord( uint16 channel_idx, size_t sample_idx )
    {
        return boost::numeric_cast<size_t>( floor( ((double)sample_idx) / ((double)m_header.getSignalHeader_readonly( channel_idx ).get_samples_per_record( )) ) );
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::getSignals( std::vector< std::vector<double> > &buffer, double start_time, double end_time, std::vector<uint16> signal_indices )
    {
        using boost::numeric_cast;

        if( signal_indices.size() == 0 )
        {
            signal_indices.resize( m_header.getMainHeader_readonly().get_num_signals() );
            for( size_t i=0; i<m_header.getMainHeader_readonly().get_num_signals(); i++ )
                signal_indices[i] = i;
        }

        buffer.resize( signal_indices.size() );

        double record_rate = m_header.getMainHeader_readonly().get_datarecord_duration(1) / m_header.getMainHeader_readonly().get_datarecord_duration(0);
        size_t record = boost::numeric_cast<size_t>( floor( start_time * record_rate ) );


        std::vector<size_t> start, samples_to_go, readpos, writepos;

        start.resize( signal_indices.size() );
        samples_to_go.resize( signal_indices.size() );
        readpos.resize( signal_indices.size() );
        writepos.resize( signal_indices.size() );
        for( size_t i=0; i<signal_indices.size(); i++ )
        {
            SignalHeader *sh = &m_header.getSignalHeader( signal_indices[i] );
            double fs = sh->get_samplerate( );
            start[i] = boost::numeric_cast<size_t>( floor( start_time * fs ) );
            samples_to_go[i] = boost::numeric_cast<size_t>( floor( end_time * fs ) );
            if( end_time < 0 )
                samples_to_go[i] = boost::numeric_cast<size_t>( m_header.getMainHeader_readonly().get_num_datarecords() * sh->get_samples_per_record() );
            samples_to_go[i] = std::min( samples_to_go[i], boost::numeric_cast<size_t>( m_header.getMainHeader_readonly().get_num_datarecords() * sh->get_samples_per_record() ) );
            samples_to_go[i] -= start[i];
            buffer[i].resize( boost::numeric_cast<size_t>( samples_to_go[i] ) );
            readpos[i] = start[i] % sh->get_samples_per_record();
        }

        while( sum(samples_to_go) > 0 )
        {
            Record *r = getRecordPtr( record );
            for( size_t i=0; i<signal_indices.size(); i++ )
            {
                SignalHeader *sh = &m_header.getSignalHeader( signal_indices[i] );
                size_t n = std::min( boost::numeric_cast<size_t>(sh->get_samples_per_record( )) - readpos[i], samples_to_go[i] );
                r->getChannel( signal_indices[i] )->deblitSamplesPhys( &buffer[i][writepos[i]], readpos[i], n );
                samples_to_go[i] -= n;
                writepos[i] += n;
                readpos[i] = 0;
            }
            record++;
        }
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::getSignal( uint16 channel_idx, double *buffer, size_t start, size_t end  )
    {
        using boost::numeric_cast;

        SignalHeader *sh = &m_header.getSignalHeader( channel_idx );

        if( end <= start )
            end = boost::numeric_cast<size_t>( sh->get_samples_per_record( ) * m_header.getMainHeader_readonly().get_num_datarecords( ) );

        size_t record = boost::numeric_cast<size_t>( floor( ((double)start)/((double)sh->get_samples_per_record()) ) );
        size_t readpos = start % sh->get_samples_per_record();
        size_t writepos = 0;
        size_t samples_to_go = end - start;

        while( samples_to_go > 0 )
        {
            Record *r = getRecordPtr( record );

            size_t n = std::min( (size_t)sh->get_samples_per_record( ) - readpos, samples_to_go );
            r->getChannel( channel_idx )->deblitSamplesPhys( &buffer[writepos], readpos, n );
            samples_to_go -= n;
            writepos += n;
            readpos = 0;

            record++;
        }
    }

    //===================================================================================================
    //===================================================================================================

    double Reader::getSample( uint16 channel_idx, size_t sample_idx )
    {
        Record *r = getRecordPtr( findRecord( channel_idx, sample_idx ) );
        size_t spr = m_header.getSignalHeader_readonly( channel_idx ).get_samples_per_record( );
        return r->getChannel( channel_idx )->getSamplePhys( sample_idx % spr );
    }

    //===================================================================================================
    //===================================================================================================

    Record *Reader::getRecordPtr( size_t index )
    {
        assert( index < boost::numeric_cast<size_t>(m_header.getMainHeader_readonly().get_num_datarecords()) );
        Record *r = m_record_cache[index];
        if( r == NULL )
        {
            size_t pos = m_record_offset + m_record_length*index;
            m_file.seekg( pos );
            if( m_cache_enabled )
            {
                r = new Record( &m_header );
                m_file >> *r;
                m_record_cache[index] = r;
                m_cache_entries.push_back( index );
            }
            else
            {
                m_file >> *m_record_nocache;
                r = m_record_nocache;
            }
        }
        return r;
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::readRecord( size_t index, Record *rec )
    {
        assert( index < boost::numeric_cast<size_t>(m_header.getMainHeader_readonly().get_num_datarecords()) );
        Record *r = m_record_cache[index];
        if( r == NULL )
        {
            size_t pos = m_record_offset + m_record_length*index;
            m_file.seekg( pos );
            if( m_cache_enabled )
            {
                r = new Record( &m_header );
                m_file >> *r;
                m_record_cache[index] = r;
                m_cache_entries.push_back( index );
            }
            else
            {
                m_file >> *rec; // read directly
                return;
            }
        }
        *rec = *r;  // copy from cache
    }

    //===================================================================================================
    //===================================================================================================

    void Reader::precacheRecords( size_t start, size_t end )
    {
        for( size_t i=start; i<end; i++ )
            getRecordPtr( i );
    }

    //===================================================================================================
    //===================================================================================================

    EventHeader *Reader::getEventHeader( )
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

    //===================================================================================================
    //===================================================================================================

    void Reader::readEvents( )
    {
        m_events->fromStream( m_file );
    }
}
