/*
 * fixediir.h
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#ifndef FIXEDIIR_H_
#define FIXEDIIR_H_

#include <types/fixedcomplex.hpp>
#include <core/filter_chain_element.hpp>

//Note that a[0] must be 1

class fixediir : public FilterChainElement
{
public:

    bool input(const filter_io_t &data) override;
       /**
        * output - provide an output sample to the caller.
        * @return false if no output sample is available.
        */
    bool output(filter_io_t &data) override;

    void tick() override;

    fixediir(int registerXSize, int registerYSize, FixedComplex2<16, 1>* aCoeffs,
               FixedComplex2<16, 1>* bCoeffs); //Constructor
    void iir(FixedComplex2<16, 1> &input); //Filtering input data
    void reset(); //resets registers
    FixedComplex2<16, 1> calculate(FixedComplex2<16, 1> current); // Calculate output based on input

    int                         m_numXRegisters;
    int                         m_numYRegisters;
    std::vector<FixedComplex2<16, 1> >   m_a; //a coefficients
    std::vector<FixedComplex2<16, 1> >   m_b; //b coefficients
    std::vector<FixedComplex2<16, 1> >   m_x; //x registers
    std::vector<FixedComplex2<16, 1> >   m_y; //y registers
    FixedComplex2<16, 1>            m_output;

};

#endif /* FIXEDIIR_H_ */
