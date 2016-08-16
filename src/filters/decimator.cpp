#include <filters/decimator.hpp>
#include <cassert>

Decimator::Decimator(size_t N, size_t O) :
    m_N(N),
    m_O(O),
    m_inputCount(0),
    m_outputReady(false)
{
    assert(N > O);
}

bool Decimator::input(const filter_io_t &data)
{
    if (m_inputCount + (m_N - m_O) == m_N) {
        m_value = data;
        m_outputReady = true;
    }
    m_inputCount++;
    if (m_inputCount >= m_N) {
        m_inputCount = 0;
    }
    return true;
}

bool Decimator::output(filter_io_t &data)
{
    if (m_outputReady == true) {
        m_outputReady = false;
        data = m_value;
        return true;
    }
    return false;
}

void Decimator::tick(void)
{

}
