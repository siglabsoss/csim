#include <filters/biquad.hpp>
#include <core/logger.hpp>
#include <utils/utils.hpp>
#include <cassert>

Biquad::Biquad() :
    FilterChainElement("Biquad"),
    m_x(3, FixedComplexNorm16(0.0, 0.0)),
    m_y(3, FixedComplexNorm16(0.0, 0.0)),
    m_b(3),
    m_a(2),
    m_newInput(false)
{

}

void Biquad::init(double b0,
        double b1,
        double b2,
        double a1,
        double a2)
{
    constexpr size_t BIT_WIDTH = 16;
    size_t bitShift = 0;
    m_b[0].first = utils::createDynamicFixedPoint(b0, BIT_WIDTH, bitShift);
    m_b[0].second = bitShift;

    m_b[1].first = utils::createDynamicFixedPoint(b1, BIT_WIDTH, bitShift);
    m_b[1].second = bitShift;

    m_b[2].first = utils::createDynamicFixedPoint(b2, BIT_WIDTH, bitShift);
    m_b[2].second = bitShift;

    m_a[0].first = utils::createDynamicFixedPoint(a1, BIT_WIDTH, bitShift);
    m_a[0].second = bitShift;

    m_a[1].first = utils::createDynamicFixedPoint(a2, BIT_WIDTH, bitShift);
    m_a[1].second = bitShift;

    double b[3] = {b0, b1, b2};
    double a[2] = {a1, a2};
    for (size_t i = 0; i < m_b.size(); i++) {
        log_debug("b%d (Q%d.%d)  = %f (%f when scaled by 2^%d and quantized)", i, m_b[i].first->iwl(), m_b[i].first->wl() - m_b[i].first->iwl(), b[i], m_b[i].first->to_double(), m_b[i].second);
    }
    for (size_t i = 0; i < m_a.size(); i++) {
        log_debug("a%d (Q%d.%d)  = %f (%f when scaled by 2^%d and quantized)", i, m_a[i].first->iwl(), m_a[i].first->wl() - m_a[i].first->iwl(), a[i], m_a[i].first->to_double(), m_a[i].second);
    }
}

bool Biquad::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX);
    //shift inputs to make room for new sample
    size_t size = m_x.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_x[size - i - 1] = m_x[size - i - 2];
    }
    m_x[0] = data.fc;
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
     * PHASE 1 - Intermediate value (XXX this can be done on initialization)
     * Construct intermediate fixed point values to store the result of the five multiplication operations. Each intermediate result
     * will have the same total bit width, but may be scaled differently (i.e. fixed point position may be different). This is determined
     * by analyzing the scaling of each coefficient
     */


    size_t coeffWordWidth = 0;
    size_t coeffIntWidth  = 0;
    size_t resultWidth = 0;
    size_t resultIntWidth = 0;

    //input widths will be the same, so grab just from the first
    size_t inputWordWidth = m_x[0].real().wl();
    size_t inputIntWidth  = m_x[0].real().iwl();

    coeffWordWidth  = m_b[0].first->wl();
    coeffIntWidth   = m_b[0].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth - 1;
    std::complex<sc_fix> bx0(sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP));
    //std::cout << "bx0 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[0].second << std::endl;

    coeffWordWidth  = m_b[1].first->wl();
    coeffIntWidth   = m_b[1].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth - 1;
    std::complex<sc_fix> bx1(sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP));
    //std::cout << "bx1 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[1].second << std::endl;

    coeffWordWidth  = m_b[2].first->wl();
    coeffIntWidth   = m_b[2].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth - 1;
    std::complex<sc_fix> bx2(sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP));
    //std::cout << "bx2 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[2].second << std::endl;

    coeffWordWidth  = m_a[0].first->wl();
    coeffIntWidth   = m_a[0].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth - 1;
    std::complex<sc_fix> ay1(sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP));
    //std::cout << "ay1 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_a[0].second << std::endl;

    coeffWordWidth  = m_a[1].first->wl();
    coeffIntWidth   = m_a[1].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth - 1;
    std::complex<sc_fix> ay2(sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_WRAP));
    //std::cout << "ay2 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_a[1].second << std::endl;

    /**
     * PHASE 2 - Multiplication
     * Perform the 3 feedback (b coeff) multiplications and the 2 feedforward (a coeff) and store
     * the results in the 5 intermediate values.
     */

    std::complex<sc_fix> x0(sc_fix(16, 1, SC_RND, SC_WRAP), sc_fix(16, 1, SC_RND, SC_WRAP));
    std::complex<sc_fix> x1(sc_fix(16, 1, SC_RND, SC_WRAP), sc_fix(16, 1, SC_RND, SC_WRAP));
    std::complex<sc_fix> x2(sc_fix(16, 1, SC_RND, SC_WRAP), sc_fix(16, 1, SC_RND, SC_WRAP));
    std::complex<sc_fix> y1(sc_fix(16, 1, SC_RND, SC_WRAP), sc_fix(16, 1, SC_RND, SC_WRAP));
    std::complex<sc_fix> y2(sc_fix(16, 1, SC_RND, SC_WRAP), sc_fix(16, 1, SC_RND, SC_WRAP));

    x0 = m_x[0];
    x1 = m_x[1];
    x2 = m_x[2];
    y1 = m_y[1];
    y2 = m_x[2];

    //bx0 = (*(m_b[0].first) * x0);
    std::cout << "bx0 (Q" << bx0.real().iwl() << "." << bx0.real().wl() - bx0.real().iwl() << ") = " << bx0 << " (" << *(m_b[0].first) << " * " << x0 << ")" << std::endl;
    complexScalarMultiply(bx0, x0, *(m_b[0].first));
    shiftRightFixedComplex(bx0, m_b[0].second);
    std::cout << "bx0 (Q" << bx0.real().iwl() << "." << bx0.real().wl() - bx0.real().iwl() << ") = " << bx0 << " (" << *(m_b[0].first) << " * " << x0 << ")" << std::endl;

    bx1 = (*(m_b[1].first) * x1);
    shiftRightFixedComplex(bx1, m_b[1].second);
    std::cout << "bx1 = " << bx1 << std::endl;

    bx2 = (*(m_b[2].first) * x2);
    shiftRightFixedComplex(bx2, m_b[2].second);
    std::cout << "bx2 = " << bx2 << std::endl;

    ay1 = (*(m_a[0].first) * y1);
    shiftRightFixedComplex(ay1, m_a[0].second);
    std::cout << "ay1 = " << ay1 << std::endl;

    ay2 = (*(m_a[1].first) * y2);
    shiftRightFixedComplex(ay2, m_a[1].second);
    std::cout << "ay2 = " << ay2 << std::endl;

    /**
     * PHASE 3 - Addition
     * Add up the 5 intermediate values, which are all double width and store them
     * in the single width result (this can/will cause loss of precision, but what about overflow?)
     */
    m_y[0] = bx0 + bx1 + bx2 + ay1 + ay2;
}

void Biquad::complexScalarMultiply(std::complex<sc_fix> &result, std::complex<sc_fix> &complex, sc_fix &scalar)
{
    sc_fix tempResultReal(result.real().wl(), result.real().iwl());
    sc_fix tempResultImag(result.imag().wl(), result.imag().iwl());

    tempResultReal = complex.real() * scalar;
    tempResultImag = complex.imag() * scalar;

    std::cout << "tempResultReal " << tempResultReal << std::endl;
    std::cout << "tempResultImag " << tempResultReal << std::endl;

    result.real(tempResultReal);
    result.imag(tempResultImag);
}

void Biquad::shiftRightFixedComplex(std::complex<sc_fix> &val, size_t shiftBits)
{
    if (shiftBits == 0) {
        return;
    }
    //std::cout << "val (Q" << val.real().iwl() << "." << val.real().wl() - val.real().iwl() << ") = " << val.real() << " (" << val.real().range().to_int64() << ")" << std::endl;
    //std::cout << "temp (Q" << temp.iwl() << "." << temp.wl() - temp.iwl() << ") = " << temp << " (" << temp.range().to_int64() << ")" << std::endl;
    //Found that we must use an intermediate value here in order for the shifting to work properly. Unsure as of why exactly.
    sc_fix temp(val.real().wl(), val.real().iwl());
    temp = val.real();
    temp >>= shiftBits;
    val.real(temp);
    //std::cout << "val (Q" << val.real().iwl() << "." << val.real().wl() - val.real().iwl() << ") = " << val.real() << " (" << val.real().range().to_int64() << ")" << std::endl;
    temp = val.imag();
    temp >>= shiftBits;
    val.imag(temp);
}
