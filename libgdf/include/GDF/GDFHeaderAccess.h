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

#ifndef __GDFHEADERACCESS_H_INCLUDED__
#define __GDFHEADERACCESS_H_INCLUDED__

#include "GDF/EventHeader.h"
#include "GDF/SignalHeader.h"
#include "GDF/MainHeader.h"
#include "GDF/TagHeader.h"
#include <map>
#include <iostream>

namespace gdf
{
    class GDFHeaderAccess
    {
    public:

        /// Constructor
        GDFHeaderAccess( );

        /// Destructor
        virtual ~GDFHeaderAccess( );

        /// Reset header to initial state
        void clear( );

        /// perform sanity check on header and normalize header information.
        /** If there are issues with the configuration an exception with detailed information is thrown.*/
        void sanitize( );

        /// set record duration
        /** Normally record duration is automatically set to the smallest possible value. This functionality
            is overriden when manually setting the record duration. */
        void setRecordDuration( uint32 num, uint32 den );

        /// enable automatic record duration
        void enableAutoRecordDuration( );

        const MainHeader &getMainHeader_readonly( ) const;
        MainHeader &getMainHeader( );

        const SignalHeader &getSignalHeader_readonly( size_t idx ) const;
        SignalHeader &getSignalHeader( size_t idx );

        bool createSignal( size_t index, bool throwexc = false );

        size_t getFirstFreeSignalIndex( );

        size_t getNumSignals( ) const;

        void swapSignals( size_t a, size_t b );

        void relocateSignal( size_t src, size_t dst );

        EventHeader &getEventHeader( );

        friend std::ostream& operator<< (std::ostream& out, const GDFHeaderAccess& hdr);
        friend std::istream& operator>> (std::istream& in, GDFHeaderAccess& hdr);

        /// Lock write access to headers
        void setLock( bool b) { m_lock = b; }

        // Utility functions

        size_t getNumberOfSamplesInSignal( size_t signal_index ) const;

    private:
        MainHeader m_mainhdr;
        std::map<uint16,SignalHeader> m_sighdr;
        EventHeader m_events;
        TagHeader m_taghdr;
        uint32 drd_num, drd_den;
        bool m_lock;
    };

    /// Header Serializer
    std::ostream& operator<< (std::ostream& out, const GDFHeaderAccess& hdr);

    /// Header Deserializer
    std::istream& operator>> (std::istream& in, GDFHeaderAccess& hdr);
}

#endif
