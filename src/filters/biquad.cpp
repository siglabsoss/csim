#include <filters/biquad.hpp>
#include <core/logger.hpp>
#include <utils/utils.hpp>
#include <cassert>
#include <limits.h>

Biquad::Biquad(size_t coeffBitWidth) :
    FilterChainElement("Biquad"),
    m_x(3, SLFixComplex(0.0, 0.0)),
    m_y(3, SLFixComplex(0.0, 0.0)),
    m_b(3),
    m_a(2),
    m_newInput(false),
    m_coeffWidth(coeffBitWidth)
{

}

Biquad::Biquad(const Biquad &other) :
        FilterChainElement("Biquad"),
        m_x(3, SLFixComplex(0.0, 0.0)),
        m_y(3, SLFixComplex(0.0, 0.0)),
        m_b(3),
        m_a(2),
        m_newInput(false),
        m_coeffWidth(other.m_coeffWidth)
{
    //We don't actually care for the notion of "copying"
    //Biquads, so we just start with a clean slate. Default
    //copy constructor is not generated due to unique ptr members
}

void Biquad::init(const SOSCoeffs &coeffs)
{
    double b0 = coeffs.b0;
    double b1 = coeffs.b1;
    double b2 = coeffs.b2;
    double a1 = coeffs.a1;
    double a2 = coeffs.a2;

    double coeffList[5] = {b0, b1, b2, a1, a2};
    int maxIntLength = -INT_MAX;
    for (size_t i = 0; i < 5; i++) {
        int intLength = utils::getShiftAmount(coeffList[i]);
        if (intLength > maxIntLength) {
            maxIntLength = intLength;
        }
    }

    maxIntLength++; //add one for sign bit

    log_debug("Initializing biquad filter structure...");

    m_b[0].setFormat(m_coeffWidth, maxIntLength);
    m_b[0] = b0;

    m_b[1].setFormat(m_coeffWidth, maxIntLength);
    m_b[1] = b1;

    m_b[2].setFormat(m_coeffWidth, maxIntLength);
    m_b[2] = b2;

    m_a[0].setFormat(m_coeffWidth, maxIntLength);
    m_a[0] = a1;

    m_a[1].setFormat(m_coeffWidth, maxIntLength);
    m_a[1] = a2;

    for (size_t i = 0; i < m_x.size(); i++) {
        m_x[i].setFormat(16, 1);
        m_y[i].setFormat(16, 1);
    }

    log_debug("Coefficient format is Q(%d.%d)", maxIntLength, m_coeffWidth - maxIntLength);
    log_debug("Input/output format is Q(1.15)");

    size_t coeffWordWidth = m_b[0].wl();
    size_t coeffIntWidth  = m_b[0].iwl();

    //input widths will be the same, so grab just from the first
    size_t inputWordWidth = m_x[0].real().wl();
    size_t inputIntWidth  = m_x[0].real().iwl();

    size_t resultWidth     = inputWordWidth + coeffWordWidth;
    size_t resultIntWidth  = inputIntWidth + coeffIntWidth;
    //Set the fixed point format of the acuumulator based on the coefficient and input widths
    m_accum.setFormat(resultWidth, resultIntWidth);

    log_debug("Accumulator format is (Q%d.%d)", m_accum.real().iwl(), m_accum.real().wl() - m_accum.real().iwl());
}

bool Biquad::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_INT32_COMPLEX);
    //shift inputs to make room for new sample
    size_t size = m_x.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_x[size - i - 1] = m_x[size - i - 2];
    }
    m_x[0].real(data.intc.normalizedReal());
    m_x[0].imag(data.intc.normalizedImag());

    m_newInput = true;
    return true;
}

bool Biquad::output(filter_io_t &data)
{
    if (m_newInput) {
        data = m_y[0];
        m_newInput = false;
        return true;
    }
    return false;
}

void Biquad::tick(void)
{
    if (!m_newInput) {
        return;
    }

    //shift outputs to make room for new sample
    size_t size = m_y.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_y[size - i - 1] = m_y[size - i - 2];
    }

    /**
     * Multiply accumulator
     */
    //XXX revise after implementing += operator
    m_accum =           (m_x[0] * m_b[0]);
    m_accum = m_accum + (m_x[1] * m_b[1]);
    m_accum = m_accum + (m_x[2] * m_b[2]);
    m_accum = m_accum + (m_y[1] * m_a[0]);
    m_accum = m_accum + (m_y[2] * m_a[1]);

    //Store larger width accumulator in smaller width output
    m_y[0] = m_accum;
}

