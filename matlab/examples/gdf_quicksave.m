function gdf_quicksave( filename, s, h, e )

% gdf_quicksave( filename, data, header, events )
%
%  Quickly save a GDF file
%
%  Inputs:
%    filename : output filename
%    data     : signal data
%    header   : GDF header structure
%    events   : Events

    if ~iscell(s)
        error 'Only cell input data is supported so far. Input 2 must be a cell array.'
    end
    
    s = s(:);
    num_signals = length(s);

    handle = gdf_writer( 'init' ); 

    for c = 1 : num_signals
        gdf_writer( 'createsignal', handle, c );
    end
    
    gdf_writer( 'setheader', handle, h );
    
    gdf_writer( 'recordduration', handle, 0 );   % automatic record duration
    
    gdf_writer( 'eventconfig', handle, e.mode, e.sample_rate );
    
    gdf_writer( 'open', handle, filename );
    
    chunksize = 1; % seconds
    num_chunks = ceil( length(s{1}) / (chunksize*h.signals(1).sampling_rate) );
    datapos = ones(1,h.file.num_signals);
    
    for d = 1 : num_chunks    
        for c = 1 : h.file.num_signals
            datalen = chunksize * h.signals(c).sampling_rate;
            datalen = min( datalen, length(s{c}) - datapos(c) + 1 );
            data = s{c}( datapos(c) + (1:datalen) - 1 );
            gdf_writer( 'blitsamples', handle, c, data );
            datapos(c) = datapos(c) + datalen;
        end
    end
    
    if e.mode == 1
        for i = 1 : length( e.position )
            gdf_writer( 'mode1ev', handle, e.position(i), e.event_code(i) );
        end
    elseif e.mode == 3
        for i = 1 : length( e.position )
            gdf_writer( 'mode3ev', handle, e.position(i), e.event_code(i), e.channel(i), e.duration(i) );
        end
    end
    
    gdf_writer( 'close', handle );    
    gdf_writer( 'clear', handle );

end
