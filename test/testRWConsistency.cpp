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

#include "config-tests.h"

#include <GDF/Writer.h>
#include <GDF/Reader.h>

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

const string testfile = "test.gdf.tmp";
const string reffile = string(GDF_SOURCE_ROOT)+"/sampledata/MI128.gdf";

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

bool fcompare( std::string fileA, std::string fileB )
{
    std::ifstream f1( fileA.c_str(), std::ios_base::in | std::ios_base::binary );
    std::ifstream f2( fileB.c_str(), std::ios_base::in | std::ios_base::binary );

    size_t ofs = 0;
    while( !( f1.eof() || f2.eof() ) )
    {
        unsigned char a, b;
        f1 >> a;
        f2 >> b;

        cout << ofs << " : " << (int)a << " ... " << (int)b << endl;

        if( a != b )
            return false;

        ofs++;

    }
    return true;
}

int main( )
{
    try
    {
        cout << "Creating Writer instance." << endl;
        gdf::Writer w;

        cout << "Creating Reader instance." << endl;
        gdf::Reader r;

        r.enableCache( false );

        cout << "Opening '" << reffile << "' for reading." << endl;
        r.open( reffile );

        cout << "Copying Header information." << endl;
        w.getMainHeader( ).copyFrom( r.getMainHeader_readonly() );
        w.getHeaderAccess().setRecordDuration( r.getMainHeader_readonly().get_datarecord_duration( 0 ), r.getMainHeader_readonly().get_datarecord_duration( 1 ) );
        for( size_t m=0; m<w.getMainHeader_readonly().get_num_signals(); m++ )
        {
            w.createSignal( m, true );
            w.getSignalHeader( m ).copyFrom( r.getSignalHeader_readonly( m ) );
        }

        w.setEventMode( r.getEventHeader()->getMode() );
        w.setEventSamplingRate( r.getEventHeader()->getSamplingRate() );

        cout << "Opening '" << testfile << "' for writing." << endl;
        w.open( testfile, gdf::writer_ev_memory | gdf::writer_overwrite );

        cout << "Copying data .... ";
        size_t num_recs = boost::numeric_cast<size_t>( r.getMainHeader_readonly( ).get_num_datarecords( ) );

        for( size_t n=0; n<num_recs; n++ )
        {
            gdf::Record *rec = w.acquireRecord( );
            r.readRecord( n, rec );
            w.addRecord( rec );
        }
        cout << "OK" << endl;

        cout << "Copying events .... ";
        gdf::EventHeader* ev_header = r.getEventHeader();
        unsigned int num_events = ev_header->getNumEvents();switch( ev_header->getMode() )
        {
        default: throw(std::runtime_error("ERROR -- Invalid event mode!"));
        case 1: {
                gdf::Mode1Event ev;
                for(unsigned int m = 0; m < num_events; m++)
                {
                  ev_header->getEvent(m, ev);
                  w.addEvent(ev);
                }
            } break;
        case 3: {
                gdf::Mode3Event ev;
                for(unsigned int m = 0; m < num_events; m++)
                {
                  ev_header->getEvent(m, ev);
                  w.addEvent(ev);
                }
            } break;
        }
        cout << "OK" << endl;

        cout << "Comparing files .... ";
        if( !fcompare( reffile, testfile ) )
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
