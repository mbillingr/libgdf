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

#include <GDF/Writer.h>

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

const string testfile = "test.gdf.tmp";
const size_t gdf_emptysize = 264;   // 256 bytes main header, 8 bytes event header

bool fexist( std::string filename )
{
    std::ifstream f( filename.c_str(), std::ios_base::in );
    if( f.fail() )
        return false;
    f.close( );
    return true;
}

size_t fsize( std::string filename )
{
    struct stat filestatus;
    stat( filename.c_str(), &filestatus );
    return filestatus.st_size;
}

int main( )
{
    try
    {
        if( fexist( testfile ) )
        {
            cout << "Removing existing " << testfile << endl;
            remove( testfile.c_str() );
        }

        cout << "Creating Writer instance." << endl;
        gdf::Writer w;

        w.setEventSamplingRate( 100 );

        cout << "Opening file for writing." << endl;
        try {
            w.open( testfile );
        } catch( gdf::exception::header_issues &e )
        {
            if( e.num_errors() > 0 ) throw;
            cout << "Header Issues: " << endl << e.what( ) << endl;
        }

        cout << "Closing file." << endl;
        w.close( );

        cout << "Checking if " << testfile << " exists .... ";
        if( !fexist( testfile ) )
        {
            cout << "Failed." << endl;
            return 1;
        }
        cout << "OK" << endl;

        cout << "Checking file size .... ";
        if( fsize( testfile ) != gdf_emptysize )
        {
            cout << "Failed." << endl;
            return 1;
        }
        cout << "OK" << endl;

        cout << "Removing " << testfile << endl;
        remove( testfile.c_str() );

        return 0;   // test succeeded
    }
    catch( std::exception &e )
    {
        std::cout << "Caught Exception: " << e.what( ) << endl;
    }
    catch( ... )
    {
        std::cout << "Caught Unknown Exception." << endl;
    }

    return 1;   // test failed
}
