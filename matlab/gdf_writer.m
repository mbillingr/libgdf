% function [VAROUT] = gdf_writer( COMMAND, VARIN )
%
% Create GDF files.
% Number of Inputs and Outputs depends on the Command.
%
% Commands:
%
%   INIT: create a new writer instance
%       handle = gdf_writer( 'Init' );
%
%   CLEAR: remove writer instance
%       handle = gdf_writer( 'Clear', handle );
%
%   CLEAR ALL: remove all writer instances
%       handle = gdf_writer( 'Clear All' );
%
%   OPEN: create a new writer instance
%       handle = gdf_writer( 'Open', handle, filename );
%
%   CLOSE: create a new writer instance
%       handle = gdf_writer( 'Close', handle );
%
%   GETHEADER: returns current header struct
%       header = gdf_writer( 'GetHeader', handle );
%
%   SETHEADER: set header struct
%       gdf_writer( 'SetHeader', handle, header );
%
%   CREATESIGNAL: create a new signal
%       gdf_writer( 'CreateSignal', handle, index );
%
%   RECORDDURATION: set record duration
%       gdf_writer( 'RecordDuration', handle, seconds );
%
%   EVENTCONFIG: set event configuration
%       gdf_writer( 'EventConfig', handle, mode, samplerate );
%
%   ADDSAMPLE: write a sample (physical units)
%       gdf_writer( 'AddSample', handle, channel, value );
%
%   ADDRAWSAMPLE: write a sample (raw)
%       gdf_writer( 'AddRawSample', handle, channel, value );
%
%   ADDVECSAMPLE: write a sample vector (physical units, multiple channels)
%       gdf_writer( 'AddVecSample', handle, first_channel, values[] );
%
%   BLITSAMPLES: write multiple samples (physical units, one channel)
%       gdf_writer( 'BlitSamples', handle, channel, values[] );
%
%   BLITRAWSAMPLES: write multiple samples (physical units, one channel)
%       gdf_writer( 'BlitRawSamples', handle, channel, values[] );
%
%   MODE1EV: write a mode 1 event
%       gdf_writer( 'Mode1Ev', handle, timestamp, eventcode );
%
%   MODE3EV: write a mode 3 event
%       gdf_writer( 'Mode3Ev', handle, timestamp, eventcode, channel, duration );
%
