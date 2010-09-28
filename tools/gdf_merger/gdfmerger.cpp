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

#include "gdfmerger.h"

#include <exception>
#include <boost/filesystem.hpp>

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

namespace fs  = boost::filesystem;

//---------------------------------------------------------------------------------------

gdfMerger::gdfMerger(const vector<string>& inputs, const string& output)
  : input_files_(inputs), output_file_(output), event_offset_(0)
{
  for(unsigned int n = 0; n < inputs.size(); n++)
    if(!fs::exists(inputs[n]) )
      throw(std::invalid_argument("Error - file not found: " + inputs[n]));
}

//---------------------------------------------------------------------------------------

gdfMerger::~gdfMerger()
{

}

//---------------------------------------------------------------------------------------

void gdfMerger::merge()
{

  reader_.open(input_files_[0]);

  writer_.getMainHeader( ).copyFrom( reader_.getMainHeader_readonly() );
  for( size_t m = 0; m < writer_.getMainHeader_readonly().get_num_signals(); m++ )
  {
      writer_.createSignal( m, true );
      writer_.getSignalHeader( m ).copyFrom( reader_.getSignalHeader_readonly( m ) );
  }

  gdf::EventHeader* ev_header = reader_.getEventHeader();
  gdf::float32 fs_events = ev_header->getSamplingRate();

  writer_.setEventMode( ev_header->getMode() );
  writer_.setEventSamplingRate(fs_events);

  reader_.close( );

  writer_.setMaxFullRecords( 0 );
  writer_.open( output_file_, gdf::writer_ev_memory | gdf::writer_overwrite );

  for(unsigned int n = 0; n < input_files_.size(); n++)
  {
    reader_.enableCache( false );
    reader_.open(input_files_[n]);

    cout << "  -- merging: " << input_files_[n];
    cout << "     Warning: No header checks performed yet!" << endl;

    size_t num_recs = reader_.getMainHeader_readonly( ).get_num_datarecords( );

    for( size_t r = 0; r< num_recs; r++ )
    //for( size_t r = 0; r< 1; r++ )
    {
        //std::cout << r << std::endl;
        //gdf::Record *rec = reader_.getRecordPtr( r );
        //writer_.writeRecordDirect( rec );

        gdf::Record *rec = writer_.acquireRecord( );
        reader_.readRecord( r, rec );
        writer_.addRecord( rec );

        /*for( size_t c = 0; c < reader_.getMainHeader_readonly().get_num_signals(); c++ )
        {
            for( size_t n = 0; n < reader_.getSignalHeader_readonly(c).get_samples_per_record(); n++ )
            {
                double v = rec->getChannel(c)->getSamplePhys(n);
                writer_.addSamplePhys( c, v );
            }
        }*/
    }

    ev_header = reader_.getEventHeader();
    unsigned int num_events = ev_header->getNumEvents();

    if(ev_header->getSamplingRate() != fs_events)
      throw(std::runtime_error("ERROR -- Event sampling rates differ!"));

    switch( ev_header->getMode() )
    {
    default: throw(std::runtime_error("ERROR -- Invalid event mode!"));
    case 1: {
            gdf::Mode1Event ev;
            for(unsigned int m = 0; m < num_events; m++)
            {
              ev_header->getEvent(m, ev);
              ev.position += event_offset_;
              writer_.addEvent(ev);
            }
        } break;
    case 3: {
            gdf::Mode3Event ev;
            for(unsigned int m = 0; m < num_events; m++)
            {
              ev_header->getEvent(m, ev);
              ev.position += event_offset_;
              writer_.addEvent(ev);
            }
        } break;
    }

    double  recdur_num = boost::numeric_cast<double>( reader_.getMainHeader_readonly().get_datarecord_duration( 0 ) );
    double  recdur_den = boost::numeric_cast<double>( reader_.getMainHeader_readonly().get_datarecord_duration( 1 ) );
    double numrec = boost::numeric_cast<double>( reader_.getMainHeader_readonly().get_num_datarecords( ) );

    event_offset_ += boost::numeric_cast<gdf::uint32>( numrec * recdur_num * (fs_events / recdur_den) );
    reader_.close();
  }

  writer_.close();
}

//---------------------------------------------------------------------------------------
