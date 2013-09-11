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

#include "GDF/Exceptions.h"
#include "GDF/TagHeader.h"
#include "GDF/EventDescriptor.h"
#include "GDF/GDFHeaderAccess.h"
//#include <algorithm>
//#include <iostream>
#include<iterator>
#include <boost/lexical_cast.hpp>
//#include <boost/shared_ptr.hpp>

namespace gdf
{
    //===================================================================================================
    //===================================================================================================

    TagHeader::TagHeader( )
        : m_header3ByteSizeUnpadded(0), m_hdr3SizeBlocks(0)
    {
    }

    //===================================================================================================
    //===================================================================================================

    TagHeader::~TagHeader( )
    {
    }

    //===================================================================================================
    //===================================================================================================

    void TagHeader::fromStream( std::istream &stream , gdf::GDFHeaderAccess& hdr)
    {
        unsigned char tag = 1;
        size_t header3unpaddedsize = 0;
        while( tag != 0 )
        {
            //std::streampos pos = stream.tellg();
            gdf::TagField tagfield(0);
            tagfield.fromStream(stream); 
            tag = tagfield.getTagNumber();
            switch( tag )
            {
            case 0:
                // Zero tag value indicates end of Header 3. See first row of Table 10 in GDF standard.
                header3unpaddedsize += 1;
                break;
            case 1:
                header3unpaddedsize += tagfield.getLength();
                this->addTagField( tagfield );
                break;
            default:
                throw exception::feature_not_implemented("Only tag==1 is supported in this build");
                break;
            }
        }
        // consume the padding bytes that make Header 3 a multiple of 256 bytes
        size_t ns = hdr.getMainHeader_readonly().get_num_signals( );
        size_t nhdrblocks = hdr.getMainHeader_readonly().get_header_length( );
        int padbytecount = (nhdrblocks - (ns+1))*256 - header3unpaddedsize;
        if( padbytecount < 0 )
        {
            throw exception::serialization_error("problem reading GDF Header 3");
        }
        stream.seekg(std::streamoff(padbytecount), std::ios_base::cur);
    }

    //===================================================================================================
    //===================================================================================================

    /// Serializer
    void TagHeader::toStream( std::ostream &stream ) const
    {
        if( m_hdr3SizeBlocks > 0) 
        {
            int bytesRemaining = 256*m_hdr3SizeBlocks;
            //std::map<int,boost::shared_ptr<TagField>>::const_iterator it = m_tags.begin();
            std::map<int,TagField>::const_iterator it = m_tags.begin();
            for( ; it != m_tags.end(); ++it) {
                // copy each tag to the stream
                const TagField atagf = (*it).second;
                atagf.toStream(stream);
                bytesRemaining -= atagf.getLength();
            }
            // Send a single zero after the tags to indicate the end of the tag list.
            // See GDF 2.20 Table 10 first row regarding Tag == 0.
            unsigned char terminatingTag = 0;
            stream << terminatingTag; 
            bytesRemaining --;
            if( bytesRemaining < 0 ) {
                throw exception::serialization_error("problem writing GDF Header 3");
            }
            // Fill out the stream so that header 3 is a multiple of 256.
            unsigned char padByte = 0;
            while( bytesRemaining > 0 )
            {
                stream << padByte; 
                bytesRemaining --;
            }
        }
    }

    //===================================================================================================
    //===================================================================================================

    void TagHeader::clear()
    {
        m_tags.clear();
        m_header3ByteSizeUnpadded = 0;
        m_hdr3SizeBlocks = 0;
        m_eventdesc.clear();
    }

    //===================================================================================================
    //===================================================================================================

    void TagHeader::finalize( )
    {
        /// Copy each specialized Tag field to its corresponsing TagField instance.
        // According to GDF 220 Table 10:
        // Tag = 0, there is no instance.
        // Tag == 1, user defined 
        if( this->getEventDescriptor().getNumUserDesc() > 0 )
            this->m_tags[1] = this->getEventDescriptor().toTagField();
        // Tag == 2..8 are defined in GDF but not yet implemented here

        // update m_hdr3SizeBlocks based on changes to supporting arrays
        this->setLength();
    }

    //===================================================================================================
    //===================================================================================================

    void TagHeader::setLength( ) {
        size_t bytecount = 0; 
        std::map<int,TagField>::iterator it = m_tags.begin();
        for( ; it != m_tags.end(); ++it) {
            bytecount += (*it).second.getLength();
        }
        if( bytecount > 0 ) 
        {
            bytecount += 1; // Allow one character for the terminating zero of the T-L-V table.
        } 
        // GDF calculates header sizes in terms of 256-byte blocks.
        size_t blockcount = (bytecount + 255)/256;
        this->m_header3ByteSizeUnpadded = bytecount;
        this->m_hdr3SizeBlocks = blockcount;
    }

    //===================================================================================================
    //===================================================================================================

    void TagHeader::addTagField( TagField & tagfield )
    {
        m_tags[tagfield.getTagNumber()] = tagfield;
    }


    //===================================================================================================
    //===================================================================================================

    void TagHeader::copyFrom( const TagHeader &other )
    {
        (*this) = other;
    }


    //===================================================================================================
    //===================================================================================================

    TagField::TagField( uint8 tag=0 ) : m_tag(tag)
    {
        ;
    }

    //===================================================================================================
    //===================================================================================================

    TagField::~TagField( )
    {
        ;
    }

    //===================================================================================================
    //===================================================================================================

    /// Serializer
    void TagField::toStream( std::ostream &stream ) const
    {
        std::copy(m_value.begin(),m_value.end(),std::ostream_iterator<unsigned char>(stream));
    }

    //===================================================================================================
    //===================================================================================================

    /// Deserializer
    void TagField::fromStream( std::istream &stream )
    {
        m_value.clear();
        stream.read( reinterpret_cast<char*>(&m_tag), 1 );
        if( m_tag == 0 ) 
        {
            // Zero tag value indicates end of Header 3. See first row of Table 10 in GDF standard.
            return;
        }
        uint8 tmp[3];
        stream.read( reinterpret_cast<char*>(tmp), 3 );
        uint32 taglength = tmp[0] + tmp[1]*256 + tmp[2]*65536; // length of the value-only field
        m_value.resize( taglength + 4 ); // +4 to include the leading header 
        // copy header elements
        m_value[0] = m_tag;
        m_value[1] = tmp[0];
        m_value[2] = tmp[1];
        m_value[3] = tmp[2];
        // read the value field from stream
        for( size_t i=0; i<taglength; i++ )
            stream.read( reinterpret_cast<char*>(&m_value[i+4]), 1 );
    }
}