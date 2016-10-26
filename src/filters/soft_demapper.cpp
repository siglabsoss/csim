#include <filters/soft_demapper.hpp>
#include <cassert>
#include <iomanip>
#include <bitset>
#include <utils/plotter.hpp>

#define SD_LLR_FORMAT             18, 4,  SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

//
SoftDemapper::SoftDemapper(Mapper::constellation_set_t scheme) :
    HardDemapper(scheme, 0.0),
    m_awgnVariance(1.0) //XXX will need a way to adjust at runtime based on signal power / SNR = noise power
{

}

bool SoftDemapper::output(filter_io_t &data)
{
    bool haveByte = false;
    if (m_llrs.size() > 0) {
        haveByte = true;
        data.type = IO_TYPE_FIXPOINT;
        data.fp.setFormat(SD_LLR_FORMAT);
        data.fp = m_llrs.front();
        m_llrs.pop();
    }

    return haveByte;
}

void SoftDemapper::tick(void)
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
            double incr = calcLLRIncrement(value, it->second.toComplexDouble(), m_awgnVariance);
            if(it->first & (1<<j)) {
                llr_den += incr;
            } else {
                llr_num += incr;
           }

        }

        if( llr_den == 0.0 ) {
            llr_den = 0.000000001; // fudge to avoid infinity
        }
        if( llr_num == 0.0 ) {
            llr_num = 0.000000001;
        }

        m_llrs.push( log(llr_num/llr_den) );
    }
}

double SoftDemapper::calcLLRIncrement(const ComplexDouble &rxSymbol, const ComplexDouble &constellation, double variance)
{
    double distance = std::abs(rxSymbol - constellation);
    return exp(-(distance*distance) / variance);
}

