// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Martin Billinger

#ifndef __STRING_H_INCLUDED__
#define __STRING_H_INCLUDED__

#include <iostream>

class String
{
public:
    String( )
    {
        len = 0;
        str = new char[1];
        str[0] = 0;
    }

    String( const char *ch )
    {
        len = 0;

        while( ch[len] != 0 )
        {
            len++;
        }

        str = new char[len+1];

        for( size_t i=0; i<len; i++ )
            str[i] = ch[i];

        str[len] = 0;
    }

    String( const char *ch, size_t length )
    {
        len = length;

        str = new char[len+1];

        for( size_t i=0; i<len; i++ )
            str[i] = ch[i];

        str[len] = 0;
    }

    String( const String &src )
    {
        len = src.len;
        str = new char[len+1];
        for( size_t i=0; i<len; i++ )
            str[i] = src.str[i];
        str[len] = 0;
    }

    String( const String &a, const String &b )
    {
        len = a.len + b.len;
        str = new char[len+1];
        for( size_t i=0; i<a.len; i++ )
            str[i] = a.str[i];
        for( size_t i=0; i<b.len; i++ )
            str[a.len+i] = b.str[i];
        str[len] = 0;
    }

    ~String( )
    {
        delete[] str;
    }

    size_t find( char ch ) const
    {
        for( size_t i=0; i<len; i++ )
            if( str[i] == ch )
                return i;
        return len;
    }

    inline char &operator[]( size_t idx ) { return str[idx]; }
    inline const char &operator[]( size_t idx ) const { return str[idx]; }

    inline const char *c_str( ) const { return str; }
    inline size_t length( ) const { return len; }

    bool operator<( const String &other ) const
    {
        size_t n = 0;
        while( n < len && n < other.len )
        {
            if( str[n] < other[n] )
                return true;
            else if( str[n] > other[n] )
                return false;
            n++;
        }
        return len < other.len;
    }

    String substr( size_t start, size_t length )
    {
        return String( &str[start], length );
    }

private:
    char *str;
    size_t len;
};

String operator+( const String &a, const String &b )
{
    return String( a, b );
}

String operator+( const char c[], const String &s )
{
    return String(c) + s;
}

std::ostream& operator<<( std::ostream& out, const String &s )
{
    return out << s.c_str();
}

#endif // STRING_H
