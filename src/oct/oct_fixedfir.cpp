#include <octave/oct.h>

#include <filters/fixed_fir.hpp>

template<class T>
std::vector<T> octaveComplexToFixedComplex(const ComplexNDArray &input)
{
    std::vector<T> result;
    for (octave_idx_type i = 0; i < input.length(); i++) {
        Complex val = input(i);
        result.push_back(FixedComplex16(val.real(), val.imag()));
        //octave_stdout << "Converting complex value " << result[i] << std::endl;
    }
    return result;
}

DEFUN_DLD (oct_fixedfir, args, nargout,
           "oct_fixedfir <taps> <inputs>")
{
    int nargin = args.length ();
    ComplexNDArray result;
    if (nargin != 2) {
        print_usage();
    } else {
        ComplexNDArray taps_in = args(0).complex_array_value();
        ComplexNDArray vals_in = args(1).complex_array_value();
        if (!error_state) {
            std::vector<FixedComplex16> taps = octaveComplexToFixedComplex<FixedComplex16>(taps_in);
            std::vector<FixedComplex16> vals = octaveComplexToFixedComplex<FixedComplex16>(vals_in);
            result.resize1(vals.size()); //expect just as many outputs as inputs
            std::vector<FixedComplex16> filter_out;
            FixedFIR fir(taps);
            for (size_t i = 0; i < vals.size(); i++) {
                filter_io_t data;
                data = vals[i];
                filter_io_t output;
                fir.input(data);
                fir.tick();
                if (fir.output(output)) {
                    result(i) = Complex(output.fcn.real(), output.fcn.imag());
                    //filter_out.push_back(data.fcn);
                    //octave_stdout << "Output " << data << std::endl;
                }
            }
        }
    }
    return octave_value(result);
}
