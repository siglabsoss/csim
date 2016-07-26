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

    fixediir(int registerXSize, int registerYSize, FixedComplex<16>* aCoeffs,
               FixedComplex<16>* bCoeffs); //Constructor
    void iir(FixedComplex<16> &input); //Filtering input data
    void reset(); //resets registers
    FixedComplex<16> calculate(FixedComplex<16> current); // Calculate output based on input

    int                         m_numXRegisters;
    int                         m_numYRegisters;
    vector<FixedComplex<16> >   m_a; //a coefficients
    vector<FixedComplex<16> >   m_b; //b coefficients
    vector<FixedComplex<16> >   m_x; //x registers
    vector<FixedComplex<16> >   m_y; //y registers
    FixedComplex<16>            m_output;

};

#endif /* FIXEDIIR_H_ */
