# OFDM Frame Captures
The data in this directory is used to test frame synchronization via prerecorded
captures of OFDM frames.

## Generation
This data was generated using the `filter_loopback` tool, with filter probing
enabled to capture a dump of the DDC output.
The `filter_loopback_80211n_codes.json` config file was used. The `snr` `start`
and `end` field were set to the same value to force the simulation to run
for a single SNR at a time. The `start` and `end` values were incremented, and
the simulation was rerun until all the data captures in this folder were
created.

## Frame Structure
The frame structure, as defined by the OFDM transmit chain in the
`filter_loopback` tool, is as follows:

`[CP]` `[training symbol 1]` `[CP]` `[training symbol 2]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]` `[CP]` `[OFDM Symbol]`

* `[CP]` = Cyclic Prefix
* `[training symbol 1]` = Training symbol with pseudo-random BPSK sequence occupying every fourth tone (thus repeating four times in time-domain)
* `[training symbol 2]` = Training symbol with pseudo-random BPSK sequence occupying every other tone (thus repeating twice in time-domain)
* `[OFDM Symbol]` = OFDM symbol consisting of random BPSK accross the tones.

The symbols are 1024 samples long. As illustrated above, there are two pilots followed by eight OFDM symbols for every frame.

## Data Captured
Each file holds a bit more than 10 full frames. Each file is a capture for a different SNR. The following SNRs were captured:
* 0dB
* 2.5dB
* 5dB
* 7.5dB
* 10dB
* 15dB
* 20dB
