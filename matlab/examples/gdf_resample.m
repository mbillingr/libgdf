function gdf_resample( fs_new, inputfile, outputfile )

% 1. load gdf file

    [signals, header, events] = gdf_reader( inputfile, 'multiratesignals', 'single' );

    num_channels = length( signals );
    
% 2. resample

    for c = 1 : num_channels
        
        fs_old = header.signals(c).sampling_rate;

        div = gcd( fs_new, fs_old );
        P = fs_new / div;
        Q = fs_old /div;
        
        signals{c} = resample( signals{c}, P, Q );
        
        header.signals(c).sampling_rate = fs_new;        
    end
    
% 3. fix event positions

    events.position = events.position * double(fs_new / events.sample_rate);
    events.sample_rate = fs_new;
    
% 4. save new gdf

    handle = gdf_writer( 'init' );
    
    for c = 1 : num_channels
        gdf_writer( 'createsignal', handle, c );
    end
    
    gdf_writer( 'setheader', handle, header );
    
    gdf_writer( 'recordduration', handle, 0 );   % automatic record duration
    
    gdf_writer( 'eventconfig', handle, events.mode, events.sample_rate );
    
    gdf_writer( 'open', handle, outputfile );
    
    for c = 1 : num_channels
        gdf_writer( 'blitsamples', handle, c, signals{c} );
    end
    
    if events.mode == 1
        for e = 1 : length( events.position )
            gdf_writer( 'mode1ev', handle, events.position(e), events.event_code(e) );
        end
    elseif events.mode == 3
        for e = 1 : length( events.position )
            gdf_writer( 'mode1ev', handle, events.position(e), events.event_code(e), events.channel(e), events.duration(e) );
        end
    end
    
    gdf_writer( 'close', handle );
    
    gdf_writer( 'clear', handle );

end