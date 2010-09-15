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

#include "GDF/Channel.h"
#include "GDF/ChannelData.h"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
//#include <iostream>

namespace gdf {

    Channel::Channel( const SignalHeader *sig_hdr, const size_t length )
    {
        m_signalheader = sig_hdr;

        switch( m_signalheader->get_datatype( ) )
        {
        case INT8:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int8>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT8:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint8>( length ) );
                m_data = temp_pointer;
            } break;
        case INT16:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int16>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT16:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint16>( length ) );
                m_data = temp_pointer;
            } break;
        case INT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int32>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint32>( length ) );
                m_data = temp_pointer;
            } break;
        case INT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int64>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint64>( length ) );
                m_data = temp_pointer;
            } break;
        case FLOAT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<float32>( length ) );
                m_data = temp_pointer;
            } break;
        case FLOAT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<float64>( length ) );
                m_data = temp_pointer;
            } break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        }
    }

    //===================================================================================================
    //===================================================================================================

    Channel::Channel( const SignalHeader *sig_hdr )
    {
        m_signalheader = sig_hdr;

        size_t length = sig_hdr->get_samples_per_record( );

        switch( m_signalheader->get_datatype( ) )
        {
        case INT8:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int8>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT8:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint8>( length ) );
                m_data = temp_pointer;
            } break;
        case INT16:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int16>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT16:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint16>( length ) );
                m_data = temp_pointer;
            } break;
        case INT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int32>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint32>( length ) );
                m_data = temp_pointer;
            } break;
        case INT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<int64>( length ) );
                m_data = temp_pointer;
            } break;
        case UINT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<uint64>( length ) );
                m_data = temp_pointer;
            } break;
        case FLOAT32:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<float32>( length ) );
                m_data = temp_pointer;
            } break;
        case FLOAT64:
            {
                boost::shared_ptr<ChannelDataBase> temp_pointer( new ChannelData<float64>( length ) );
                m_data = temp_pointer;
            } break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        }
    }

    //===================================================================================================
    //===================================================================================================

    Channel::~Channel( )
    {
        //std::cout << "~Channel( )" << std::endl;
    }

    //===================================================================================================
    //===================================================================================================

    void Channel::addSamplePhys( const double value )
    {
        using boost::numeric_cast;

        double rawval = m_signalheader->phys_to_raw( value );

        switch( m_signalheader->get_datatype( ) )
        {
        case INT8: m_data->addSample( numeric_cast<int8>(rawval) ); break;
        case UINT8: m_data->addSample( numeric_cast<uint8>(rawval) ); break;
        case INT16: m_data->addSample( numeric_cast<int16>(rawval) ); break;
        case UINT16: m_data->addSample( numeric_cast<uint16>(rawval) ); break;
        case INT32: m_data->addSample( numeric_cast<int32>(rawval) ); break;
        case UINT32: m_data->addSample( numeric_cast<uint32>(rawval) ); break;
        case INT64: m_data->addSample( numeric_cast<int64>(rawval) ); break;
        case UINT64: m_data->addSample( numeric_cast<uint64>(rawval) ); break;
        case FLOAT32: m_data->addSample( numeric_cast<float32>(rawval) ); break;
        case FLOAT64: m_data->addSample( numeric_cast<float64>(rawval) ); break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        };
    }

    //===================================================================================================
    //===================================================================================================

    void Channel::blitSamplesPhys( const double *values, size_t num )
    {
        for( size_t i=0; i<num; i++ )
            addSamplePhys( values[i] );
    }

    //===================================================================================================
    //===================================================================================================

    void Channel::fillPhys( const double value, size_t num )
    {
        using boost::numeric_cast;
        double rawval = value * m_signalheader->phys_to_raw( value );

        switch( m_signalheader->get_datatype( ) )
        {
        case INT8: m_data->fill( numeric_cast<int8>(rawval), num ); break;
        case UINT8: m_data->fill( numeric_cast<uint8>(rawval), num ); break;
        case INT16: m_data->fill( numeric_cast<int16>(rawval), num ); break;
        case UINT16: m_data->fill( numeric_cast<uint16>(rawval), num ); break;
        case INT32: m_data->fill( numeric_cast<int32>(rawval), num ); break;
        case UINT32: m_data->fill( numeric_cast<uint32>(rawval), num ); break;
        case INT64: m_data->fill( numeric_cast<int64>(rawval), num ); break;
        case UINT64: m_data->fill( numeric_cast<uint64>(rawval), num ); break;
        case FLOAT32: m_data->fill( numeric_cast<float32>(rawval), num ); break;
        case FLOAT64: m_data->fill( numeric_cast<float64>(rawval), num ); break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        };
    }

    //===================================================================================================
    //===================================================================================================

    template<typename T> void Channel::fillRaw( const T rawval, size_t num )
    {
        using boost::numeric_cast;

        switch( m_signalheader->get_datatype( ) )
        {
        case INT8: m_data->fill( numeric_cast<int8>(rawval), num ); break;
        case UINT8: m_data->fill( numeric_cast<uint8>(rawval), num ); break;
        case INT16: m_data->fill( numeric_cast<int16>(rawval), num ); break;
        case UINT16: m_data->fill( numeric_cast<uint16>(rawval), num ); break;
        case INT32: m_data->fill( numeric_cast<int32>(rawval), num ); break;
        case UINT32: m_data->fill( numeric_cast<uint32>(rawval), num ); break;
        case INT64: m_data->fill( numeric_cast<int64>(rawval), num ); break;
        case UINT64: m_data->fill( numeric_cast<uint64>(rawval), num ); break;
        case FLOAT32: m_data->fill( numeric_cast<float32>(rawval), num ); break;
        case FLOAT64: m_data->fill( numeric_cast<float64>(rawval), num ); break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        };
    }

    //===================================================================================================
    //===================================================================================================

    void Channel::setSamplePhys( size_t pos, double value )
    {
        using boost::numeric_cast;

        double rawval = m_signalheader->phys_to_raw( value );

        switch( m_signalheader->get_datatype() )
        {
        case INT8: m_data->setSample( pos, numeric_cast<int8>( rawval ) ); break;
        case UINT8: m_data->setSample( pos, numeric_cast<uint8>( rawval ) ); break;
        case INT16: m_data->setSample( pos, numeric_cast<int16>( rawval ) ); break;
        case UINT16: m_data->setSample( pos, numeric_cast<uint16>( rawval ) ); break;
        case INT32: m_data->setSample( pos, numeric_cast<int32>( rawval ) ); break;
        case UINT32: m_data->setSample( pos, numeric_cast<uint32>( rawval ) ); break;
        case INT64: m_data->setSample( pos, numeric_cast<int64>( rawval ) ); break;
        case UINT64: m_data->setSample( pos, numeric_cast<uint64>( rawval ) ); break;
        case FLOAT32: m_data->setSample( pos, numeric_cast<float32>( rawval ) ); break;
        case FLOAT64: m_data->setSample( pos, numeric_cast<float64>( rawval ) ); break;
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        }
    }

    //===================================================================================================
    //===================================================================================================

    double Channel::getSamplePhys( size_t pos )
    {
        using boost::numeric_cast;

        switch( m_signalheader->get_datatype() )
        {
        case INT8: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, int8() ) ) );
        case UINT8: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, uint8() ) ) );
        case INT16: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, int16() ) ) );
        case UINT16: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, uint16() ) ) );
        case INT32: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, int32() ) ) );
        case UINT32: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, uint32() ) ) );
        case INT64: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, int64() ) ) );
        case UINT64: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, uint64() ) ) );
        case FLOAT32: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, float32() ) ) );
        case FLOAT64: return m_signalheader->raw_to_phys( numeric_cast<double>( m_data->getSample( pos, float64() ) ) );
        default: throw exception::invalid_type_id( boost::lexical_cast<std::string>(m_signalheader->get_datatype( )) ); break;
        }
    }

    //===================================================================================================
    //===================================================================================================

    void Channel::deblitSamplesPhys( double *values, size_t start, size_t num )
    {
        for( size_t i=0; i<num; i++ )
        {
            values[i] = getSamplePhys( start + i );
        }
    }

    //===================================================================================================
    //===================================================================================================

    size_t Channel::getFree( )
    {
        return m_data->getFree( );
    }

    //===================================================================================================
    //===================================================================================================

    size_t Channel::getWritten( )
    {
        return m_data->getWritten( );
    }

    //===================================================================================================
    //===================================================================================================

    uint32 Channel::getTypeID( )
    {
        return m_signalheader->get_datatype( );
    }

    //===================================================================================================
    //===================================================================================================

    std::ostream &operator<<( std::ostream &out, const Channel &c )
    {
        c.m_data->tostream( out );
        return out;
    }

    //===================================================================================================
    //===================================================================================================

    std::istream &operator>>( std::istream &in, Channel &c )
    {
        c.m_data->fromstream( in );
        return in;
    }

}
