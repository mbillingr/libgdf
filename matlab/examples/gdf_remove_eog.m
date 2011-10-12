function gdf_remove_eog( inputfile, outputfile, reffile, eegchan, eogchan )

% gdf_remove_eog( inputfile, outputfile, reffile )
%
%  Remove eog artifacts by regression.
%
%  Inputs:
%    inputfile  : filename of the original raw data
%    outputfile : output filename
%    reffile    : file that contains eog reference data
%    eegchan    : EEG channel numbers
%    eogchan    : EOG channel numbers

% apply spatial filter matrix to a gdf

% load gdf files

    [s, h, e] = gdf_reader( inputfile, 'dataformat', 'single' );
    input.s = s;
    input.h = h;
    input.e = e;

    [s, h, e] = gdf_reader( reffile, 'dataformat', 'single' );
    ref.s = s;
    ref.h = h;
    ref.e = e;
    
% calculate eog regression

    refeeg = [ref.s{eegchan}];
    refeog = [ref.s{eogchan}];
    
    refeeg( any( isnan( refeeg ), 2 ), : ) = [];
    refeog( any( isnan( refeog ), 2 ), : ) = [];

    Y = refeeg;
    U = refeog;
    
    CNN = U'*U/size(U,1);
    CNY = U'*Y/size(U,1);
    eog_weights = CNN\CNY;
    
% apply eog correction

    eeg = [input.s{eegchan}];
    eog = [input.s{eogchan}];
    
    eeg = eeg - eog * eog_weights;
    
% 4. save new gdf

    handle = gdf_writer( 'init' ); 

    for c = 1 : input.h.file.num_signals
        gdf_writer( 'createsignal', handle, c );
    end
    
    gdf_writer( 'setheader', handle, input.h );
    
    gdf_writer( 'recordduration', handle, 0 );   % automatic record duration
    
    gdf_writer( 'eventconfig', handle, input.e.mode, input.e.sample_rate );
    
    gdf_writer( 'open', handle, outputfile );
    
    chunksize = 1; % seconds
    num_chunks = ceil( length(input.s{1}) / (chunksize*input.h.signals(1).sampling_rate) );
    datapos = ones(1,input.h.file.num_signals);
    
    for d = 1 : num_chunks    
        for c = 1 : input.h.file.num_signals
            datalen = chunksize * input.h.signals(c).sampling_rate;
            if find( eegchan==c )
                data = input.s{c}( datapos(c) + (1:datalen) - 1 );
            else
                data = input.s{c}( datapos(c) + (1:datalen) - 1 );
            end            
            gdf_writer( 'blitsamples', handle, c, data );
            datapos(c) = datapos(c) + datalen;
        end
    end
    
    if input.e.mode == 1
        for e = 1 : length( input.e.position )
            gdf_writer( 'mode1ev', handle, input.e.position(e), input.e.event_code(e) );
        end
    elseif input.e.mode == 3
        for e = 1 : length( input.e.position )
            gdf_writer( 'mode3ev', handle, input.e.position(e), input.e.event_code(e), input.e.channel(e), input.e.duration(e) );
        end
    end
    
    gdf_writer( 'close', handle );
    
    gdf_writer( 'clear', handle );

end
