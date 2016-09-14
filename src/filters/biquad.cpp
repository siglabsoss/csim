#include <filters/biquad.hpp>
#include <core/logger.hpp>
#include <utils/utils.hpp>
#include <cassert>

Biquad::Biquad(size_t coeffBitWidth) :
    FilterChainElement("Biquad"),
    m_x(3, FixedComplexNorm16(0.0, 0.0)),
    m_y(3, FixedComplexNorm16(0.0, 0.0)),
    m_b(3),
    m_a(2),
    m_newInput(false),
    m_coeffWidth(coeffBitWidth)
{

}

Biquad::Biquad(const Biquad &other) :
        FilterChainElement("Biquad"),
        m_x(3, FixedComplexNorm16(0.0, 0.0)),
        m_y(3, FixedComplexNorm16(0.0, 0.0)),
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

    log_debug("Initializing biquad filter structure...");

    ssize_t bitShift = 0;
    m_b[0].first = utils::createDynamicFixedPoint(b0, m_coeffWidth, bitShift);
    m_b[0].second = bitShift;

    m_b[1].first = utils::createDynamicFixedPoint(b1, m_coeffWidth, bitShift);
    m_b[1].second = bitShift;

    m_b[2].first = utils::createDynamicFixedPoint(b2, m_coeffWidth, bitShift);
    m_b[2].second = bitShift;

    m_a[0].first = utils::createDynamicFixedPoint(a1, m_coeffWidth, bitShift);
    m_a[0].second = bitShift;

    m_a[1].first = utils::createDynamicFixedPoint(a2, m_coeffWidth, bitShift);
    m_a[1].second = bitShift;

    double b[3] = {b0, b1, b2};
    double a[2] = {a1, a2};
    for (size_t i = 0; i < m_b.size(); i++) {
        log_debug("b%d (Q%d.%d)  = %f (%f when scaled by 2^%d and quantized)", i, m_b[i].first->iwl(), m_b[i].first->wl() - m_b[i].first->iwl(), b[i], m_b[i].first->to_double(), m_b[i].second);
    }
    for (size_t i = 0; i < m_a.size(); i++) {
        log_debug("a%d (Q%d.%d)  = %f (%f when scaled by 2^%d and quantized)", i, m_a[i].first->iwl(), m_a[i].first->wl() - m_a[i].first->iwl(), a[i], m_a[i].first->to_double(), m_a[i].second);
    }

    /**
     * Construct intermediate fixed point values to store the result of the five multiplication operations. Each intermediate result
     * will have the same total bit width, but may be scaled differently (i.e. number of integer bits will be different). This is determined
     * by analyzing the scaling of each coefficient because the input scaling is fixed.
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
    resultIntWidth  = inputIntWidth + coeffIntWidth;
    m_bx0 = std::unique_ptr<FixedComplex >(new FixedComplex (sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT)));
    log_debug("bx0 (Q%d.%d)", m_bx0->real().iwl(), m_bx0->real().wl() - m_bx0->real().iwl());
    //std::cout << "bx0 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[0].second << std::endl;

    coeffWordWidth  = m_b[1].first->wl();
    coeffIntWidth   = m_b[1].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth;
    m_bx1 = std::unique_ptr<FixedComplex >(new FixedComplex (sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT)));
    log_debug("bx1 (Q%d.%d)", m_bx1->real().iwl(), m_bx1->real().wl() - m_bx1->real().iwl());
    //std::cout << "bx1 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[1].second << std::endl;

    coeffWordWidth  = m_b[2].first->wl();
    coeffIntWidth   = m_b[2].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth;
    m_bx2 = std::unique_ptr<FixedComplex >(new FixedComplex (sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT)));
    log_debug("bx2 (Q%d.%d)", m_bx2->real().iwl(), m_bx2->real().wl() - m_bx2->real().iwl());
    //std::cout << "bx2 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_b[2].second << std::endl;

    coeffWordWidth  = m_a[0].first->wl();
    coeffIntWidth   = m_a[0].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth;
    m_ay1 = std::unique_ptr<FixedComplex >(new FixedComplex (sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT)));
    log_debug("ay1 (Q%d.%d)", m_ay1->real().iwl(), m_ay1->real().wl() - m_ay1->real().iwl());
    //std::cout << "ay1 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_a[0].second << std::endl;

    coeffWordWidth  = m_a[1].first->wl();
    coeffIntWidth   = m_a[1].first->iwl();
    resultWidth     = inputWordWidth + coeffWordWidth;
    resultIntWidth  = inputIntWidth + coeffIntWidth;
    m_ay2 = std::unique_ptr<FixedComplex >(new FixedComplex (sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT), sc_fix(resultWidth, resultIntWidth, SC_RND, SC_SAT)));
    log_debug("ay2 (Q%d.%d)", m_ay2->real().iwl(), m_ay2->real().wl() - m_ay2->real().iwl());
    //std::cout << "ay2 = Q" << resultIntWidth << "." << resultWidth - resultIntWidth << " scaled 2^" << m_a[1].second << std::endl;
}

bool Biquad::input(const filter_io_t &data)
{
    static constexpr uint32_t SCALE_FACTOR = (1 << 31);
    assert(data.type == IO_TYPE_INT32_COMPLEX);
    //shift inputs to make room for new sample
    size_t size = m_x.size();
    for (size_t i = 0; i < size - 1; i++) {
        m_x[size - i - 1] = m_x[size - i - 2];
    }
    m_x[0].real(static_cast<double>(data.intc.real()) / SCALE_FACTOR);
    m_x[0].imag(static_cast<double>(data.intc.imag()) / SCALE_FACTOR);

    m_newInput = true;
    return true;
}

bool Biquad::output(filter_io_t &data)
{
    if (m_newInput) {
        data.type = IO_TYPE_INT32_COMPLEX;
        data.intc.real(m_y[0].real().range().to_int() << 16);
        data.intc.imag(m_y[0].imag().range().to_int() << 16);
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
     * PHASE 1 - Multiplication
     * Perform the 3 feedforward (b coeff) multiplications and the 2 feedback (a coeff) and store
     * the results in the 5 intermediate values.
     */

    FixedComplex x0(sc_fix(16, 1, SC_RND, SC_SAT), sc_fix(16, 1, SC_RND, SC_SAT));
    FixedComplex x1(sc_fix(16, 1, SC_RND, SC_SAT), sc_fix(16, 1, SC_RND, SC_SAT));
    FixedComplex x2(sc_fix(16, 1, SC_RND, SC_SAT), sc_fix(16, 1, SC_RND, SC_SAT));
    FixedComplex y1(sc_fix(16, 1, SC_RND, SC_SAT), sc_fix(16, 1, SC_RND, SC_SAT));
    FixedComplex y2(sc_fix(16, 1, SC_RND, SC_SAT), sc_fix(16, 1, SC_RND, SC_SAT));

    //This conversion from sc_fixed to sc_fix is needed for the types to play nice together
    x0 = m_x[0];
    x1 = m_x[1];
    x2 = m_x[2];
    y1 = m_y[1];
    y2 = m_y[2];

    //bx0 = (*(m_b[0].first) * x0);
    complexScalarMultiply(*m_bx0, x0, *(m_b[0].first));
    shiftRightFixedComplex(*m_bx0, m_b[0].second);
//    std::cout << "bx0 (Q" << m_bx0->real().iwl() << "." << m_bx0->real().wl() - m_bx0->real().iwl() << ") = " << *m_bx0 << " (" << *(m_b[0].first) << " * " << x0 << ")" << std::endl;

    complexScalarMultiply(*m_bx1, x1, *(m_b[1].first));
    shiftRightFixedComplex(*m_bx1, m_b[1].second);
//    std::cout << "bx1 (Q" << m_bx1->real().iwl() << "." << m_bx1->real().wl() - m_bx1->real().iwl() << ") = " << *m_bx1 << " (" << *(m_b[1].first) << " * " << x1 << ")" << std::endl;

    complexScalarMultiply(*m_bx2, x2, *(m_b[2].first));
    shiftRightFixedComplex(*m_bx2, m_b[2].second);
//    std::cout << "bx2 (Q" << m_bx2->real().iwl() << "." << m_bx2->real().wl() - m_bx2->real().iwl() << ") = " << *m_bx2 << " (" << *(m_b[2].first) << " * " << x2 << ")" << std::endl;

    complexScalarMultiply(*m_ay1, y1, *(m_a[0].first));
    shiftRightFixedComplex(*m_ay1, m_a[0].second);
//    std::cout << "ay1 (Q" << m_ay1->real().iwl() << "." << m_ay1->real().wl() - m_ay1->real().iwl() << ") = " << *m_ay1 << " (" << *(m_a[0].first) << " * " << y1 << ")" << std::endl;

    complexScalarMultiply(*m_ay2, y2, *(m_a[1].first));
    shiftRightFixedComplex(*m_ay2, m_a[1].second);
//    std::cout << "ay2 (Q" << m_ay2->real().iwl() << "." << m_ay2->real().wl() - m_ay2->real().iwl() << ") = " << *m_ay2 << " (" << *(m_a[1].first) << " * " << y2 << ")" << std::endl;

    /**
     * PHASE 2 - Addition
     * Add up the 5 intermediate values, which are all double-width and store them
     * in the single width-result. This chained addition matches the ideal fixed point addition. There
     * is no loss of range or precision until the final assignment to m_y[0].
     *
     * If this must be changed away from the ideal case, the addition should be done as follows:
     * 1) Add the values starting with the ones that have the most precision (least integer bits).
     * 2) For each addition, the precision of the least precise operand must be used (if not equal) meaning the
     *    other operand must lose some precision before the addition.
     * 3) Determine if the addition is to cause overflow. If not, skip step #4.
     * 4) If yes to above, shift the values and scaling factor to get more range at the expense of precision
     * 5) You have your intermediate result, which can then be added to the next operand
     */

    m_y[0].real(m_bx0->real() + m_bx1->real() + m_bx2->real() + m_ay1->real() + m_ay2->real());
    m_y[0].imag(m_bx0->imag() + m_bx1->imag() + m_bx2->imag() + m_ay1->imag() + m_ay2->imag());

    //Below is an example of the addition of the real elements using intermediate values after each addition (leaving in case it becomes necessary to use)
//    size_t wl = std::max(m_bx0->real().wl(), m_bx1->real().wl());
//    size_t iwl = std::max(m_bx0->real().iwl(), m_bx1->real().iwl()) + 1;
//    sc_fix x0x1(wl, iwl, SC_RND, SC_SAT);
//    x0x1 = m_bx0->real() + m_bx1->real();
//
//    wl  = std::max(x0x1.wl(), m_bx2->real().wl());
//    iwl = std::max(x0x1.iwl(), m_bx2->real().iwl()) + 1;
//    sc_fix x0x1x2(wl, iwl, SC_RND, SC_SAT);
//    x0x1x2 = x0x1 + m_bx2->real();
//
//    wl  = std::max(x0x1x2.wl(), m_ay1->real().wl());
//    iwl = std::max(x0x1x2.iwl(), m_ay1->real().iwl()) + 1;
//    sc_fix x0x1x2y1(wl, iwl, SC_RND, SC_SAT);
//    x0x1x2y1 = x0x1x2 + m_ay1->real();
//
//    wl  = std::max(x0x1x2y1.wl(), m_ay2->real().wl());
//    iwl = std::max(x0x1x2y1.iwl(), m_ay2->real().iwl()) + 1;
//    sc_fix x0x1x2y1y2(wl, iwl, SC_RND, SC_SAT);
//    x0x1x2y1y2 = x0x1x2y1 + m_ay2->real();
//
//    m_y[0].real(x0x1x2y1y2);
}

void Biquad::complexScalarMultiply(FixedComplex &result, const FixedComplex &complex, const sc_fix &scalar)
{
    //For some reason not yet known, when dealing with complex<sc_fix> we must store the calculation in
    //intermediate results instead of acting on the complex variable directly. This does not seem to be necessary
    //when dealing with sc_fixed<N,M>.
    sc_fix tempResultReal(result.real().wl(), result.real().iwl());
    sc_fix tempResultImag(result.imag().wl(), result.imag().iwl());

    tempResultReal = complex.real() * scalar;
    tempResultImag = complex.imag() * scalar;

    result.real(tempResultReal);
    result.imag(tempResultImag);
}

void Biquad::shiftRightFixedComplex(FixedComplex &val, size_t shiftBits)
{
    if (shiftBits == 0) {
        return;
    }

    //Found that we must use an intermediate value here in order for the shifting to work properly. Unsure as of why exactly.
    sc_fix temp(val.real().wl(), val.real().iwl());

    temp = val.real();
    temp >>= shiftBits;
    val.real(temp);

    temp = val.imag();
    temp >>= shiftBits;
    val.imag(temp);
}
