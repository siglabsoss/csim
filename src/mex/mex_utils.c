#include <mex.h>
#include <stdbool.h>
#include <stdint.h>

#include "mex_utils.h"

bool check_for_scalars(size_t num_scalars, int nrhs, const mxArray *prhs[])
{
    if (nrhs != num_scalars) {
        return false;
    }
    for (size_t i = 0 ; i < num_scalars; i++) {
        if (!mxIsScalar(prhs[i])) {
            return false;
        }
    }

    return true;
}
bool check_for_double_col_vector(int nrhs, const mxArray *prhs[])
{
    if (nrhs != 1) {
        return false;
    }
    if (!mxIsDouble(prhs[0])) {
        return false;
    }
    if (mxGetNumberOfDimensions(prhs[0]) != 2) {
        return false;
    }
    const mwSize *dimSize = mxGetDimensions(prhs[0]);
    const mwSize colSize = dimSize[1]; //this is OK because dimSize is guaranteed to be >= 2 in length
    return (colSize == 1);
}

bool check_for_double_col_vector_of_length(mwSize len, int nrhs, const mxArray *prhs[])
{
    if (check_for_double_col_vector(nrhs, prhs)) {
        const mwSize *dimSize = mxGetDimensions(prhs[0]);
        const mwSize rowSize = dimSize[0];
        return (rowSize == len);
    }
    return false;
}

bool check_for_double_2d_matrix(int nrhs, const mxArray *prhs[])
{
    if (nrhs != 1) {
        return false;
    }
    if (mxGetNumberOfDimensions(prhs[0]) != 2) {
        return false;
    }
    if (!mxIsDouble(prhs[0])) {
        return false;
    }
    return true;
}

bool check_for_double_2d_matrix_with_colsize(size_t colsize, int nrhs, const mxArray *prhs[])
{
    if (check_for_double_2d_matrix(nrhs, prhs)) {
        const mwSize *dimSize = mxGetDimensions(prhs[0]);
        const mwSize cols = dimSize[1];
        return (colsize == cols);
    }
    return false;
}
