radios = [0 0; 0 100];
rx_samples = [0; 0];
tx_samples = []
loadlibrary('/usr/local/lib/libcsim', '/home/behrooze/Work/csim/src/mex/csim.h');
calllib('libcsim', 'csim_init_radios', radios);
calllib('libcsim', 'csim_set_tx_byte', 0, 240);
%calllib('libcsim', 'csim_set_tx_byte', 1, 240);

for k = 1:10000
    results = calllib('libcsim', 'csim_tick', rx_samples);
    rx_samples(1, 1) = results(2, 1);
    rx_samples(2, 1) = results(1, 1);
    if (max(abs(results)) > 0.0)
        tx_samples = [tx_samples, results];
    end
    output = tx_samples';
%    tx_samples(1, 1) = rx_samples(2, 1);
%    tx_samples(2, 1) = rx_samples(1, 1);
end
