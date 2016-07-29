#pragma once

#include <core/filter_chain_element.hpp>
#include <complex>
#include <cmath>

class AutomaticGain : public FilterChainElement
{
public:
    virtual ~AutomaticGain() {}
    AutomaticGain(double desiredPowerW = 10) :
        FilterChainElement("AutomaticGain"),
        m_input(),
        m_output(),
        m_gain(1.0),
        m_avgPower(0.0),
        m_firstRun(true),
        m_gotInput(false),
        m_desiredPower(desiredPowerW),
        m_count(0)
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_COMPLEX_DOUBLE);
        m_input = data.rf;
        m_gotInput = true;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        if (m_gotInput) {
            data.type = IO_TYPE_COMPLEX_DOUBLE;
            data.rf = m_output;
            m_gotInput = false;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        if (!m_gotInput) {
            return;
        }

        m_output = m_input * m_gain;

        m_count++;
        if (m_count > 100000) {
            log_debug("m_avgPower = %f \t m_output = %f\tm_gain = %f", m_avgPower, std::norm(m_output), m_gain);
            m_count = 0;
        }

        double power = pow(std::norm(m_output), 2) / 50; //XXX define 50
        if (m_firstRun) {
            m_avgPower = power;
        }else {
            m_avgPower = 0.1 * power + 0.9 * m_avgPower;
        }

        double error = m_desiredPower - m_avgPower;

        //bang bang control
        if (error > 0) {
            m_gain += 0.000001;
        } else {
            m_gain -= 0.000001;
        }
    }
private:
    std::complex<double> m_input;
    std::complex<double> m_output;
    double               m_gain;
    double               m_avgPower;
    bool                 m_firstRun;
    bool                 m_gotInput;
    double               m_desiredPower;

    size_t               m_count;
};
