# CSIM
This repository contains software simulations of SigLab's MAC/PHY layer.

## Building
### Dependencies
The following dependencies need to be installed in order to build CSIM
* libboost-all-dev (installed via apt-get)
* zeromq (installed from http://zeromq.org/intro:get-the-software)

### Build Steps
#### Applications
##### From CLI
1. `./scripts/generate_eclipse_build.sh`
1. `cd build/`
1. `make all -j4`

#### Unit Tests
##### From CLI
###### Build and run all tests
1. `./src/test/run_tests.sh`
###### Build individual tests
1. Invoke the `run_tests.sh` script at least once
1. `cd build_utest/`
1. `make test_name`

## Directory Structure
The repository is structured as follows:
* src/apps/ - standalone programs that generally make use of the software in this
repository.
* src/core/ - core classes that make up the structure of the simulator and
define some abstractions.
* src/filters/ - actual digital signal processing blocks
* src/channel/ - code related to channel modeling
* src/3rd/ - 3rd party libraries
* src/interfaces/ - abstract interfaces
* src/mathlib/ - reusable mathematical routines
* src/mex/ - matlab interfaces
* src/oct/ - octave interfaces
* src/probes/ - probes to capture DSP inputs and/or outputs
* src/types/ - custom types and definitions
* src/utils/ - general purpose reusable routines
