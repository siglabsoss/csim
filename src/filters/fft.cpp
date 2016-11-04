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

#define FFT_DO_DECIMATE_IN_FREQUENCY

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
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
    size_t inputIdx = m_inputIdx++;
#else
    //using a bit-reversed index to decimate in time
    size_t N = m_inputs.capacity();
    size_t inputIdx = utils::reverseBits(N, m_inputIdx++);
#endif

    //Rely on upstream to use expected input format so that
    //this module can be slightly more flexible
    m_inputs[inputIdx].setFormat(data.fc);

    SLFixPoint::throwOnOverflow = true;
    m_inputs[inputIdx] = data.fc;
    SLFixPoint::throwOnOverflow = false;

    //Track the maximum input value for proper scaling
    updateMaxValueForStage(1, m_inputs[inputIdx]);

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
    if (m_inputIdx != m_inputs.size()) {
        return;
    }

    //We're good to go...
    execute();

    for (size_t i = 0; i < m_inputs.size(); i++) {
        //For IIFT we need to divide results by N (e.g. shift right by log2(N) = numStages)
        if (m_inverse) {
            m_inputs[i].shiftRadixRight(m_numStages);
        }

        //XXX eventually remove throwOnOverflow here because we will actually
        //allow clipping (saturation) as long as it's below an acceptable rate
        SLFixPoint::throwOnOverflow = true;
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
        size_t outIdx = utils::reverseBits(m_inputs.size(), i);
#else
        size_t outIdx = i;
#endif

        m_outputs[outIdx].setFormat(FFT_OUTPUT_FORMAT);
        m_outputs[outIdx] = m_inputs[i];
        SLFixPoint::throwOnOverflow = false;
    }
//    std::cout << "Output format is " << m_outputs[0].wl() << "," << m_outputs[0].iwl() << std::endl;

    //Reset some internal state for the next round
    m_inputIdx = 0;
    std::fill(m_maxValuePerStage.begin(), m_maxValuePerStage.end(), 0);
    m_scaleExp = 0;

    m_outputValid = true;
    assert(m_outputIdx == 0);
}

void FFT::execute()
{
    /**
     * Block floating point scaling operates on a per-stage basis by executing the following steps
     * 1) Calculate the appropriate shift for the entire "block" (inputs for the current stage) based on the single maximum input value.
     * 2) Shift the "block" based on the scaling factor from step 1.
     * 3) Perform the radix-2 butterflies
     */
    size_t N = m_inputs.size() >> 1; //number of butterflies per block
    size_t numBlocks = 1; //number of blocks in the current stage
    for (size_t stage = 1; stage <= m_numStages; stage++) { //stage loop
        //Step 1 - Calculate shift amount
        ssize_t shiftAmount = calculateShiftAmountForStage(stage);

#ifdef FFT_DO_BLOCK_FLOATING_POINT
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

#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
                size_t topIdx = baseT + n;//utils::reverseBits(m_inputs.size(), baseT + n);
                size_t botIdx = baseB + n;//utils::reverseBits(m_inputs.size(), baseB + n);
#else
                size_t topIdx = utils::reverseBits(m_inputs.size(), baseT + n);
                size_t botIdx = utils::reverseBits(m_inputs.size(), baseB + n);
#endif

//                std::cout << topIdx << " & " << botIdx << std::endl;
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
                size_t k = n;
#else
                size_t k = n + block * N;
#endif
                SLFixComplex twiddle = getTwiddleFactor(stage, k);

                //Step 3) Perform the Radix-2 butterflies
                SLFixComplex top = m_inputs[topIdx];
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
                SLFixComplex bot = m_inputs[botIdx];
#else
                SLFixComplex bot = m_inputs[botIdx] * twiddle;
#endif


#ifndef FFT_DO_BLOCK_FLOATING_POINT
                //We'll simulate growth of the buffer width at each stage by adjusting
                //the fixed point format when we're not doing block floating point.
                size_t wordLength = m_inputs[topIdx].wl();
                size_t intWordLength = m_inputs[topIdx].iwl();

                //Instead of blindly growing the width of each stage to ensure no overflow, reuse the calcShiftAmount
                //routine from BFP, to understand whether growth is possible or not. This will help keep us under
                //SLFixPoint's 64-bit limit without impacting results
                wordLength    += shiftAmount;
                intWordLength += shiftAmount;
                m_inputs[topIdx].setFormat(wordLength, intWordLength);


                wordLength = m_inputs[botIdx].wl();
                intWordLength = m_inputs[botIdx].iwl();
                wordLength += shiftAmount;
                intWordLength += shiftAmount;
                m_inputs[botIdx].setFormat(wordLength, intWordLength);
#endif
                SLFixPoint::throwOnOverflow = true;
                m_inputs[topIdx] = top + bot;
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
                m_inputs[botIdx] = (top - bot) * twiddle;
#else
                m_inputs[botIdx] = top - bot;
#endif
                SLFixPoint::throwOnOverflow = false;

                //Track the max value to shift the next stage properly
                if (stage != m_numStages) {
                    updateMaxValueForStage(stage+1, m_inputs[topIdx]);
                    updateMaxValueForStage(stage+1, m_inputs[botIdx]);
                }
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

    //Using runtime word length so that this routine returns correct results even
    //if the fixed point format of the stage inputs is changing, which is the case
    //if BFP is turned off
    size_t wordLength = m_inputs[0].wl();

    if (stage == 1 || stage == 2) {
        upper = (1ull << (wordLength - 2)) >> 1ull;
        lower = (1ull << (wordLength - 2)) >> 2ull;
    } else {
        upper = (1ull << (wordLength - 2)) >> 2ull;
        lower = (1ull << (wordLength - 2)) >> 3ull;
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
    size_t stageSize = (1 << stage);
#ifdef FFT_DO_DECIMATE_IN_FREQUENCY
    size_t A1 = (stageSize * n) / 2;
#else
    size_t N = m_inputs.size();
    size_t A1 = (stageSize * n) / N;
    int temp = m_numStages - 1;
    A1 = utils::reverseBits(1 << temp, A1);
#endif
//    std::cout << "stage " << stage << " twiddle: " << n << " -> " << A1 << std::endl;
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
