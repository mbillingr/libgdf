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

#include <GDF/Reader.h>
#include <GDF/Writer.h>
#include <GDF/Modifier.h>
#include <GDF/tools.h>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using boost::lexical_cast;

void test_guillaume( );
void test_simple( );
void test_events( );
void test_buffer( size_t M, size_t T );
void test_load( );
void test_modify( );

int main( )
{
    test_guillaume( );
    //test_buffer( 100, 100 );
    //test_simple( );
    /*test_load( );
    test_modify( );
    test_load( );
    test_events( );*/
    return 0;
}

void test_guillaume( )
{
    gdf::Reader r;

    r.open( "/media/bci-public/clauzel/Clemens/bo8_2dCenterOut_01.gdf");

    r.getEventHeader( );

    r.close( );
}

void test_modify( )
{
    gdf::Modifier m;

    m.open( "test.gdf" );

    m.setSample( 0, 0, -1 );
    m.setSample( 0, 1, -1 );
    m.setSample( 1, 0, -1 );
    m.setSample( 1, 1, -1 );

    gdf::EventHeader *e = m.getEventHeader( );

    e->addEvent( gdf::Mode1Event( ) );

    e->sort( );

    m.close( );
    m.saveChanges( );
}

void test_load( )
{
    gdf::Reader r;

    r.open( "test.gdf" );

    vector<double> s1, s2;
    s1.resize( 22 );
    s2.resize( 11 );

    r.getSignal( 0, &s1[0] );
    r.getSignal( 1, &s2[0] );

    gdf::EventHeader *e = r.getEventHeader( );

    r.close( );

    cout << s1[0];
    for( size_t i=1; i<s1.size(); i++ )
        cout << ", " << s1[i];
    cout << endl;

    cout << s2[0];
    for( size_t i=1; i<s2.size(); i++ )
        cout << ", " << s2[i];
    cout << endl;

    for( size_t i=0; i<e->getNumEvents(); i++ )
    {
        if( e->getMode() == 1 )
        {
            gdf::Mode1Event ev;
            e->getEvent( i, ev );
            cout << ev.position << " - " << ev.type << endl;
        }
        else if( e->getMode() == 1 )
        {
            gdf::Mode3Event ev;
            e->getEvent( i, ev );
            cout << ev.position << " - " << ev.type << ", " << ev.channel << ", " << ev.duration << endl;
        }
    }

}

void test_simple( )
{
    gdf::Writer w;

    w.getMainHeader( ).set_recording_id( "gdf::Writer - Simple Test" );

    w.getMainHeader( ).setString( "patient_id", "bluba" );

    cout << w.getFirstFreeSignalIndex( ) << endl;
    w.createSignal( 0 );
    w.getSignalHeader(0).set_label( "Signal 1" );
    w.getSignalHeader(0).set_datatype( gdf::FLOAT32 );
    w.getSignalHeader(0).set_samplerate( 250 );
    w.getSignalHeader(0).set_digmin( -1 );
    w.getSignalHeader(0).set_digmax( 1 );
    w.getSignalHeader(0).set_physmin( -100 );
    w.getSignalHeader(0).set_physmax( 100 );

    cout << w.getFirstFreeSignalIndex( ) << endl;
    w.createSignal( 1 );
    w.getSignalHeader(1).set_label( "Signal 2" );
    w.getSignalHeader(1).set_datatype( gdf::FLOAT32 );
    w.getSignalHeader(1).set_samplerate( 125 );
    w.getSignalHeader(1).set_digmin( -100 );
    w.getSignalHeader(1).set_digmax( 100 );
    w.getSignalHeader(1).set_physmin( -1 );
    w.getSignalHeader(1).set_physmax( 1 );

    try
    {
        w.open( "test.gdf", gdf::writer_ev_memory | gdf::writer_overwrite );
    }
    catch( gdf::exception::header_issues &e )
    {
        std::list< std::string >::iterator it;
        if( e.warnings.size( ) > 0 )
            std::cout << "Warnings: " << std::endl;
        for( it=e.warnings.begin(); it!=e.warnings.end(); it++ )
            std::cout << " -> " << *it << std::endl;

        if( e.errors.size( ) > 0 )
        {
            std::cout << "Errors: " << std::endl;
        for( it=e.errors.begin(); it!=e.errors.end(); it++ )
            std::cout << " -> " << *it << std::endl;
            exit( 1 );
        }
    }

    cout << "Record Duration: " << w.getMainHeader_readonly( ).get_datarecord_duration()[0] << " / " << w.getMainHeader_readonly( ).get_datarecord_duration()[1] << endl;
    cout << "Number of signals: " << w.getMainHeader_readonly().get_num_signals( ) << endl;

    std::vector<double> f;
    f.push_back( 9 );
    f.push_back( 8 );
    f.push_back( 7 );
    f.push_back( 6 );
    f.push_back( 5 );
    f.push_back( 4 );
    f.push_back( 3 );
    f.push_back( 2 );
    f.push_back( 1 );

    w.blitSamplesPhys( 0, f );
    w.blitSamplesPhys( 0, f );
    w.blitSamplesRaw( 1, f );

    /*w.addSamplePhys( 0, 1 );
    w.addSamplePhys( 0, 2 );
    w.addSamplePhys( 0, 3 );
    w.addSamplePhys( 0, 4 );
    w.addSamplePhys( 0, 5 );
    w.addSampleRaw( 1, 0 );
    w.addSampleRaw( 1, 10 );*/

    w.addEvent( 0, 0x300 );
    w.addEvent( 10, 0x300 );
    w.addEvent( 20, 0x300 );
    w.addEvent( 30, 0x300 );

    w.close( );
}

void test_events( )
{
    gdf::Writer w;

    w.getMainHeader( ).set_recording_id( "gdf::Writer - Event Test" );

    w.getMainHeader( ).setString( "patient_id", "bluba" );

    cout << w.getFirstFreeSignalIndex( ) << endl;
    w.createSignal( 0 );
    w.getSignalHeader(0).set_label( "Signal 1" );
    w.getSignalHeader(0).set_datatype( gdf::FLOAT32 );
    w.getSignalHeader(0).set_samplerate( 250 );
    w.getSignalHeader(0).set_digmin( -1 );
    w.getSignalHeader(0).set_digmax( 1 );
    w.getSignalHeader(0).set_physmin( -100 );
    w.getSignalHeader(0).set_physmax( 100 );

    cout << w.getFirstFreeSignalIndex( ) << endl;
    w.createSignal( 1 );
    w.getSignalHeader(1).set_label( "Signal 2" );
    w.getSignalHeader(1).set_datatype( gdf::FLOAT32 );
    w.getSignalHeader(1).set_samplerate( 125 );
    w.getSignalHeader(1).set_digmin( -100 );
    w.getSignalHeader(1).set_digmax( 100 );
    w.getSignalHeader(1).set_physmin( -1 );
    w.getSignalHeader(1).set_physmax( 1 );

    try
    {
        w.open( "test_events.gdf", gdf::writer_ev_memory | gdf::writer_overwrite );
    }
    catch( gdf::exception::header_issues &e )
    {
        std::list< std::string >::iterator it;
        if( e.warnings.size( ) > 0 )
            std::cout << "Warnings: " << std::endl;
        for( it=e.warnings.begin(); it!=e.warnings.end(); it++ )
            std::cout << " -> " << *it << std::endl;

        if( e.errors.size( ) > 0 )
        {
            std::cout << "Errors: " << std::endl;
        for( it=e.errors.begin(); it!=e.errors.end(); it++ )
            std::cout << " -> " << *it << std::endl;
            exit( 1 );
        }
    }

    cout << "Record Duration: " << w.getMainHeader_readonly( ).get_datarecord_duration()[0] << " / " << w.getMainHeader_readonly( ).get_datarecord_duration()[1] << endl;
    cout << "Number of signals: " << w.getMainHeader_readonly().get_num_signals( ) << endl;

    std::vector<double> f;
    f.push_back( 9 );
    f.push_back( 8 );
    f.push_back( 7 );
    f.push_back( 6 );
    f.push_back( 5 );
    f.push_back( 4 );
    f.push_back( 3 );
    f.push_back( 2 );
    f.push_back( 1 );

    w.blitSamplesPhys( 0, f );
    w.blitSamplesRaw( 1, f );

    w.addSamplePhys( 0, 1 );
    w.addSamplePhys( 0, 2 );
    w.addSamplePhys( 0, 3 );
    w.addSamplePhys( 0, 4 );
    w.addSamplePhys( 0, 5 );
    w.addSampleRaw( 1, 0 );
    w.addSampleRaw( 1, 10 );

    size_t time = 0;
    for( int i=0; i<100; i++ )
    {
        w.addEvent( time, 0x300 );
        w.addEvent( time+900, 0x8300 );
        time += 1000;
    }

    w.close( );
}

void test_buffer( size_t M, size_t T )
{
    std::vector<double> fs;
    fs.push_back( 250 );
    fs.push_back( 125 );
    fs.push_back( 5 );

    gdf::Writer w;

    w.setMaxFullRecords( 0 );

    w.getMainHeader( ).set_recording_id( "gdf::Writer - Simple Test" );

    size_t m=0;
    for( size_t j=0; j<fs.size(); j++)
        for( size_t i=0; i<M; i++ )
        {
            w.getSignalHeader(m).set_label( "Signal "+lexical_cast<string>(m) );
            w.getSignalHeader(m).set_datatype( gdf::FLOAT32 );
            w.getSignalHeader(m).set_samplerate( fs[j] );
            w.getSignalHeader(m).set_digmin( -1 );
            w.getSignalHeader(m).set_digmax( 1 );
            w.getSignalHeader(m).set_physmin( -1 );
            w.getSignalHeader(m).set_physmax( 1 );
            m++;
        }

    M *= fs.size( );

    w.open( "test.gdf", gdf::writer_ev_memory | gdf::writer_overwrite );

    cout << "Record Duration: " << w.getMainHeader_readonly( ).get_datarecord_duration()[0] << " / " << w.getMainHeader_readonly( ).get_datarecord_duration()[1] << endl;
    cout << "Number of signals: " << w.getMainHeader_readonly().get_num_signals( ) << endl;

    size_t R = T * w.getMainHeader_readonly().get_datarecord_duration( )[1] / w.getMainHeader_readonly().get_datarecord_duration( )[0];

    std::vector<size_t> samples_per_channel;
    samples_per_channel.resize( M );
    for( size_t m=0; m<M; m++ )
        samples_per_channel[m] = w.getSignalHeader_readonly( m ).get_samples_per_record( ) * R;
    size_t total_samples = gdf::sum( samples_per_channel );

    double *buf = new double[total_samples];
    for( size_t i=0; i< total_samples; i++)
        buf[i] = i;

    w.blitFromSerialBufferPhys( buf, samples_per_channel );

    delete[] buf;

    w.close( );
}
