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

#ifndef GDF_HEADERSTRUCT_INCLUDED
#define GDF_HEADERSTRUCT_INCLUDED


#define GDFH_STRUCT_FILE_VersionID              "file.version_id"
#define GDFH_LENGTH_FILE_VersionID              8
#define GDFH_STRUCT_FILE_HeaderLength           "file.header_length"
#define GDFH_STRUCT_FILE_NumDatarecords         "file.num_datarecords"
#define GDFH_STRUCT_FILE_DatarecordsDuration    "file.datarecord_duration"
#define GDFH_STRUCT_FILE_NumSignals             "file.num_signals"


#define GDFH_STRUCT_PATIENT_IdentificationCode  "patient.identification_code"
#define GDFH_STRUCT_PATIENT_Name                "patient.name"
#define GDFH_STRUCT_PATIENT_Classification      "patient.classification"
#define GDFH_STRUCT_PATIENT_Drugs               "patient.drugs"
#define GDFH_STRUCT_PATIENT_Drugs_Smoking       "patient.drugs.smoking"
#define GDFH_STRUCT_PATIENT_Drugs_Alcohol       "patient.drugs.alcohol_abuse"
#define GDFH_STRUCT_PATIENT_Drugs_Drugs         "patient.drugs.drug_abuse"
#define GDFH_STRUCT_PATIENT_Drugs_Medics        "patient.drugs.medication"
#define GDFH_STRUCT_PATIENT_Weight              "patient.weight"
#define GDFH_STRUCT_PATIENT_Height              "patient.height"
#define GDFH_STRUCT_PATIENT_Flags               "patient.flags"
#define GDFH_STRUCT_PATIENT_Flags_Gender        "patient.flags.gender"
#define GDFH_STRUCT_PATIENT_Flags_Handed        "patient.flags.handedness"
#define GDFH_STRUCT_PATIENT_Flags_Visual        "patient.flags.visual_impairment"
#define GDFH_STRUCT_PATIENT_Flags_Hearty        "patient.flags.heart_impairment"
#define GDFH_STRUCT_PATIENT_Birthdate           "patient.birthday"
#define GDFH_STRUCT_PATIENT_ICD                 "patient.ICD"
#define GDFH_LENGTH_PATIENT_ICD                 6
#define GDFH_STRUCT_PATIENT_HeadSize            "patient.headsize"


#define GDFH_STRUCT_REC_ID                  "recording.id"
#define GDFH_LENGTH_REC_ID                  64
#define GDFH_STRUCT_REC_LOC                 "recording.location"
#define GDFH_STRUCT_REC_LOC_VER             "recording.location.version"
#define GDFH_STRUCT_REC_LOC_SIZE            "recording.location.size"
#define GDFH_STRUCT_REC_LOC_HRPEC           "recording.location.horizontal_precision"
#define GDFH_STRUCT_REC_LOC_VRPEC           "recording.location.vertical_precision"
#define GDFH_STRUCT_REC_LOC_LAT             "recording.location.latitude"
#define GDFH_STRUCT_REC_LOC_LON             "recording.location.longitude"
#define GDFH_STRUCT_REC_LOC_ALT             "recording.location.altitude"
#define GDFH_STRUCT_REC_START               "recording.start"
#define GDFH_STRUCT_REC_EQUIPMENT           "recording.equipment_provider_classification"
#define GDFH_STRUCT_REC_PosRef              "recording.pos_reference"
#define GDFH_STRUCT_REC_PosGND              "recording.pos_ground"

//  signal and tlv headers work slightly differently, due to their array-ish nature.
#define GDFH_STRUCT_SIGNAL                  "signals"
    #define GDFH_STRUCT_SIGNAL_Label            "label"
    #define GDFH_LENGTH_SIGNAL_Label            16
    #define GDFH_STRUCT_SIGNAL_Transducer       "transducer_type"
    #define GDFH_LENGTH_SIGNAL_Transducer       80
    #define GDFH_STRUCT_SIGNAL_PhysDim          "physical_dimension"
    #define GDFH_LENGTH_SIGNAL_PhysDim          6
    #define GDFH_STRUCT_SIGNAL_PhysCode         "physical_dimension_code"
    #define GDFH_STRUCT_SIGNAL_PhysMin          "physmin"
    #define GDFH_STRUCT_SIGNAL_PhysMax          "physmax"
    #define GDFH_STRUCT_SIGNAL_DigMin           "digmin"
    #define GDFH_STRUCT_SIGNAL_DigMax           "digmax"
    #define GDFH_STRUCT_SIGNAL_Lowpass          "lowpass"
    #define GDFH_STRUCT_SIGNAL_Highpass         "highpass"
    #define GDFH_STRUCT_SIGNAL_Notch            "notch"
    #define GDFH_STRUCT_SIGNAL_RecSamples       "samples_per_record"
    #define GDFH_STRUCT_SIGNAL_SampleRate       "sampling_rate"
    #define GDFH_STRUCT_SIGNAL_Datatype         "datatype"
    #define GDFH_STRUCT_SIGNAL_SensorPos        "sensor_pos"
    #define GDFH_STRUCT_SIGNAL_Sensinf          "sensor_info"
    #define GDFH_STRUCT_SIGNAL_Sensinf_Info     "sensor_info.information"
    #define GDFH_STRUCT_SIGNAL_Sensinf_Value    "sensor_info.value"


#define GDFH_STRUCT_TLV        "TLV"
    #define GDFH_STRUCT_TLV_Tag    "tag"
    #define GDFH_STRUCT_TLV_Val    "value"

#define GDFE_MODE     "mode"
#define GDFE_FS       "sample_rate"
#define GDFE_POS      "position"
#define GDFE_TYP      "event_code"

#define GDFE_3_CHA      "channel"
#define GDFE_3_DUR      "duration"

#endif
