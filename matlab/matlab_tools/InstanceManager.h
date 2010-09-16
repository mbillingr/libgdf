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

#ifndef __INSTANCEMANAGER_H_INCLUDED__
#define __INSTANCEMANAGER_H_INCLUDED__

#include <stdlib.h>
#include <stdexcept>
#include <map>

template<class C>
class InstanceManager
{
public:
    InstanceManager( )
    {
        instances.clear( );
    }

    virtual ~InstanceManager( )
    {
        while( instances.size() > 0 )
        {
            delete instances.begin()->second;
            instances.erase( instances.begin() );
        }
    }

    bool checkHandle( size_t id )
    {
        if( instances.find(id) == instances.end() )
            return false;

        return true;
    }

    C *get( size_t i )
    {
        if( !checkHandle(i) )
            throw std::invalid_argument( "Invalid Handle." );
        return instances[i];
    }

    size_t newInstance( )
    {
        size_t handle = rand( );
        while( instances.find(handle) != instances.end() )
        {
            handle = rand( );
        }
        instances[handle] = new C( );  // accessing creates a new element in the map
        return handle;
    }

    void remove( size_t id )
    {
        instances.erase( id );
    }

    void clear( )
    {
        instances.clear( );
    }

private:
    std::map<size_t,C*> instances;
};

#endif // INSTANCEMANAGER_H
