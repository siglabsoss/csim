/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 *
 * This implementation was inspired by the non-recursive method outlined here: http://www.engineeringproductivitytools.com/stuff/T0001/PT04.HTM#Head208
 *
 * The "block floating point" technique was inspired by the paper here: http://www.ti.com/lit/an/spra948/spra948.pdf
 *
 * In order to switch between 32-bit and 16-bit inputs,
 * change the type of m_inputs and twiddle to match, and also the scale factor in the calculateShiftAmountForStage function
 */

#include <filters/bfp_fft.hpp>
#include <utils/utils.hpp>

//NOTE: This is meant to match the scaling factor of the input fixed point type (Q.31)
static constexpr uint32_t SCALE_FACTOR = (1 << 31);

BFPFFT::BFPFFT(size_t N, bool inverse) :
    FilterChainElement(std::string("BFPFFT")),
    m_numStages(static_cast<uint64_t>(log2(N))),
    m_inputs(N),
    m_outputs(N),
    m_twiddleFactors(N),
    m_outputValid(false),
    m_outputIdx(0),
    m_inputIdx(0),
    m_maxValuePerStage(m_numStages),
    m_scaleExponent(0),
    m_inverse(inverse)
{
    //N must be power of two, use subtract and bit-compare trick
    assert((N != 0) && !(N & (N - 1)));

    for (size_t i = 0; i < N; i++) {
        //Calculate twiddle factors
        double theta = (-2 * M_PI * i) / N;

        if (m_inverse) {
            m_twiddleFactors[i] = FixedComplexNorm16(cos(theta), -sin(theta));
        } else {
            m_twiddleFactors[i] = FixedComplexNorm16(cos(theta), sin(theta));
        }
    }
}

bool BFPFFT::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_INT32_COMPLEX);
    size_t N = m_inputs.capacity();
    //using a bit-reversed index to decimate in time
    size_t reverseIdx = reverseBits(N, m_inputIdx++);

    //XXX can we just set the underlying integer value directly instead of going through a double?
    m_inputs[reverseIdx].real(static_cast<double>(data.intc.real()) / SCALE_FACTOR);
    m_inputs[reverseIdx].imag(static_cast<double>(data.intc.imag()) / SCALE_FACTOR);

    //Track the maximum input value for proper scaling
    updateMaxValueForStage(1, m_inputs[reverseIdx]);

    return true;
}

bool BFPFFT::output(filter_io_t &data)
{
    if (m_outputValid == true) {
        data.type = IO_TYPE_INT32_COMPLEX;
        data.intc = m_outputs[m_outputIdx++];
        if (m_outputIdx >= m_outputs.size()) {
            m_outputIdx = 0;
        }
        return true;
    }
    return false;
}

void BFPFFT::tick(void)
{
    if (m_inputIdx != m_inputs.capacity()) {
        return;
    }

    //We're good to go...
    dit();

    for (size_t i = 0; i < m_inputs.size(); i++) {
        m_outputs[i].real(m_inputs[i].real().range().to_int64());
        m_outputs[i].imag(m_inputs[i].imag().range().to_int64());
    }

    //For IIFT we need to divide results by N (e.g. shift right by log2(N) = numStages)
    //We keep track of the exponent, even though we're not doing with it right now
    if (m_inverse) {
        m_scaleExponent -= m_numStages;
    }

    //Reset some internal state for the next round
    m_inputIdx = 0;
    std::fill(m_maxValuePerStage.begin(), m_maxValuePerStage.end(), 0);
    m_scaleExponent = 0;

    m_outputValid = true;
    assert(m_outputIdx == 0);
}

void BFPFFT::dit()
{
    size_t N = m_inputs.size() >> 1; //number of butterflies per block
    size_t numBlocks = 1; //number of blocks per stage
    for (size_t stage = 1; stage <= m_numStages; stage++) { //stage loop
        //Calculate shift amount based on maximum value per stage and then shift the values
        ssize_t shiftAmount = calculateShiftAmountForStage(stage);
        shiftStage(shiftAmount);
//        std::cout << "stage " << stage << " shifted by " << shiftAmount << std::endl;
        //Track the accumulated scale amount so that we can scale back at the end
        m_scaleExponent += shiftAmount;
        for (size_t block = 0; block < numBlocks; block++) { //block loop
//            std::cout << "block = " << block << std::endl;
            size_t baseT = block * N * 2;
            size_t baseB = baseT + N;
            for (size_t n = 0; n < N; n++) { //butterfly loop

                size_t topIdx = reverseBits(m_inputs.size(), baseT + n);
                size_t botIdx = reverseBits(m_inputs.size(), baseB + n);

                FixedComplex1_31 twiddle = getTwiddleFactor(stage, n + baseT/2);

                //Radix-2 butterfly
                FixedComplex1_31 top = m_inputs[topIdx];
                FixedComplex1_31 bot = m_inputs[botIdx] * twiddle;
                m_inputs[topIdx] = top + bot;
                m_inputs[botIdx] = top - bot;

//                std::cout << "x(" << baseT + n << ") & x(" << baseB + n << ") * twiddle(" << n + baseT/2 << ")" << std::endl;

                //Track the max value to shift the next stage properly
                if (stage != m_numStages) {
                    updateMaxValueForStage(stage+1, m_inputs[topIdx]);
                    updateMaxValueForStage(stage+1, m_inputs[botIdx]);
                }
            } //end butterfly loop
        } //end block loop
        N >>= 1;
        numBlocks <<= 1;
    } //end stage loop
}

template <typename COMPLEX_T>
void BFPFFT::shiftFixedComplex(COMPLEX_T &val, ssize_t shiftBits)
{
    if (shiftBits == 0) {
        return;
    }
    bool leftShift = (shiftBits < 0); //left shift is negative
    shiftBits = abs(shiftBits);

    //Found that we must use an intermediate value here in order for the shifting to work properly. Unsure as of why exactly.
    sc_fix temp(val.real().wl(), val.real().iwl());

    temp = val.real();
    if (leftShift) {
        temp <<= shiftBits;
    } else {
        temp >>= shiftBits;
    }
    val.real(temp);

    temp = val.imag();
    if (leftShift) {
        temp <<= shiftBits;
    } else {
        temp >>= shiftBits;
    }
    val.imag(temp);
}

void BFPFFT::shiftOutput(ssize_t shiftAmount)
{
    for (size_t i = 0; i < m_outputs.size(); i++) {
        int32_t real = m_outputs[i].real();
        int32_t imag = m_outputs[i].imag();
        if (shiftAmount < 0) {
            real <<= static_cast<size_t>(abs(shiftAmount));
            imag <<= static_cast<size_t>(abs(shiftAmount));
        } else {
            real >>= shiftAmount;
            imag >>= shiftAmount;
        }
        m_outputs[i].real(real);
        m_outputs[i].imag(imag);
    }
}

void BFPFFT::shiftStage(ssize_t shiftAmount)
{
    for (size_t i = 0; i < m_inputs.size(); i++) {
        shiftFixedComplex<FixedComplex1_31>(m_inputs[i], shiftAmount);
    }
}

ssize_t BFPFFT::calculateShiftAmountForStage(size_t stage)
{
    ssize_t shiftCount = 0;
    size_t upper, lower;
    uint64_t value = m_maxValuePerStage[stage - 1];
    if (value == 0) {
        return shiftCount;
    }
    //From the analysis in the T.I. document (linked at top),
    //the first two stages have the potential to grow by one bit after
    //a radix-2 operation. The rest of the stages have the potential to
    //grow by 2 bits. Set the upper and lower thresholds to know how
    //much to shift to accommodate the potential growth
    if (stage == 1 || stage == 2) {
        upper = SCALE_FACTOR >> 1;
        lower = SCALE_FACTOR >> 2;
    } else {
        upper = SCALE_FACTOR >> 2;
        lower = SCALE_FACTOR >> 3;
    }
    while (value >= upper || value < lower) {
        if (value >= upper) {
            value >>= 1;
            shiftCount += 1; //right shift is positive
        } else {
            value <<= 1;
            shiftCount -= 1; //left shift is negative
        }
    }
    return shiftCount;
}

void BFPFFT::updateMaxValueForStage(size_t stage, const FixedComplex &val)
{
    int64_t rawValue = val.real().range().to_int64();
    if (static_cast<uint64_t>(abs(rawValue)) > m_maxValuePerStage[stage - 1]) {
        m_maxValuePerStage[stage - 1] = abs(rawValue);
    }
    rawValue = val.imag().range().to_int64();
    if (static_cast<uint64_t>(abs(rawValue)) > m_maxValuePerStage[stage - 1]) {
        m_maxValuePerStage[stage - 1] = abs(rawValue);
    }
}

FixedComplexNorm16 BFPFFT::getTwiddleFactor(size_t stage, size_t n) const
{
    //see https://www.dsprelated.com/showarticle/107.php
    size_t N = m_inputs.size();
    size_t stageSize = (1 << stage);
    size_t A1 = (stageSize * n) / N;
    int temp = m_numStages - 1;
    A1 = reverseBits(1 << temp, A1);
    return m_twiddleFactors[A1];
}
