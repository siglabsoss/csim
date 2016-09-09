#include <filters/bfp_fft.hpp>
#include <utils/utils.hpp>

BFPFFT::BFPFFT(size_t N) :
    FilterChainElement(std::string("BFPFFT")),
    m_inputs(N),
    m_outputs(N),
    m_twiddleFactors(N),
    m_outputValid(false),
    m_outputIdx(0),
    m_inputIdx(0)

{
    //N must be power of two, use subtract and bit-compare trick
    assert((N != 0) && !(N & (N - 1)));

    for (size_t i = 0; i < N; i++) {
        double theta = (-2 * M_PI * i) / N;
        m_twiddleFactors[i] = ComplexDouble(cos(theta), sin(theta));
    }
}

bool BFPFFT::input(const filter_io_t &data)
{
    //assert(data.type == IO_TYPE_FIXED_COMPLEX);
    size_t N = m_inputs.capacity();
    //using a bit-reversed index to decimate in time
    size_t reverseIdx = reverseBits(N, m_inputIdx++);
    m_inputs[reverseIdx] = data.toComplexDouble(); //XXX just for testing
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
    dit(0, m_inputs.size(), 1);
    m_inputIdx = 0;

    //Double-buffer to accept more inputs as outputs are pushed out
    m_outputs = m_inputs;

    m_outputValid = true;
    assert(m_outputIdx == 0);
}

void BFPFFT::dit(size_t baseT, size_t N, size_t stage)
{
    if (N == 1) {
        return;
    }
    N >>= 1;
    size_t baseB = baseT + N;
    for (size_t n = 0; n < N; n++) {
        ComplexDouble twiddle = getTwiddleFactor(stage, n + baseT/2);
        size_t topIdx = reverseBits(m_inputs.size(), baseT + n);
        size_t botIdx = reverseBits(m_inputs.size(), baseB + n);

        ComplexDouble top = m_inputs[topIdx];
        ComplexDouble bot = m_inputs[botIdx] * twiddle;
        //std::cout << "stage " << stage << " x(" << topIdx << + ") & x(" << botIdx << ")" << std::endl;
        m_inputs[topIdx] = top + bot;
        m_inputs[botIdx] = top - bot;
    }
    dit(baseT, N, stage+1);
    dit(baseB, N, stage+1);
}

ComplexDouble BFPFFT::getTwiddleFactor(size_t stage, size_t n) const
{
    size_t N = m_inputs.size();
    size_t stageSize = (1 << stage);
    size_t A1 = (stageSize * n) / N;
    int temp = static_cast<int>(log2(N)) - 1;
    A1 = reverseBits(1 << temp, A1);
    return m_twiddleFactors[A1];
}
