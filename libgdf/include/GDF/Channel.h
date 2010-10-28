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

#ifndef __CHANNEL_H_INCLUDED__
#define __CHANNEL_H_INCLUDED__

#include "GDF/SignalHeader.h"
#include "ChannelDataBase.h"
#include "GDF/Types.h"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace gdf
{
    class SignalHeader;

    /// Representation of a channel (signal in GDF)
    /**
    */
    class Channel
    {
    public:
        /// Constructor
        Channel( const SignalHeader *sig_hdr, const size_t length );

        /// Constructor
        /** Channel length is initialized to channel's samples_per_record. */
        Channel( const SignalHeader *sig_hdr );

        /// Copy Constructor
        Channel( const Channel &other );

        /// Destructor
        virtual ~Channel( );

        /// Reset read and write positions
        void clear( );

        /// Add a physical sample to the channel.
        /** value is scaled from [phys_min..phys_max] to [dig_min..dig_max] and converted to the channel's data type */
        void addSamplePhys( const double value );

        /// Add a raw sample to the channel.
        /** value is converted to the channel's data type. but otherwise remains unmodified */
        template<typename T> void addSampleRaw( const T rawval );

        /// Blit a number of physical samples into channel.
        /** values are scaled from [phys_min..phys_max] to [dig_min..dig_max] and converted to the channel's data type */
        void blitSamplesPhys( const double *values, size_t num );

        /// Blit a number of raw samples into channel.
        /** values are converted to the channel's data type but otherwise remains unmodified */
        template<typename T> void blitSamplesRaw( const T *values, size_t num );

        /// Fill a number of samples with the same physical value.
        /** value is scaled from [phys_min..phys_max] to [dig_min..dig_max] and converted to the channel's data type */
        void fillPhys( const double value, size_t num );

        /// Fill a number of samples with the same raw value.
        /** value is converted to the channel's data type but otherwise remains unmodified */
        template<typename T> void fillRaw( const T value, size_t num );

        /// set sample value
        void setSamplePhys( size_t pos, double value );

        /// get sample value
        double getSamplePhys( size_t pos );

        /// Blit a number of physical samples from channel to buffer.
        /** values are scaled from [dig_min..dig_max] to [phys_min..phys_max] and converted to double */
        void deblitSamplesPhys( double *values, size_t start, size_t num );

        /// Blit a number of raw samples from channel to buffer.
        template<typename T> void deblitSamplesRaw( T *values, size_t start, size_t num );

        /// Get number of free samples
        size_t getFree( );

        /// Get number of written samples
        size_t getWritten( );

        /// Get type of channel
        uint32 getTypeID( );

        friend std::ostream &operator<<( std::ostream &out, const Channel &c );
        friend std::istream &operator>>( std::istream &in, Channel &c );

    private:
        const SignalHeader *m_signalheader;
        ChannelDataBase *m_data;
    };

    /// Channel Serializer
    std::ostream &operator<<( std::ostream &out, const Channel &c );

    /// Channel Deserializer
    std::istream &operator>>( std::istream &in, Channel &c );



    //===================================================================================================
    //===================================================================================================

    template<typename T> void Channel::addSampleRaw( const T rawval )
    {
        using boost::numeric_cast;

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

    template<typename T> void Channel::blitSamplesRaw( const T *values, size_t num )
    {
        for( size_t i=0; i<num; i++ )
            addSampleRaw( values[i] );
    }

}

#endif
