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

#ifndef __EVENTHEADER_H_INCLUDED__
#define __EVENTHEADER_H_INCLUDED__

#include "Types.h"
#include <vector>

#define GDF_MAXNUM_EVENTS 16777215

namespace gdf
{
    struct Mode1Event
    {
        bool operator<( const Mode1Event &e ) const { return position < e.position; }
        uint32 position;
        uint16 type;
    };

    struct Mode3Event
    {
        bool operator<( const Mode3Event &e ) const { return position < e.position; }
        uint32 position;
        uint16 type;
        uint16 channel;
        union {
            uint32 duration;
            float32 value;
        };
    };

    /// Class that provides access to GDF events
    class EventHeader
    {
    public:

        /// Constructor
        EventHeader( );

        /// Destructor
        virtual ~EventHeader( );

        /// Serializer
        void toStream( std::ostream &stream );

        /// Deserializer
        void fromStream( std::istream &stream );

        /// Set Event Mode
        /** mode can be 1 or 3
              1: (default) Events are stored as position,type pairs
              3: Events are stored with position and type, associated to a channel and have a duration
                 or value. */
        void setMode( uint8 mode );

        /// Set Sampling Rate associated with event positions
        /** Events are not actually sampled, but their position is stored in samples rather than seconds.
            In order to convert event positions between time and sample, this sampling rate is used. */
        void setSamplingRate( float32 fs );

		// converts seconds to event position using event sampling rate
		uint32 secToPos( const double sample_time_sec );

		// converts event position to time in seconds (0.0 sec corresponds to the main header start time)
		double posToSec( const uint32 event_pos ) const;

        /// returns event mode
        uint8 getMode( ) { return m_mode; }

        /// returns sampling rate
        float32 getSamplingRate( ) { return m_efs; }

        /// Number of events
        uint32 getNumEvents( );

        /// Returns a Mode 1 Event
        void getEvent( uint32 index, Mode1Event &ev );

        /// Returns all Mode 1 Events
        std::vector<Mode1Event> getMode1Events () const;

        /// Returns all Mode 3 Events
        std::vector<Mode3Event> getMode3Events () const;

        /// Returns indices of sparse samples associated with a channel
		//    chan_idx is event.CHN from GDF spec (a 1-based index)
        std::vector<uint32> getSparseSamples (const size_t chan_idx);

        /// Returns a Mode 3 Event
        void getEvent( uint32 index, Mode3Event &ev );

        /// Add a Mode 1 Event
        void addEvent( const Mode1Event &ev );

        /// Add a Mode 3 Event
        void addEvent( const Mode3Event &ev );

        /// Sort Events by position
        void sort( );

        /// Clears all Events
        void clear( );

    private:
        uint8 m_mode;
        //uint32 m_num_ev;  /// only 3 bytes are used. Maximum is 16777215
        float32 m_efs;    /// sampling rate associated with event positions

        std::vector<Mode1Event> m_mode1;
        std::vector<Mode3Event> m_mode3;
    };
}

#endif // EVENTHEADER_H
