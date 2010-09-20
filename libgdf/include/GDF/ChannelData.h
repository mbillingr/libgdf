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

#ifndef __CHANNELDATA_H_INCLUDED
#define __CHANNELDATA_H_INCLUDED

#include "ChannelDataBase.h"
#include <vector>
#include <stddef.h>
#include <assert.h>
//#include <iostream>

namespace gdf
{

    /// Contains data samples for a channel of given type and length
    /** Only access functions of type T are reimplemented from ChannelDataBase. Calling addSample(), blitSamples() or fill() with
        the wrong data type throws an exception.
    */
    template<typename T>
    class ChannelData : public ChannelDataBase
    {
    public:
        /// Constructor
        ChannelData( size_t length )
        {
            m_data.resize( length );
            m_writepos = 0;
        }

        /// Copy Constructor
        ChannelData( ChannelDataBase *base )
        {
            ChannelData<T>* other = reinterpret_cast<ChannelData<T>*>( base );
            m_data.resize( other->m_data.size( ) );
            size_t N = m_data.size();
            for( size_t i=0; i<N; i++)
                m_data[i] = other->m_data[i];
            m_writepos = other->m_writepos;
        }

        /// Destructor
        virtual ~ChannelData( )
        {
            //std::cout << "~ChannelData( )" << std::endl;
        }

        /// Add a single sample to the channel. Channel must not be full.
        void addSample( const T value )
        {
            assert( getFree( ) > 0 );
            m_data[m_writepos++] = value;
        }

        /// Blit a given number of samples into channel. That number of samples must be free.
        void blitSamples( const T *values, const size_t num )
        {
            assert( getFree( ) >= num );
            for( size_t i=0; i<num; i++ )
                m_data[m_writepos++] = values[i];
        }

        /// Fills a given number of samples with value. That number of samples must be free.
        void fill( const T value, const size_t num )
        {
            assert( getFree( ) >= num );
            for( size_t i=0; i<num; i++ )
                m_data[m_writepos++] = value;
        }

        /// set sapmle value
        void setSample( size_t pos, T rawval )
        {
            m_data[pos] = rawval;
        }

        /// get sapmle value
        T getSample( size_t pos, T /*dummy*/ )
        {
            return m_data[pos];
        }

        /// Reset read and write positions
        virtual void clear( )
        {
            m_writepos = 0;
        }

        /// Get number of free samples
        size_t getFree( )
        {
            return m_data.size( ) - m_writepos;
        }

        /// Get number of written samples.
        virtual size_t getWritten( )
        {
            return m_writepos;
        }

        /// Serializer
        void tostream( std::ostream &out )
        {
            out.write( reinterpret_cast<char*>(&m_data[0]), sizeof(T)*m_data.size() );
        }

        /// Deserializer
        void fromstream( std::istream &in )
        {
            //char *charbuf = new char[sizeof(T)*m_data.size()];

            in.read( reinterpret_cast<char*>(&m_data[0]), sizeof(T)*m_data.size() );
            /*in.read( charbuf, sizeof(T)*m_data.size() );

            for( size_t i=0; i<m_data.size(); i++ )
                std::cout << ((T*)charbuf)[i] << " ";*/

            /*for( size_t i=0; i<m_data.size(); i++ )
                std::cout << m_data[i] << " ";*/
        }

    protected:
        /// Get reference to data
        std::vector<T> &getData( ) { return m_data; }

        /// Get pointer to data
        std::vector<T> *getDataPtr( ) { return &m_data; }

    private:
        std::vector<T> m_data;
        size_t m_writepos;
    };

}

#endif
