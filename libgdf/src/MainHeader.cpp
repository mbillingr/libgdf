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

#include "GDF/MainHeader.h"
#include <memory.h>

namespace gdf
{
    MainHeader::MainHeader( )
    {
        setDefaultValues( );
    }

    //===================================================================================================
    //===================================================================================================

    void MainHeader::setDefaultValues( )
    {
        set_version_id( "GDF 2.10" );
        memset( patient_id.item, ' ', patient_id.len );
        set_patient_id( "X X X" );
        memset( reserved_1.item, ' ', reserved_1.len );
        set_patient_drugs( 0 );
        set_patient_weight( 0 );
        set_patient_height( 0 );
        set_patient_flags( 0 );
        memset( recording_id.item, 0, recording_id.len );
        recording_location[0] = 0;
        recording_location[1] = 0;
        recording_location[2] = 0;
        recording_location[3] = 0;
        set_recording_start( 0 );
        set_patient_birthday( 0 );
        set_header_length( 0 );
        memset( patient_ICD.item, ' ', patient_ICD.len );
        set_equipment_provider_classification( 0 );
        memset( reserved_2.item, 0, reserved_2.len );
        patient_headsize[0] = 0;
        patient_headsize[1] = 0;
        patient_headsize[2] = 0;
        pos_reference[0] = 0;
        pos_reference[1] = 0;
        pos_reference[2] = 0;
        pos_ground[0] = 0;
        pos_ground[1] = 0;
        pos_ground[2] = 0;
        set_num_datarecords( -1 );
        datarecord_duration[0] = 0;
        datarecord_duration[1] = 1;
        set_num_signals( 0 );
        memset( reserved_3.item, 0, reserved_3.len );
    }

    //===================================================================================================
    //===================================================================================================

    void MainHeader::copyFrom( const MainHeader &other )
    {
        set_version_id( other.get_version_id() );
        memcpy( patient_id.item, other.patient_id.item, patient_id.len );
        memset( reserved_1.item, ' ', reserved_1.len );
        set_patient_drugs( other.get_patient_drugs() );
        set_patient_weight( other.get_patient_weight() );
        set_patient_height( other.get_patient_height() );
        set_patient_flags( other.get_patient_flags() );
        memcpy( recording_id.item, other.recording_id.item, recording_id.len );
        recording_location[0] = other.recording_location[0];
        recording_location[1] = other.recording_location[1];
        recording_location[2] = other.recording_location[2];
        recording_location[3] = other.recording_location[3];
        set_recording_start( other.get_recording_start() );
        set_patient_birthday( other.get_patient_birthday() );
        set_header_length( other.get_header_length() );
        memcpy( patient_ICD.item, other.patient_ICD.item, patient_ICD.len );
        set_equipment_provider_classification( other.get_equipment_provider_classification() );
        memset( reserved_2.item, 0, reserved_2.len );
        patient_headsize[0] = other.patient_headsize[0];
        patient_headsize[1] = other.patient_headsize[1];
        patient_headsize[2] = other.patient_headsize[2];
        pos_reference[0] = other.pos_reference[0];
        pos_reference[1] = other.pos_reference[1];
        pos_reference[2] = other.pos_reference[2];
        pos_ground[0] = other.pos_ground[0];
        pos_ground[1] = other.pos_ground[1];
        pos_ground[2] = other.pos_ground[2];
        set_num_datarecords( other.get_num_datarecords() );
        datarecord_duration[0] = other.datarecord_duration[0];
        datarecord_duration[1] = other.datarecord_duration[0];
        set_num_signals( other.get_num_signals() );
        memset( reserved_3.item, 0, reserved_3.len );
    }
}
