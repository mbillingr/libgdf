function gdf_resample( fs_new, inputfile, outputfile, datatype )

% 1. load gdf file

    [signals, header, events] = gdf_reader( inputfile, 'multiratesignals', 'single', 'dataorientation', 'row' );

    num_channels = length( signals );
    
% 2. resample

    for c = 1 : num_channels
        
        fs_old = header.signals(c).sampling_rate;

        div = gcd( fs_new, fs_old );
        P = fs_new / div;
        Q = fs_old /div;
        
        signals{c} = resample( signals{c}, P, Q );
        
        header.signals(c).sampling_rate = fs_new;
        
        % better adjust physmin and physmax, as the signal may locally
        % increase in amplitude due to the anti aliasing filter.        
        header.signals(c).physmin = min(signals{c});
        header.signals(c).physmax = max(signals{c});
        
        if exist( 'datatype', 'var' )
            header.signals(c).datatype = datatype;
            switch datatype
                case 1, header.signals(c).digmin=-128; header.signals(c).digmax=127;
                case 2, header.signals(c).digmin=0; header.signals(c).digmax=256;
                case 3, header.signals(c).digmin=-32768; header.signals(c).digmax=32767;
                case 4, header.signals(c).digmin=0; header.signals(c).digmax=65536;
                case 5, header.signals(c).digmin=-2147483648; header.signals(c).digmax=2147483647;
                case 6, header.signals(c).digmin=0; header.signals(c).digmax=4294967295;
                case 7, header.signals(c).digmin=-9223372036854775808; header.signals(c).digmax=9223372036854775807;
                case 8, header.signals(c).digmin=0; header.signals(c).digmax=18446744073709551615;
                case 16, header.signals(c).digmin=-1; header.signals(c).digmax=1;
                case 17, header.signals(c).digmin=-1; header.signals(c).digmax=1;
            end
        end
    end
    
% 3. fix event positions

    if events.mode == 3
        events.duration = events.duration * double(fs_new / events.sample_rate);
    end

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
    
%     for c = 1 : num_channels
%         gdf_writer( 'blitsamples', handle, c, signals{c} );
%     end
    
    chunksize = 512;
    num_chunks = ceil(size(signals{1},2) / chunksize);
    
    for d = 1 : num_chunks   
        for c = 1 : num_channels
            start = (d-1)*chunksize + 1;
            ende = min( d*chunksize, size(signals{1},2) );
            gdf_writer( 'blitsamples', handle, c, signals{c}(start:ende) );
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
