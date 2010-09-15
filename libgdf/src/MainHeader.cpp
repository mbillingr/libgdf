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
}
