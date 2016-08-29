#include <complex>
#include <vector>
#include <queue>
#include <cmath>
#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>

#ifndef __FIXEDFFT_H__
#define __FIXEDFFT_H__

enum FFFT_STATE
{
    FFFT_STATE_INITIAL, FFFT_STATE_READ, FFFT_STATE_OUTPUT
};

class fixedfftbase
{
public:
    virtual void inputandtick(FixedComplex32 x) = 0;
    virtual ~fixedfftbase() {}
    int ready;
};

class fixedfftstage: public fixedfftbase
{
public:
    int 							N;
    std::vector<FixedComplex32>    	memory;
    FFFT_STATE         				state;
    int                 			read_pointer;
    int                 			write_pointer;
    int                 			clock;
    fixedfftbase        			*next;
    std::vector<int> 				*mainTablePointer;

    fixedfftstage();
    void init(int Ninput, bool m_inverse);
    void inputandtick(FixedComplex32 x);
    void output(FixedComplex32 x);
    void butterfly(FixedComplex32 array[2], FixedComplex32 x, FixedComplex32 y);
    FixedComplex32 twiddler(int k);

private:
    bool m_inverse;


};
class fixedfftprint: public fixedfftbase
{
public:
    int								N; //N point fft
    std::queue<FixedComplex32>		m_output; //Stores outputs

    fixedfftprint(int Ninput, bool inverse);
    void inputandtick(FixedComplex32 x); //Adds output to queue

private:
    bool m_inverse; //If this filter is an fft or ifft
};

class fixedfft : public FilterChainElement
{
public:
	unsigned int 				N;
    int 						stagecount;
    unsigned int 				m_count;
    bool 						newInput;
    std::vector<int> 			mainTable;
    std::vector<fixedfftstage> 	stages;
    fixedfftprint 				printer;
    
	void tick() override;
    fixedfft(int Ninput, int tableSize = 0, bool inverse = 0);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;

private:
    bool m_inverse;

};

#endif
