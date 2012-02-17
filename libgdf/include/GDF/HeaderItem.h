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

#ifndef __HEADERITEM_H_INCLUDED__
#define __HEADERITEM_H_INCLUDED__

#include "GDF/Types.h"
#include "GDF/Exceptions.h"
#include <iostream>
#include <stddef.h>
#include <map>
#include <algorithm>

#define GDF_DECLARE_HEADERITEM( NAME, TYPE, POS ) \
        public: \
        const TYPE &get_##NAME( ) const { return NAME.item; }; \
        void set_##NAME( const TYPE &val ) { NAME.item = val; }; \
        private: \
        HeaderItem<TYPE,POS> NAME;

#define GDF_DECLARE_HEADERITEM_PRIVATE( NAME, TYPE, POS ) \
        public: \
        const TYPE &get_##NAME( ) const { return NAME.item; }; \
        private: \
        void set_##NAME( const TYPE &val ) { NAME.item = val; }; \
        HeaderItem<TYPE,POS> NAME;

#define GDF_DECLARE_HEADERARRAY( NAME, TYPE, POS, LEN ) \
        public: \
        const TYPE *get_##NAME( ) const { return NAME.item; } \
        TYPE *get_##NAME( ) { return NAME.item; } \
        TYPE get_##NAME( size_t i ) const { return NAME.item[i]; } \
        void set_##NAME( const TYPE val, size_t i ) { NAME.item[i] = val; } \
        void set_##NAME( const TYPE *val, const size_t len=LEN ) { for( size_t i=0; i<std::min(len,NAME.len); i++ ) NAME.item[i] = val[i]; }; \
        private: \
        HeaderArray<TYPE,POS,LEN> NAME;

#define GDF_DECLARE_HEADERSTRING( NAME, POS, LEN ) \
        public: \
        const std::string get_##NAME( ) const { return std::string( NAME.item, LEN ); }; \
        void set_##NAME( const std::string str ) { for( size_t i=0; i<std::min(str.size(),NAME.len); i++ ) NAME.item[i] = str[i]; }; \
        size_t getLength_##NAME( ) const { return LEN; } \
        private: \
        HeaderArray<char,POS,LEN> NAME;

#define GDF_DECLARE_RESERVED( NAME, POS, LEN ) \
        public: \
        const char *get_##NAME( ) const { return NAME.item; }; \
        private: \
        void set_##NAME( const char *val, const size_t len ) { for( size_t i=0; i<len; i++ ) NAME.item[i] = val[i]; }; \
        void set_##NAME( const char *val ) { for( size_t i=0; i<NAME.len; i++ ) NAME.item[i] = val[i]; }; \
        HeaderArray<char,POS,LEN> NAME;

#define GDF_BEGIN_HEADERMAP( ) \
        public: \
            const std::map< size_t, HeaderRef > &getposmap() const {return posmap;} \
            const std::map< std::string, HeaderRef > &getstrmap() const {return strmap;} \
        private:    \
            std::map< size_t, HeaderRef > posmap; \
            std::map< std::string, HeaderRef > strmap; \
            void initHeaderMap( ) {

#define GDF_END_HEADERMAP( ) \
            }

#define GDF_ASSIGN_HEADERARRAY( NAME ) \
            posmap[NAME.pos] = HeaderRef( #NAME, NAME.item, NAME.pos, sizeof(NAME.item) ); \
            strmap[#NAME] = HeaderRef( #NAME, NAME.item, NAME.pos, sizeof(NAME.item) );

#define GDF_ASSIGN_RESERVED( NAME ) \
            posmap[NAME.pos] = HeaderRef( #NAME, NAME.item, NAME.pos, sizeof(NAME.item) ); \
            strmap[#NAME] = HeaderRef( #NAME, NAME.item, NAME.pos, sizeof(NAME.item) );

#define GDF_ASSIGN_HEADERITEM( NAME ) \
            posmap[NAME.pos] = HeaderRef( #NAME, &NAME.item, NAME.pos, sizeof(NAME.item) ); \
            strmap[#NAME] = HeaderRef( #NAME, &NAME.item, NAME.pos, sizeof(NAME.item) );

namespace gdf {
    struct HeaderItemBase
    {
        virtual void tostream( std::ostream &out ) const = 0;
    };

    /// Store header variable of type T at offset P
    template<typename T, size_t P>
    struct HeaderItem : public HeaderItemBase
    {
        HeaderItem( ) : item(), pos(P) { }
        //void tostream( std::ostream & out ) const { out.write( reinterpret_cast<const char*>(&item), sizeof(item) ); }
        //void fromstream( std::istream &in ) { in.read( reinterpret_cast<char*>(&item), sizeof(item) ); }
        void tostream( std::ostream & out ) const { writeLittleEndian( out, item ); }
        void fromstream( std::istream &in ) { readLittleEndian( in, item ); }
        T item;
        size_t pos;
    };

    /// Store array-style header variable of length L and type T at offset P
    template<typename T, size_t P, size_t L>
    struct HeaderArray : public HeaderItemBase
    {
        HeaderArray( ) : pos(P), len(L) { }
        T &operator[]( size_t idx ) { return item[idx]; }
        const T &operator[]( size_t idx ) const { return item[idx]; }
        void tostream( std::ostream & out ) const { out.write( reinterpret_cast<const char*>(item), sizeof(item) ); }
        void fromstream( std::istream &in ) { in.read( reinterpret_cast<char*>(item), sizeof(item) ); }
        T item[L];
        size_t pos, len;
    };

    struct HeaderRef
    {
        HeaderRef( std::string n="", void *r = NULL, size_t o = 0, size_t l = 0) : name(n), ref(r), ofs(o), len(l) { }
        std::string name;
        void *ref;
        size_t ofs;
        size_t len;
    };
}

#endif
