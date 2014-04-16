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
// Copyright 2010, 2013 Martin Billinger, Owen Kelly

#ifndef __TAGHEADER_H_INCLUDED__
#define __TAGHEADER_H_INCLUDED__

#include "Types.h"
#include "EventHeader.h"
#include "EventDescriptor.h"
//#include "GDFHeaderAccess.h"
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace gdf
{
    class GDFHeaderAccess;

    /// Contains all information required to construct a T-L-V item in GDF tag header
    class TagField 
    {
    public:
        /// Constructor
        TagField( uint8 tag=0 );

        /// Destructor
        virtual ~TagField( );

        /// Serializer
        virtual void toStream( std::ostream &stream ) const;

        /// Deserializer
        virtual void fromStream( std::istream &stream );

        /// Returns number the length of the header (number of bytes)
        size_t getLength() const { return m_value.size(); } 

        /// Set the value explicitly. 
        void setValue( std::vector <unsigned char> value ){ m_value = value; }

        /// Get the value 
        std::vector <unsigned char> & getValue() { return m_value; }

        /// Update the output representation of the T-L-V field. 
        virtual void finalize(){;}

        // Return the tag number
        int getTagNumber(){return int(m_tag);}

    private:
        std::vector <unsigned char> m_value;
        uint8 m_tag; 
    };

    /// Contains all information required to construct the optional GDF tag header
    class TagHeader
    {
    public:
        /// Constructor
        TagHeader( );

        /// Destructor
        virtual ~TagHeader( );

        /// Serializer
        void toStream( std::ostream &stream ) const;

        /// Deserializer
        void fromStream( std::istream &stream , GDFHeaderAccess& hdr);

        /// Returns number the length of the header (number of 256 byte blocks)
        size_t getLength( ) {return m_hdr3SizeBlocks;}

        /// Calculates number of 256 byte blocks required to hold the tags that have been stored.
        //  Useful when writing GDF files.
        void setLength( );

        /// Update the output representation of the T-L-V field. 
        void finalize();

        /// Sets the number of 256 byte blocks to reserve for header3 data.
        //  Useful when reading GDF files.
        void setLength( uint16 len ){m_hdr3SizeBlocks = len;}

        // stores a tag into the tagheader
        void addTagField( TagField & tagfield );

        // Accessors
        void setEventDescriptor( EventDescriptor evd ){m_eventdesc = evd;}
        EventDescriptor & getEventDescriptor( ){return m_eventdesc;}

        void clear();

        void copyFrom( const TagHeader &other );

        // Container for the array of T-L-V sets. See Table 10 of GDF standard.
        std::map<int,TagField> m_tags;

    private: 
        // Minimum dumber of bytes to store the Header3 info (without padding) .
        uint16 m_header3ByteSizeUnpadded;
        // Number of 256 byte blocks required to store Header3.
        uint16 m_hdr3SizeBlocks;
        //std::map<int,boost::shared_ptr<TagField>> m_tags;
        EventDescriptor m_eventdesc; 
    };

}

#endif
