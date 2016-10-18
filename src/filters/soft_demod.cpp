#include <filters/soft_demod.hpp>
#include <cassert>
#include <iomanip>
#include <bitset>
#include <utils/plotter.hpp>

//
SoftDemod::SoftDemod(Modulator::mod_scheme_t scheme, double noise_variance) :
HardDemod(scheme, 0),
m_noise_variance(noise_variance)
{
    assert(noise_variance > 0);
//    cout << "SoftDemod with " << m_bitsPerSymbol << " m_bitsPerSymbol" << endl;
}

bool SoftDemod::output(filter_io_t &data)
{
    bool haveByte = false;
    if (m_llrs.size() > 0) {
        haveByte = true;

        data = ComplexDouble(m_llrs.front(),0);
        m_llrs.pop();
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
    }

    return haveByte;
}

void SoftDemod::tick(void)
{
    if (!m_inputValid) {
        return;
    }
    m_inputValid = false;
    ComplexDouble value = m_value.toComplexDouble(); // FIXME


    for(size_t j = 0; j < m_bitsPerSymbol; j++) {
        double llr_num = 0;
        double llr_den = 0;
        for (auto it = m_constellations.begin(); it != m_constellations.end(); ++it) {
            double distance = abs(it->second.toComplexDouble() - value);
//            cout << endl << endl << "value: " << value << " dist " << distance << endl;
            if(it->first & (1<<j)) {
                llr_num += distance;
            } else {
                llr_den += distance;
           }

        }
//        cout << endl << endl << "l " << log2(llr_num/llr_den) << endl;

        if( llr_den == 0.0 ) {
            llr_den = 0.000000001; // fudge to avoid infinity
        }
        if( llr_num == 0.0 ) {
            llr_num = 0.000000001;
        }

//        cout << "num: " << llr_num << " / " << llr_den << endl;

        m_llrs.push( log2(llr_num/llr_den) );
    }
}

