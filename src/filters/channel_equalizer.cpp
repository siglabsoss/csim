
#include <filters/channel_equalizer.hpp>
#include <cassert>

ChannelEqualizer::ChannelEqualizer(const std::vector<ComplexDouble>& Hf) :
    FilterChainElement("CHANNEL_EQ"),
    m_Hf(Hf),
    m_sample(),
    m_outIdx(0),
    gotInput(false)
{
    assert(m_Hf.size() == 1024);
}

bool ChannelEqualizer::input(const filter_io_t& data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    gotInput = true;
    m_sample.setFormat(data.fc.getFormat());
    m_sample = data.fc;
    return true;
}

bool ChannelEqualizer::output(filter_io_t& data)
{
    if (gotInput) {
        gotInput = false;
        ComplexDouble result = m_sample.toComplexDouble() / m_Hf[m_outIdx++];

        if (m_outIdx == m_Hf.size()) {
            m_outIdx = 0;
        }
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(m_sample.getFormat());
        data.fc.set(result.real(), result.imag());
        return true;
    }
    return false;
}

void ChannelEqualizer::tick()
{}
