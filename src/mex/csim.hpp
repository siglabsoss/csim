#ifndef CSIM_HPP
#define CSIM_HPP

#include "matrix.h"

void csim_init_radios(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void csim_get_rx_bytes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void csim_set_tx_byte(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void csim_tick(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

#endif //CSIM_HPP
