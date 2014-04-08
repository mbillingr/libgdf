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


#include "config-tests.h"

#include <GDF/Writer.h>
#include <GDF/Reader.h>
#include <GDF/TagHeader.h>
#include <GDF/EventDescriptor.h>

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

const string testfile = "test.gdf.tmp";
const string reffile0 = string(GDF_SOURCE_ROOT)+"/sampledata/MI128.gdf";
const string annotfile = string(GDF_SOURCE_ROOT)+"/sampledata/Header3Tag1.gdf";
const string alltypesfile = string(GDF_SOURCE_ROOT)+"/sampledata/alltypes.gdf";
const string eventcodefile = string(GDF_SOURCE_ROOT)+"/libgdf/eventcodes.txt";

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

    bool state = true;

    size_t ofs = 0;
    while( !( f1.eof() || f2.eof() ) )
    {
        signed char a, b;
        f1 >> a;
        f2 >> b;


        if( abs(a-b) > 1 )  // tolerate a difference of 1 due to rounding errors in digitial -> physical -> digital conversion
        {
            cout << ofs << " : " << (int)a << " ... " << (int)b << endl;
            state = false;
        }

        ofs++;

    }
    return state;
}

int main( )
{
    std::vector<string> infilelist; // a list of files on which to run tests
    infilelist.push_back(annotfile);
    infilelist.push_back(alltypesfile);
    infilelist.push_back(reffile0);

    string reffile;


    try
    {
        for(size_t file_count=0; file_count < infilelist.size(); file_count++)
        {
            reffile = infilelist[file_count]; // file to be tested in this loop iteration

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
            // Copy GDF header 3 including user-specific event description table
            gdf::TagHeader ath = r.getHeaderAccess_readonly().getTagHeader_readonly();
            w.getHeaderAccess().getTagHeader().copyFrom( ath );

            cout << "Opening '" << testfile << "' for writing." << endl;
            w.open( testfile, gdf::writer_ev_memory | gdf::writer_overwrite );

            cout << "Copying data .... ";

            //size_t num_recs = boost::numeric_cast<size_t>( r.getMainHeader_readonly( ).get_num_datarecords( ) );
            //for( size_t n=0; n<num_recs; n++ )
            //{
            //    gdf::Record *rec = w.acquireRecord( );
            //    r.readRecord( n, rec );
            //    w.addRecord( rec );
            //}

            std::vector< std::vector< double > > buffer;
            r.getSignals(buffer);

            for( size_t ch=0; ch<buffer.size(); ch++)
            {
                cout << ch << endl;
                w.blitSamplesPhys(ch, buffer[ch]);
            }

            cout << "OK" << endl;

            cout << "Copying events .... ";
            gdf::EventHeader* ev_header = r.getEventHeader();
            unsigned int num_events = ev_header->getNumEvents();
            switch( ev_header->getMode() )
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
                double sample_physical_value;
                double sample_time_sec;

                // Copy all event from source file to target file.
                // Mode 1 and 3 events are copied.
                // Sparse samples are extracted to (time,phys) then stored again.
                for(unsigned int mm = 0; mm < num_events; mm++)
                {
                    ev_header->getEvent(mm, ev);
                    if( ev.type != 0x7fff ) {
                        w.addEvent(ev);
                    } else {
                        r.eventToSample(sample_time_sec, sample_physical_value, ev);
                        // At this point we have successfully decoded a sparse sample
                        //     (sample_time_sec, sample_physical_value)    .
                        w.sampleToEvent( sample_time_sec, sample_physical_value, ev.channel, ev );
                        // At this point we have successfully encoded a sparse sample into an event.
                        // Now write the event to file.
                        w.addEvent( ev );
                    }
                }
                    } break;
            }

            cout << "OK" << endl;

            w.close( );
            cout << "Comparing files .... ";
            if( !fcompare( reffile, testfile ) )
            {
                cout << "Failed." << endl;
                return 1;
            }
            cout << "OK" << endl;

            cout << "Removing " << testfile << endl << endl;
            remove( testfile.c_str() );
        }
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
