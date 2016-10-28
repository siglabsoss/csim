/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 *
 * This implementation was inspired by the non-recursive method outlined here: http://www.engineeringproductivitytools.com/stuff/T0001/PT04.HTM#Head208
 *
 * The "block floating point" technique was inspired by the paper here: http://www.ti.com/lit/an/spra948/spra948.pdf
 *
 */

#include <filters/fft.hpp>
#include <utils/utils.hpp>

#define FFT_DO_BLOCK_FLOATING_POINT

FFT::FFT(size_t N, bool inverse) :
    FilterChainElement(std::string("FFT")),
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

bool FFT::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    size_t N = m_inputs.capacity();
    //using a bit-reversed index to decimate in time
    size_t reverseIdx = utils::reverseBits(N, m_inputIdx++);

    m_inputs[reverseIdx].setFormat(data.fc);
    SLFixPoint::throwOnOverflow = true;
    m_inputs[reverseIdx] = data.fc;
    SLFixPoint::throwOnOverflow = false;

#ifdef FFT_DO_BLOCK_FLOATING_POINT
    //Track the maximum input value for proper scaling
    updateMaxValueForStage(1, m_inputs[reverseIdx]);
#endif

    return true;
}

bool FFT::output(filter_io_t &data)
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

void FFT::tick(void)
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
    std::cout << "Output format is " << m_outputs[0].wl() << "," << m_outputs[0].iwl() << std::endl;

    //Reset some internal state for the next round
    m_inputIdx = 0;
    std::fill(m_maxValuePerStage.begin(), m_maxValuePerStage.end(), 0);
    m_scaleExp = 0;

    m_outputValid = true;
    assert(m_outputIdx == 0);
}

void FFT::dit()
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
#ifdef FFT_DO_BLOCK_FLOATING_POINT
        //Step 1 - Calculate shift amount
        ssize_t shiftAmount = calculateShiftAmountForStage(stage);
        //Step 2 - Shift the inputs
        shiftStage(shiftAmount);
        //std::cout << "stage " << stage << " shifted by " << shiftAmount << std::endl;
        //Track the accumulated scale amount so that we can scale back at the end
        m_scaleExp += shiftAmount;
#endif
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
#ifndef FFT_DO_BLOCK_FLOATING_POINT
                //We'll simulate growth of the buffer width at each stage by adjusting
                //the fixed point format when we're not doing block floating point.
                size_t wordLength = m_inputs[topIdx].wl();
                size_t intWordLength = m_inputs[topIdx].iwl();

                if (stage == 1) {
                    ++wordLength;
                    ++intWordLength;
                } else {
                    wordLength += 1;
                    intWordLength += 1;
                }

                m_inputs[topIdx].setFormat(wordLength, intWordLength);
                m_inputs[botIdx].setFormat(wordLength, intWordLength);
#endif
                SLFixPoint::throwOnOverflow = true;
                m_inputs[topIdx] = top + bot;
                m_inputs[botIdx] = top - bot;
                SLFixPoint::throwOnOverflow = false;

#ifdef FFT_DO_BLOCK_FLOATING_POINT
                //Track the max value to shift the next stage properly
                if (stage != m_numStages) {
                    updateMaxValueForStage(stage+1, m_inputs[topIdx]);
                    updateMaxValueForStage(stage+1, m_inputs[botIdx]);
                }
#endif
            } //end butterfly loop
        } //end block loop
        N >>= 1;
        numBlocks <<= 1;
//        std::cout << "Stage " << stage << " format is " << m_inputs[0].wl() << "," << m_inputs[0].iwl() << std::endl;
    } //end stage loop
}

void FFT::shiftFixedComplex(SLFixComplex &val, ssize_t shiftBits)
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

void FFT::shiftStage(ssize_t shiftAmount)
{
    for (size_t i = 0; i < m_inputs.size(); i++) {
        shiftFixedComplex(m_inputs[i], shiftAmount);
    }
}

ssize_t FFT::calculateShiftAmountForStage(size_t stage)
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
        upper = (1 << (FFT_INPUT_WL - FFT_INPUT_IWL)) >> 1;
        lower = (1 << (FFT_INPUT_WL - FFT_INPUT_IWL)) >> 2;
    } else {
        upper = (1 << (FFT_INPUT_WL - FFT_INPUT_IWL)) >> 2;
        lower = (1 << (FFT_INPUT_WL - FFT_INPUT_IWL)) >> 3;
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

void FFT::updateMaxValueForStage(size_t stage, const SLFixComplex &val)
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

SLFixComplex FFT::getTwiddleFactor(size_t stage, size_t n) const
{
    //see https://www.dsprelated.com/showarticle/107.php
    size_t N = m_inputs.size();
    size_t stageSize = (1 << stage);
    size_t A1 = (stageSize * n) / N;
    int temp = m_numStages - 1;
    A1 = utils::reverseBits(1 << temp, A1);
    return m_twiddleFactors[A1];
}

void FFT::printTwiddleFactors() const
{
    size_t N = m_twiddleFactors.size();
    std::cout << "theta, real (scaled), imag (scaled), real, imag" << std::endl;
    for (size_t i = 0; i < m_twiddleFactors.size(); i++) {
        double theta = (-2 * M_PI * i) / N;
        std::cout << theta << ", " << m_twiddleFactors[i].real().to_uint64() << ", " << m_twiddleFactors[i].imag().to_uint64() << ", " << m_twiddleFactors[i].real().to_double() << ", " << m_twiddleFactors[i].imag().to_double() << std::endl;
    }
}