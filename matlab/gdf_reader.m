


% function [SIGNALS, HEADER, EVENTS] = gdf_writer( FILENAME, OPTION, VALUE, ... )
%
% Load GDF files.
%
% Input Arguments:
%
%   FILENAME:   Full pathname of the file to load (including extension)
%
% Output Arguments:
%
%   SIGNALS:    Contains all signals (channels) stored in the GDF file.
%               The output format (cell array, matrix) can be changed
%               with the "MULTIRATESIGNALS" option.
%
%   HEADER:     Header Structure that contains all Meta-Information
%
%   EVENTS:     Event Structure
%
%   OPTIONS:    Options are provided as Option-Value pairs.
%       "MULTIRATESIGNALS" possible values:
%           "UPSAMPLE"  All signals are upsampled to the highest sampling
%                       rate that occurs in the file and returned in a matrix.
%						Upsampling is performed by interpolation (see
%						Option "UPSAMPLEMODE").
%           "SINGLE"    (default) A cell array is returned, that contains each signal
%                       as a separate vector.
%           "GROUP"     All signals with the same sampling rates are grouped
%                       into matrices, which are returned in a cell array.
%
%       "UPSAMPLEMODE"  determines how upsampling is performed when "MULTIRATESIGNALS"
%						is set to "UPSAMPLE".
%			"NEAREST"	Nearest Neighbor interpolation
%			"LINEAR"	Linear Interpolation