radios = [0 0; 0 100];
rx_samples = [0; 0];
loadlibrary('/usr/local/lib/libcsim', '/home/behrooze/Work/csim/src/mex/csim.h');
calllib('libcsim', 'csim_init_radios', radios);
calllib('libcsim', 'csim_set_tx_byte', 1, 240);
calllib('libcsim', 'csim_set_tx_byte', 1, 180);
calllib('libcsim', 'csim_set_tx_byte', 1, 060);
%calllib('libcsim', 'csim_set_tx_byte', 1, 240);

for k = 1:10000
    tx_samples = calllib('libcsim', 'csim_tick', rx_samples);
    rx_samples(1, 1) = tx_samples(2, 1);
    rx_samples(2, 1) = tx_samples(1, 1);
    rx_byte = calllib('libcsim', 'csim_get_rx_bytes', 0);
    if size(rx_byte) == [1 1]
        rx_byte
    end
end
