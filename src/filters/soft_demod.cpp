#include <filters/soft_demod.hpp>
#include <cassert>
#include <iomanip>
#include <bitset>
//
SoftDemod::SoftDemod(Modulator::mod_scheme_t scheme) :
HardDemod(scheme, 0)
//    m_bits(),
//    m_value(),
//    m_inputValid(false),
//    m_theta(theta),
//    m_constellations(),
//    m_bitsPerSymbol(0)
{
//    switch(scheme) {
//        case Modulator::MOD_SCHEME_BPSK:
//            m_constellations = Modulator::getBPSKConstellations();
//            m_bitsPerSymbol = 1;
//            break;
//        case Modulator::MOD_SCHEME_QPSK:
//            m_constellations = Modulator::getQPSKConstellations();
//            m_bitsPerSymbol = 2;
//            break;
//        case Modulator::MOD_SCHEME_8PSK:
//            m_constellations = Modulator::get8PSKConstellations();
//            m_bitsPerSymbol = 3;
//            break;
//        case Modulator::MOD_SCHEME_QAM16:
//            m_constellations = Modulator::getQAM16Constellations();
//            m_bitsPerSymbol = 4;
//            break;
//        case Modulator::MOD_SCHEME_NULL:
//        default:
//            break;
//    }
}
//
//bool SoftDemod::input(const filter_io_t &data)
//{
//    m_value = data;
//    m_inputValid = true;
//    return true;
//}
//
bool SoftDemod::output(filter_io_t &data)
{
    bool haveByte = false; //dequeueByte(data.byte);
    if (m_llrs.size() > 0) {
        haveByte = true;

        data = ComplexDouble(m_llrs.front(),0);
        m_llrs.pop();
        data.type = IO_TYPE_COMPLEX_DOUBLE;

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
    if (abs(value) < 0.1) { //XXX define threshold
        return;
    }
//    symbol_t symbol;
//    double minDistance = 1e100;

    size_t count = m_constellations.size();
//    cout << "count was" << count << endl;

    double distances[count];
    double prob[count];
    double probsum[count];
    double p1[m_bitsPerSymbol];
    size_t mmap[count];
    bool print = false;

    if(print) {
        cout << "for point" << value << " and " << m_bitsPerSymbol << " bits per" << endl;
    }

    size_t i = 0;
    for (auto it = m_constellations.begin(); it != m_constellations.end(); ++it) {
//        cout << "I " << i << endl;
        double distance = abs(it->second - value);
        distances[i] = distance;
        prob[i] = (1/(1+distance));
        if(print) {
            cout << "bitval " << hex << std::bitset<8>((int)it->first) << endl;
            cout << "distance[" << i << "] =  " << distances[i] << endl;
            cout << "prob[" << i << "] =  " << prob[i] << endl;
            cout << endl;
        }

        if(it->first & 0x01)
        {
            mmap[i] = 0;
        }

        i++;
    }

    probsum[0] = (prob[0] + prob[2])/2;
    probsum[1] = (prob[1] + prob[3])/2;
    probsum[2] = (prob[0] + prob[1])/2;
    probsum[3] = (prob[2] + prob[3])/2;

//    cout << "log " << log2(probsum[0]/probsum[1]) << endl;

    m_llrs.push( log2(probsum[0]/probsum[1]) );
    m_llrs.push( log2(probsum[2]/probsum[3]) );


    if(print) {
        cout << "p[b0=0] " << probsum[0] << endl;
        cout << "p[b0=1] " << probsum[1] << endl;
        cout << "p[b1=0] " << probsum[2] << endl;
        cout << "p[b1=1] " << probsum[3] << endl;

    }





}



//void SoftDemod::queueLLR(double llr)
//{
//    for (size_t i = 0; i < m_bitsPerSymbol; i++) {
//        bool value = symbol & (1 << i);
//        m_bits.push(value);
//    }
//}
//
//bool SoftDemod::dequeueByte(uint8_t &byte)
//{
//    if (m_bits.size() < 8) {
//        return false;
//    }
//    byte = 0;
//    for (size_t i = 0; i < 8; i++) {
//        bool nextBit = !!m_bits.front();
//        m_bits.pop();
//        byte |= (nextBit << i);
//    }
//    return true;
//}
//
//double SoftDemod::angleDiff(double a, double b)
//{
//    double diff = abs(a - b);
//    if (diff > M_PI) {
//        diff = 2 * M_PI - diff;
//    }
//    return diff;
//}
