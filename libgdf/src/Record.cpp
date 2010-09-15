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

#include "GDF/Record.h"
#include "GDF/GDFHeaderAccess.h"
#include "GDF/SignalHeader.h"
#include <limits>
//#include <iostream>

namespace gdf
{
    Record::Record( const GDFHeaderAccess *hdr )
    {
        size_t M = hdr->getMainHeader_readonly( ).get_num_signals( );
        for( size_t i=0; i<M; i++ )
        {
            channels.push_back( boost::shared_ptr<Channel>( new Channel( &hdr->getSignalHeader_readonly(i) ) ) );
        }
    }

    //===================================================================================================
    //===================================================================================================

    Record::~Record( )
    {
//        std::cout << "~Record( )" << std::endl;
    }

    //===================================================================================================
    //===================================================================================================

    void Record::fill( )
    {
        for( size_t i=0; i<channels.size(); i++ )
        {
            switch( channels[i]->getTypeID() )
            {
            default:
                channels[i]->fillPhys( 0.0, channels[i]->getFree() ); break;
            case FLOAT32:
                channels[i]->fillPhys( std::numeric_limits<float>::quiet_NaN(), channels[i]->getFree() ); break;
            case FLOAT64:
                channels[i]->fillPhys( std::numeric_limits<double>::quiet_NaN(), channels[i]->getFree() ); break;
            }
        }
    }

    //===================================================================================================
    //===================================================================================================

    bool Record::isFull( ) const
    {
        for( size_t i=0; i<channels.size(); i++ )
        {
            if( channels[i]->getFree( ) > 0 )
                return false;
        }
        return true;
    }

    //===================================================================================================
    //===================================================================================================

    bool Record::isEmpty( ) const
    {
        for( size_t i=0; i<channels.size(); i++ )
        {
            if( channels[i]->getWritten( ) > 0 )
                return false;
        }
        return true;
    }

    //===================================================================================================
    //===================================================================================================

    Channel *Record::getChannel( const size_t chan_idx )
    {
        return channels[chan_idx].get( );
    }

    //===================================================================================================
    //===================================================================================================

    std::ostream &operator<<( std::ostream &out, const Record &r )
    {
        for( size_t i=0; i<r.channels.size(); i++ )
        {
            out << *r.channels[i];
        }
        return out;
    }

    //===================================================================================================
    //===================================================================================================

    std::istream &operator>>( std::istream &in, Record &r )
    {
        for( size_t i=0; i<r.channels.size(); i++ )
            in >> *r.channels[i];
        return in;
    }
}
