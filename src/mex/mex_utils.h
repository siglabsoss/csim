#pragma once

bool check_for_scalars(size_t num_scalars, int nrhs, const mxArray *prhs[]);
bool check_for_double_col_vector(int nrhs, const mxArray *prhs[]);
bool check_for_double_col_vector_of_length(mwSize len, int nrhs, const mxArray *prhs[]);
bool check_for_double_2d_matrix(int nrhs, const mxArray *prhs[]);
bool check_for_double_2d_matrix_with_colsize(size_t colsize, int nrhs, const mxArray *prhs[]);
