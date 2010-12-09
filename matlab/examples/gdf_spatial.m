function gdf_spatial( filtermatrix, inputfile, outputfile, labels )

% apply spatial filter matrix to a gdf

% load gdf file

    [signals, header, events] = gdf_reader( inputfile, 'dataformat', 'matrix' );

    num_channels = size( filtermatrix, 1 );
    
% apply filter

    signals = filtermatrix * signals;
    
    header.signals( num_channels+1 : end ) = [];
    
    for c = 1 : num_channels
        header.signals( c ).label = labels{c};
    end
    
% 4. save new gdf

    handle = gdf_writer( 'init' ); 

    for c = 1 : num_channels
        gdf_writer( 'createsignal', handle, c );
    end
    
    gdf_writer( 'setheader', handle, header );
    
    gdf_writer( 'recordduration', handle, 0 );   % automatic record duration
    
    gdf_writer( 'eventconfig', handle, events.mode, events.sample_rate );
    
    gdf_writer( 'open', handle, outputfile );
    
    
    
    chunksize = 512;
    num_chunks = ceil(size(signals,2) / chunksize);
    
    for d = 1 : num_chunks   
        for c = 1 : num_channels
            start = (d-1)*chunksize + 1;
            ende = min( d*chunksize, size(signals,2) );
            gdf_writer( 'blitsamples', handle, c, signals(c,start:ende) );
        end
    end
    
    if events.mode == 1
        for e = 1 : length( events.position )
            gdf_writer( 'mode1ev', handle, events.position(e), events.event_code(e) );
        end
    elseif events.mode == 3
        for e = 1 : length( events.position )
            gdf_writer( 'mode3ev', handle, events.position(e), events.event_code(e), events.channel(e), events.duration(e) );
        end
    end
    
    gdf_writer( 'close', handle );
    
    gdf_writer( 'clear', handle );

end