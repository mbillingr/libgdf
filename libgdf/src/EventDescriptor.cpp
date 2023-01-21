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

#include "GDF/EventDescriptor.h"
#include "GDF/TagHeader.h"
#include "GDF/Exceptions.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>

namespace gdf
{
    //===================================================================================================
    //===================================================================================================

    EventDescriptor::EventDescriptor( ) 
    {
        ;
    }

    //===================================================================================================
    //===================================================================================================

    EventDescriptor::~EventDescriptor( )
    {
    }

    //===================================================================================================
    //===================================================================================================

    uint16 EventDescriptor::addUserSpecificDesc( const std::string str )
    {
        uint16 eventType;

        if( str.length() == 0 ) 
        {
            throw exception::event_conversion_error("Cannot store empty user event description.");
        }

        if( m_userdesc.count( str ) )
        {
            // case: the string is already known, just return the corresponding event type
            eventType = m_userdesc[str];
        } else {
            // case: the string has not been seen before
            const int max_user_events_allowed = 255;
            uint16 num_user_events = m_userdesc.size();
            if(num_user_events < max_user_events_allowed)
            {
                // assign an event type to this string; use the next available event number
                eventType = 1 + num_user_events;
                m_userdesc[str] = eventType;
                //add the pair to the global list
                setEventDesc( eventType, str );
            } else {
                throw exception::event_conversion_error("Maximum of number of user event descriptions exceeded.");
            }
        }
        return eventType;
    }

    //===================================================================================================
    //===================================================================================================

    uint16 EventDescriptor::getUserDescEventType( const std::string str )
    {
        uint16 eventType = 0;

        if( m_userdesc.count( str ) )
        {
            // case: the string is already known, just return the corresponding event type
            eventType = m_userdesc[str];
        } else {
            throw exception::event_conversion_error("No Event TYP stored for this annotation: " + str);
        }
        return eventType;
    }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::setEventDesc( uint16 typ, const std::string str )
    {
        m_desc[typ] = str;
    }

    //===================================================================================================
    //===================================================================================================

    const std::string &EventDescriptor::getEventDesc( uint16 typ )
    {
        std::string &result = m_desc[typ];
        return result;
    }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::copyEventDescToUserDesc( )
    {
        for( size_t k=1; k<256; k++ ) 
        {
            if( this->m_desc.count(k) == 1 )
                m_userdesc[m_desc[k]] = k;
        }
    }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::loadEventDescriptions( const std::string &eventfilepath ) 
    {
        std::ifstream infile(eventfilepath.c_str());
        std::string line = "";
        std::string token;
        uint16 eventType;
        std::string eventDesc;
        size_t lineNum = 0;
        while (getline(infile, line)){
            lineNum++;
            if( (line.length() > 0) && (line.c_str()[0] != '#') ) {
                std::stringstream strstr(line);
                std::getline(strstr, token, '\t');
                std::stringstream sstok(token);
                sstok >> std::hex >> eventType;
                if( sstok.fail()==true ) {
                    throw exception::event_conversion_error("Failed to parse event type from eventcode file.");
                }
                std::getline(strstr, eventDesc);
                if( strstr.fail()==true ) {
                    throw exception::event_conversion_error("Failed to parse event description from eventcode file.");
                }
                // For events from file, do not load events from the 
                // user-defined section, just the standardized events.
                if (eventType > 255) {
                    // store the parsed event and description in main table
                    setEventDesc( eventType, eventDesc );
                }
            }
        }
    }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::clear( )
    {
        m_desc.clear();
        m_userdesc.clear();
    }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::fromStream( std::istream &stream )
    {
        // First call the TagField deserializer to load a tag-length-value instance.
        gdf::TagField tf(0);
        tf.fromStream(stream);
        // Populate this object according to contents of the T-L-V
        this->clear();
        this->fromTagField(tf);
     }

    //===================================================================================================
    //===================================================================================================

    void EventDescriptor::fromTagField(const TagField &tf )
    {
        // Format the m_value info into the other data structures
        size_t tagfieldlength = tf.getLength();//m_value.size();
        // taglength is length of the string array
        // size_t taglength = tagfieldlength - 4; //1 char for tag, 3 chars for uint24 length.
        // pos is position where next string begins
        size_t pos = 4; // first string begins after T-L-V header
        std::string desc; 
        std::vector<unsigned char> value = tf.getValue(); // the byte array from the tag field
        for( uint16 eventtyp = 0; eventtyp <= 255 && pos < tagfieldlength; eventtyp++ )
        {
            desc = "";
            int i =0;
            while( (pos + i < tagfieldlength) && value[pos+i] != 0 ) {
                desc.push_back(value[pos+i]);
                i++;
            }
            size_t desclength = desc.length();
            if( desc.length() > 0 ) 
            {
                this->setEventDesc(eventtyp, desc);
            }
            pos += 1 + desclength;
        }
        this->copyEventDescToUserDesc();
    }

    //===================================================================================================
    //===================================================================================================

    gdf::TagField EventDescriptor::toTagField(  )
    {
        gdf::TagField tagfield(1); // init with Tag==1 which is reserved in GDF 220 for Event Descriptions
        std::vector <unsigned char> buf;

        // total memory required is
        // 1 (tag) + 3 (length) + totalStringLen (stored strings) + 
        //  (highestStringIndex+1)(zero terminator for each string incl empty)
        size_t highestStringIndex = 0;
        int totalStringLen = 0;
        for( size_t k=1; k<256; k++ ) 
        {
            if( this->m_desc.count(k) == 1 ) 
            {
                int len = m_desc[k].length();
                if( len > 0 ) {
                    highestStringIndex = k;
                    totalStringLen += len;
                }
            }
        }
        // GDF 220 Table 10: "last entry is identified by an additional empty 
        // string". Therefore increase highestStringIndex by 1.
        highestStringIndex ++ ; 
        if( totalStringLen > 0 ) {
            int count = 1 + 3 + highestStringIndex + 1 + totalStringLen;
            buf.resize(count,'x');
            buf[0] = 1; // tag == 1 for serialized annotations.
            uint32 len = count - 4; // len needs to be stored as a U24
            buf[1] = len & 0x000000ff;
            buf[2] = (len >> 8) & 0x000000ff;
            buf[3] = (len >> 16) & 0x000000ff;
            size_t pos = 4;
            for( size_t k=0; k<=highestStringIndex; k++ ) 
            {
                if( this->m_desc.count(k) == 1 ) 
                {
                    size_t len = m_desc[k].length();
                    for( size_t i = 0; i < len ; i++ ) {
                        buf[pos] = m_desc[k][i];
                        pos++;
                    }
                }
                buf[pos] = 0;
                pos++;
            }
        }
        else
        {
            buf.resize(0,0);
        }
        tagfield.setValue( buf );
        return tagfield;
    }

}
