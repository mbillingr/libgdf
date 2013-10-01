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
// Copyright 2013 Owen Kelly

#ifndef __EVENTDESCRIPTOR_H_INCLUDED__
#define __EVENTDESCRIPTOR_H_INCLUDED__

//#include "GDF/TagHeader.h" // for TagField and TagHeader
#include "Types.h"
#include <map>

namespace gdf
{
    class TagField;

    /// Manages standard and user-specified text-descriptions of event codes. 
    //  Stores user-specified text in Header 3 (see GDF 2.20 Table 10 Tag=1). 
    //  Standard descriptions can be loaded from eventcodes.txt.
    class EventDescriptor
    {
    public:

        /// Constructor
        EventDescriptor( );

        /// Destructor
        virtual ~EventDescriptor( );

        /// Serializer
        void toStream( std::ostream &stream );

        /// Deserializer
        void fromStream( std::istream &stream );

        /// Includes string str in the user-specific event description table.
        //  Returns event-type (1..255) corresponding to text description of event.
        //  Repeated additions of the same string have no effect.
        uint16 addUserSpecificDesc( const std::string str );

        /// Returns event-type (1..255) corresponding to text description of event.
        uint16 getUserDescEventType( const std::string str );

        /// Set the description of a particular event type (1..65535).
        void setEventDesc( uint16 typ, const std::string str );

        /// Get the description of a particular event type.
        //  Supports both user-defined events and standard events.
        //  Returns "" if event is has no description.
        const std::string &getEventDesc( uint16 typ );

        /// Fill event description table from file. 
        //  In particular using GDF eventcodes.txt under the alternate license decribed therein.
        void loadEventDescriptions( const std::string &eventfilepath );

        /// Clear event description table and user-event table. 
        void clear( ); 

        /// Copy the event-desc table onto the user-event-desc table. 
        // This utility is helpful if you want control exactly which 
        // codes are associates to which strings. 
        // First use setEventDesc() as many times as needed, then call 
        // copyEventDescToUserDesc, then getUserDescEventType will function correctly.
        void copyEventDescToUserDesc( );

        /// Returns the count of user-specified descriptions that are stored.
        int getNumUserDesc( ) {return m_userdesc.size();}
        
        // Populates class instance from a T-L-V triple.
        void fromTagField( TagField &tf );

        // Copies table of user-specified descriptions into a T-L-V triple.
        TagField toTagField(  );

        std::map<uint16, std::string> m_desc; // description of all events 1..65535

    private:
        std::map<std::string, uint16> m_userdesc; // description of user events 1..255
    };
}

#endif // EVENTDESCRIPTOR_H
