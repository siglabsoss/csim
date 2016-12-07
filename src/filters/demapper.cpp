#include <filters/demapper.hpp>
#include <cassert>

#define SD_LLR_FORMAT             18, 8,  SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

Demapper::Demapper(MCS mcs, bool hard) :
    m_value(),
    m_inputValid(false),
    m_constellations(),
    m_bitsPerSymbol(0),
    m_hard(hard),
    m_awgnVariance(1.0), //XXX will need a way to adjust at runtime based on signal power / SNR = noise power
    m_llrs()
{
    m_bitsPerSymbol = mcs.getNumBitsPerSubcarrier();
    switch(mcs.getModulation()) {
        case MCS::MOD_BPSK:
            m_constellations = Mapper::getBPSKConstellations();
            break;
        case MCS::MOD_QPSK:
            m_constellations = Mapper::getQPSKConstellations();
            break;
        case MCS::MOD_8PSK:
            m_constellations = Mapper::get8PSKConstellations();
            break;
        case MCS::MOD_QAM16:
            m_constellations = Mapper::getQAM16Constellations();
            break;
        case MCS::MOD_NULL:
        default:
            break;
    }
}

Demapper::~Demapper()
{
}

bool Demapper::input(const filter_io_t &data)
{
    m_value = data;

    m_inputValid = true;
    return true;
}

bool Demapper::output(filter_io_t &data)
{
    bool haveBit = false;
    if (m_llrs.size() > 0) {
        haveBit = true;
        if (m_hard) { //hard decision output (bit)
            data.type = IO_TYPE_BIT;
            data.bit = (m_llrs.front() < 0.0);
        } else { //soft decision output (fixed point LLR)
            data.type = IO_TYPE_FIXPOINT;
            data.fp.setFormat(SD_LLR_FORMAT);
            data.fp = m_llrs.front();
        }
        m_llrs.pop();
    }

    return haveBit;
}

void Demapper::tick(void)
{
    if (!m_inputValid) {
        return;
    }
    m_inputValid = false;
    ComplexDouble value = m_value.toComplexDouble(); // FIXME

//    std::cout << "demap: " << value << std::endl;

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

        double llr = log(llr_num/llr_den);
        m_llrs.push( llr );
    }
}

double Demapper::calcLLRIncrement(const ComplexDouble &rxSymbol, const ComplexDouble &constellation, double variance)
{
    double distance = std::abs(rxSymbol - constellation);
    return exp(-(distance*distance) / variance);
}

