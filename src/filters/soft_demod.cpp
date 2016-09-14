#include <filters/soft_demod.hpp>
#include <cassert>
#include <iomanip>
#include <bitset>
//
SoftDemod::SoftDemod(Modulator::mod_scheme_t scheme, double noise_variance) :
HardDemod(scheme, 0),
m_noise_variance(noise_variance)
{
}

bool SoftDemod::output(filter_io_t &data)
{
    bool haveByte = false;
    if (m_llrs.size() > 0) {
        haveByte = true;

        data = ComplexDouble(m_llrs.front(),0);
        m_llrs.pop();
        data.type = IO_TYPE_FIXED_COMPLEX;
    }

    return haveByte;
}

void SoftDemod::tick(void)
{
    if (!m_inputValid) {
        return;
    }
    m_inputValid = false;
    constellation_t value = m_value.toComplexDouble();


    for(size_t j = 0; j < m_bitsPerSymbol; j++) {
        double llr_num = 0;
        double llr_den = 0;
        for (auto it = m_constellations.begin(); it != m_constellations.end(); ++it) {
            double distance = abs(it->second - value);
            if(it->first & (1<<j)) {
                llr_num += exp(-pow(distance,2)/m_noise_variance);
            } else {
                llr_den += exp(-pow(distance,2)/m_noise_variance);
           }

        }
        m_llrs.push( log2(llr_num/llr_den) );
    }
}

