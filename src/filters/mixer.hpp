#pragma once

#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

class Mixer : public FilterChainElement
{
public:
    virtual ~Mixer() {}
    Mixer(uint32_t fs) :
        FilterChainElement(std::string("Mixer")),
        m_fs(fs),
        m_count(0)
    {
        m_output.type = IO_TYPE_COMPLEX_DOUBLE;
    }
    bool input(const filter_io_t &data) override
    {
    	m_hold = data;
    	m_vec.push_back(data.rf);
//        cout << data.rf << endl;
        return true;
    }

    bool output(filter_io_t &data) override
    {
    	static bool oneshot = true;
    	if(oneshot && m_count==1024)
    	{
    		oneshot = false;
    		plotter *P = new plotter();

    		std::vector<int> myvec;
    		myvec.push_back(1);
    		myvec.push_back(2);

//    		P->nplot(myvec, "int");

    		std::vector<FixedComplex<32> > fcvec;
    		fcvec.push_back(FixedComplex<32>(1,0));
    		fcvec.push_back(FixedComplex<32>(2,0));
    		fcvec.push_back(FixedComplex<32>(10,0));

//    		P->nplot(fcvec, "FixedComplex<32>");





    		std::vector<std::complex<double> > db;
    		db.push_back(std::complex<double>(0,0.1));
    		db.push_back(std::complex<double>(0,0.2));
    		db.push_back(std::complex<double>(0,0.13));
//
    		P->nplotqam(db, "std::complex<double>");


    	}
        data = m_output.rf * m_hold.rf;
        return true;
    }

    void tick(void) override
    {
        //using 10 for 10V, which is the peak voltage for a 1W / 30dBm sine wave
//        double amplitude = 1.0;
        double theta = (((2 * M_PI) / m_fs) * m_count);
        m_output.rf = std::complex<double>(cos(theta), sin(theta));
        m_count++;
        if (m_count > m_fs) {
            m_count = 0;
        }
    }
private:
//    double m_ticksPerPeriod;
    uint32_t m_fs;
    filter_io_t m_output;
    filter_io_t m_hold;
    size_t m_count;
    std::vector<std::complex<double> > m_vec;

};


