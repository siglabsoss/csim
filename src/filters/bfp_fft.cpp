/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 *
 * This implementation was inspired by the non-recursive method outlined here: http://www.engineeringproductivitytools.com/stuff/T0001/PT04.HTM#Head208
 *
 * The "block floating point" technique was inspired by the paper here: http://www.ti.com/lit/an/spra948/spra948.pdf
 *
 */

#include <filters/bfp_fft.hpp>
#include <utils/utils.hpp>

//NOTE: This is meant to match the input bit width (32-bits)
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
    m_scaleExp(0),
    m_inverse(inverse)
{
    //N must be power of two, use subtract and bit-compare trick
    assert((N != 0) && !(N & (N - 1)));

    for (size_t i = 0; i < N; i++) {
        //Calculate twiddle factors
        double theta = (-2 * M_PI * i) / N;
        m_twiddleFactors[i].setFormat(FFT_TWIDDLE_FORMAT);
        m_twiddleFactors[i].real(cos(theta));
        if (m_inverse) {
            m_twiddleFactors[i].imag(-sin(theta));
        } else {
            m_twiddleFactors[i].imag(sin(theta));
        }
    }
}

bool BFPFFT::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    size_t N = m_inputs.capacity();
    //using a bit-reversed index to decimate in time
    size_t reverseIdx = utils::reverseBits(N, m_inputIdx++);

    m_inputs[reverseIdx].setFormat(data.fc);
    m_inputs[reverseIdx] = data.fc;

    //Track the maximum input value for proper scaling
    updateMaxValueForStage(1, m_inputs[reverseIdx]);

    return true;
}

bool BFPFFT::output(filter_io_t &data)
{
    if (m_outputValid == true) {
        data = m_outputs[m_outputIdx++];
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
        //For IIFT we need to divide results by N (e.g. shift right by log2(N) = numStages)
        if (m_inverse) {
            m_inputs[i].shiftRadixRight(m_numStages);
        }
        m_outputs[i].setFormat(m_inputs[i]);
        m_outputs[i] = m_inputs[i];
    }

    //Reset some internal state for the next round
    m_inputIdx = 0;
    std::fill(m_maxValuePerStage.begin(), m_maxValuePerStage.end(), 0);
    m_scaleExp = 0;

    m_outputValid = true;
    assert(m_outputIdx == 0);
}

void BFPFFT::dit()
{
    /**
     * Block floating point scaling operates on a per-stage basis by executing the following steps
     * 1) Calculate the appropriate shift for the entire "block" (inputs for the current stage) based on the single maximum input value.
     * 2) Shift the "block" based on the scaling factor from step 1.
     * 3) Perform the radix-2 butterflies
     */
    size_t N = m_inputs.size() >> 1; //number of butterflies per block
    size_t numBlocks = 1; //number of blocks per stage
    for (size_t stage = 1; stage <= m_numStages; stage++) { //stage loop
        //Step 1 - Calculate shift amount
        ssize_t shiftAmount = calculateShiftAmountForStage(stage);
        //Step 2 - Shift the inputs
        shiftStage(shiftAmount);
        //std::cout << "stage " << stage << " shifted by " << shiftAmount << std::endl;
        //Track the accumulated scale amount so that we can scale back at the end
        m_scaleExp += shiftAmount;
        for (size_t block = 0; block < numBlocks; block++) { //block loop
            size_t baseT = block * N * 2;
            size_t baseB = baseT + N;
            for (size_t n = 0; n < N; n++) { //butterfly loop

                size_t topIdx = utils::reverseBits(m_inputs.size(), baseT + n);
                size_t botIdx = utils::reverseBits(m_inputs.size(), baseB + n);

                SLFixComplex twiddle = getTwiddleFactor(stage, n + baseT/2);

                //Step 3) Perform the Radix-2 butterflies
                SLFixComplex top = m_inputs[topIdx];
                SLFixComplex bot = m_inputs[botIdx] * twiddle;
                m_inputs[topIdx] = top + bot;
                m_inputs[botIdx] = top - bot;

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

void BFPFFT::shiftFixedComplex(SLFixComplex &val, ssize_t shiftBits)
{
    if (shiftBits == 0) {
        return;
    }
    bool leftShift = (shiftBits < 0); //left shift is negative
    shiftBits = abs(shiftBits);

    if (leftShift) {
        val = val << shiftBits;
    } else {
        val = val >> shiftBits;
    }
}

void BFPFFT::shiftStage(ssize_t shiftAmount)
{
    for (size_t i = 0; i < m_inputs.size(); i++) {
        shiftFixedComplex(m_inputs[i], shiftAmount);
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

void BFPFFT::updateMaxValueForStage(size_t stage, const SLFixComplex &val)
{
    int64_t rawValue = val.real().to_int64();
    if (static_cast<uint64_t>(abs(rawValue)) > m_maxValuePerStage[stage - 1]) {
        m_maxValuePerStage[stage - 1] = abs(rawValue);
    }
    rawValue = val.imag().to_int64();
    if (static_cast<uint64_t>(abs(rawValue)) > m_maxValuePerStage[stage - 1]) {
        m_maxValuePerStage[stage - 1] = abs(rawValue);
    }
}

SLFixComplex BFPFFT::getTwiddleFactor(size_t stage, size_t n) const
{
    //see https://www.dsprelated.com/showarticle/107.php
    size_t N = m_inputs.size();
    size_t stageSize = (1 << stage);
    size_t A1 = (stageSize * n) / N;
    int temp = m_numStages - 1;
    A1 = utils::reverseBits(1 << temp, A1);
    return m_twiddleFactors[A1];
}
