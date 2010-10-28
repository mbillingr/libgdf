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

#ifndef __TOOLS_H_INCLUDED__
#define __TOOLS_H_INCLUDED__

#include <vector>

namespace gdf
{

    template<class T>
    T gcd( T a, T b)
    {
        if( a == 0 )
            return b;
        if( b == 0 )
            return a;
        while( 1 )
        {
            a = a % b;
            if( a == 0 )
                return b;

            b = b % a;
            if( b == 0 )
                return a;
        }
    }

    template<class T>
    T gcd( const std::vector<T> &v, size_t p, size_t r )
    {
        if( v.size() == 0 )
            throw exception::empty_container( "calculating GCD" );

        if( p>=v.size() || r>= v.size() )
            throw exception::index_out_of_range( "calculating GCD" );

        if( r == p+1 )
            return gcd( v[p], v[r] );

        if( r == p )
            return v[p];

        int q = (p+r)/2;
        return gcd( gcd( v, p, q ), gcd( v, q+1, r ) );
    }

    template<class T>
    T gcd( const std::vector<T> &v )
    {
        return gcd( v, size_t(0), v.size()-1 );
    }

    template<class T>
    T max( const std::vector<T> &v )
    {
        T m = v[0];
        for( size_t i=1; i<v.size(); i++ )
            m = std::max( m, v[i] );
        return m;
    }

    template<class T>
    T min( const std::vector<T> &v )
    {
        T m = v[0];
        for( size_t i=1; i<v.size(); i++ )
            m = std::min( m, v[i] );
        return m;
    }

    template<class T>
    T sum( const std::vector<T> &v )
    {
        T m = v[0];
        for( size_t i=1; i<v.size(); i++ )
            m = m + v[i];
        return m;
    }

}

#endif // TOOLS_H
