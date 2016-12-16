# CSIM
This repository contains software simulations of SigLab's MAC/PHY layer.

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
