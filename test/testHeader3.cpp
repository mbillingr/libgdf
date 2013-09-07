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
#include <GDF/TagHeader.h>
#include <GDF/EventDescriptor.h>
//#include <GDF/TagFields.h>

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

const string testfile = "test.gdf.tmp";

const string annotfile = string(GDF_SOURCE_ROOT)+"/sampledata/Header3Tag1.gdf";
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
    infilelist.push_back(annotfile);
    
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
            gdf::TagHeader input_file_tag_header = r.getHeaderAccess_readonly().getTagHeader_readonly();
            w.getHeaderAccess().getTagHeader().copyFrom( input_file_tag_header ); 
            
            if( 0 ) {
                // Test fails on this branch (because new events are put in the 
                // file so input and output files are not the same), however
                // it is an instructive code branch to walk through.

                /// Demonstration of event description storage.
                //
                /// Workflow 1: Create event description tables from an aritrary collection of strings.
                // Each new string encountered is assigned a unique eventType beginning with eventType==1.
                gdf::EventDescriptor free_text_annotator;
                gdf::uint16 eventType;
                //
                // Phase 1 Option 1.  Create event description tables from an aritrary collection of strings.
                // Each new string encountered is assigned a unique eventType beginning with eventType==1.
                free_text_annotator.clear();
                eventType = free_text_annotator.addUserSpecificDesc("yellow"); // 1
                eventType = free_text_annotator.addUserSpecificDesc("green");  // 2
                eventType = free_text_annotator.addUserSpecificDesc("blue");   // 3
                eventType = free_text_annotator.addUserSpecificDesc("yellow"); // 1. If same string is encountered, the existing eventType is returned.
                free_text_annotator.loadEventDescriptions(eventcodefile);
                //
                // Phase X. At any time after Phase 1, use the table to get Event TYP's or event description strings
                eventType = free_text_annotator.getUserDescEventType("blue");   // 3
                // eventType = free_text_annotator.getUserDescEventType("turquoise");   // error, turquoise is not in the table.
                std::string a_description = free_text_annotator.getEventDesc(2); // user-specified descriptions
                a_description = free_text_annotator.getEventDesc(257);           // standard description
                a_description = free_text_annotator.getEventDesc(33);           // unknown user-specified description
                a_description = free_text_annotator.getEventDesc(4000);           // unknown description
                //
                // Phase 1 Option 2: Associate specific event codes to a collection of strings.
                free_text_annotator.clear();
                free_text_annotator.loadEventDescriptions(eventcodefile);
                free_text_annotator.setEventDesc(33,"yellow");
                // eventType = free_text_annotator.setEventDesc(34,"yellow"); // Unpredicatable result, don't do this.
                free_text_annotator.setEventDesc(4,"green");
                free_text_annotator.setEventDesc(17,"blue");
                free_text_annotator.copyEventDescToUserDesc();
                //
                // Phase X. At any time after Phase 1, use the table to get Event TYP's or event description strings
                eventType = free_text_annotator.getUserDescEventType("blue");  
                a_description = free_text_annotator.getEventDesc(4); 
                a_description = free_text_annotator.getEventDesc(257);         
                a_description = free_text_annotator.getEventDesc(33);
                //
                // Phase 2. Store the tables into header before opening file for write.
                w.getHeaderAccess().getTagHeader().setEventDescriptor(free_text_annotator);
                w.getHeaderAccess().getTagHeader().finalize();  
                //
                // Phase 3. Open file for write.
                cout << "Opening '" << testfile << "' for annotation demonstration." << endl;
                w.open( testfile, gdf::writer_ev_memory | gdf::writer_overwrite );
                //
                // Phase 4. Create events at specific times in the file. Use existing strings.
                switch( w.getHeaderAccess().getEventHeader().getMode() )
                {
                case 1: {
                    gdf::Mode1Event ev;
                    w.makeFreeTextEvent( 3.14159, "green", free_text_annotator, ev ); 
                    w.addEvent( ev ); 
                    w.makeFreeTextEvent( 43.9, "yellow", free_text_annotator, ev );
                    w.addEvent( ev );
                    w.makeFreeTextEvent( 19.1, "green", free_text_annotator, ev );
                    w.addEvent( ev );
                        } break;
                case 3: {
                    gdf::Mode3Event ev;
                    w.makeFreeTextEvent( 3.14159, "green", free_text_annotator, ev );
                    ev.channel = 1; // for example, the text could be associated to any specific channel
                    w.addEvent( ev );
                    w.makeFreeTextEvent( 43.9, "yellow", free_text_annotator, ev );
                    ev.channel = 2;
                    w.addEvent( ev );
                    w.makeFreeTextEvent( 19.1, "green", free_text_annotator, ev );
                    ev.channel = 3;
                    w.addEvent( ev );
                        } break;
                default:
                    break;
                }
                // Phase 5. Continue with other file writing tasks, including w.makeFreeTextEvent/w.addEvent.
                // Events are buffered and written at w.close().
                // Phase 6. w.close().
            } 
            else 
            {
                cout << "Opening '" << testfile << "' for writing." << endl;
                w.open( testfile, gdf::writer_ev_memory | gdf::writer_overwrite );
            }

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
            gdf::EventDescriptor & ev_descriptor_from_input = input_file_tag_header.getEventDescriptor();
            switch( ev_header->getMode() )
            {
            default: throw(std::runtime_error("ERROR -- Invalid event mode!"));
            case 1: {
                gdf::Mode1Event ev, ev_out;
                for(unsigned int m = 0; m < num_events; m++)
                {
                    ev_header->getEvent(m, ev);
                    if( ev.type == 0 ) 
                    {
                        throw(std::runtime_error("ERROR -- event TYP 0"));
                    } 
                    else if( ev.type < 256 ) 
                    { 
                        // case: user-specified event description

                        // Here we could do simply w.addEvent(ev); and the event 
                        // with user-specified text would be written successfully to output.
                        // However, for the sake of demonstration and testing, we will unpack 
                        // the event here and re-create it before storing it.

                        // unpacking steps:
                        std::string description_of_this_event = ev_descriptor_from_input.getEventDesc(ev.type);
                        double ev_time_sec = r.getEventHeader()->posToSec( ev.position );
                        cout << description_of_this_event << endl;

                        // recreating the free text event from time and string:
                        w.makeFreeTextEvent(ev_time_sec, description_of_this_event, ev_descriptor_from_input, ev_out);
                        w.addEvent(ev_out);
                    } 
                    else 
                    { 
                        // case: standard event description from eventcodes.txt
                        std::string description_of_this_event = ev_descriptor_from_input.getEventDesc(ev.type);
                        cout << description_of_this_event << endl;
                        w.addEvent(ev);
                    }
                }
                    } break;
            case 3: {
                gdf::Mode3Event ev;

                // Copy all event from source file to target file.
                // Mode 1 and 3 events are copied.
                for(unsigned int mm = 0; mm < num_events; mm++)
                {
                    ev_header->getEvent(mm, ev);
                    w.addEvent(ev);
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
