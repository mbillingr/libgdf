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
const string reffile0 = string(GDF_SOURCE_ROOT)+"/sampledata/MI128.gdf";
// File NEQSuint32Ch678 uses Non-Equidistant sampling.
// Channel 6, channel7 and channel 8 use NEQS. NEQS values are stored as Uint32.
const string neqsfile = string(GDF_SOURCE_ROOT)+"/sampledata/NEQSuint32Ch678.GDF";

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
        unsigned char a, b;
        f1 >> a;
        f2 >> b;


        if( a != b )
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
    infilelist.push_back(reffile0);
    infilelist.push_back(neqsfile);
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

                /// NEQS demonstration of getSparseSamples
                // For the NEQS sample file, what follows is a simple
                // demonstration of how to get the samples.
                if (reffile.compare(neqsfile)==0) {
                    /// Demonstrate getting all the events from channel 6.
                    int index_of_a_sparse_channel = 6;
                    std::vector<gdf::uint32> ch6samples = ev_header->getSparseSamples (index_of_a_sparse_channel);
                    // If the channel has enough events, let's look at the third event (for example).
                    size_t event_index_to_get = 3;
                    if (ch6samples.size() > event_index_to_get) {
                        // Extract a specific sample from the event table into a local event object, ev.
                        ev_header->getEvent(ch6samples[event_index_to_get], ev);
                        // Convert ev to time and physical values.
                        r.eventToSample(sample_time_sec, sample_physical_value, ev);
                        // sample_time_sec = time of 3rd sample of channel 6
                        // sample_physical_value = value of 3rd sample of channel 6
                    }
                }
                /// end of getSparseSamples demonstration

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
