
#include <filters/ldpc_decode.hpp>

#include <core/parameters.hpp>

#include <cassert>


bool LDPCDecode::input(const filter_io_t &data)
{

    return true;
}

bool LDPCDecode::output(filter_io_t &data)
{

    return true;
}

void LDPCDecode::tick(void)
{

}



LDPCDecode::LDPCDecode(const std::vector<std::vector<bool> > &H):
FilterChainElement(std::string("LDPCDecode")),
m_hrows(H.size()),
m_hcols(H[0].size()), //will crash if hrows == 0
m_H(H),
m_n(H[0].size()),
m_m(H.size())
{
    // calculate stuff about H
    prep_once();
}

LDPCDecode::~LDPCDecode()
{

}


void LDPCDecode::print_h()
{
    std::cout << "H = " << std::endl;
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            std::cout << m_H[i][j] << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void LDPCDecode::prep_once()
{
    // calculate degree for each check node, fill in node_index
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        m_m[i].degree = 0;
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            if( m_H[i][j] )
            {
                // keep track of the index of which variable nodes this check node is connected to
                m_m[i].node_index.push_back(j);
                m_m[i].degree++;
            }
        }

//        std::cout << "Check node " << i << " has degree " << m_m[i].degree << std::endl;
    }
}


void LDPCDecode::calc_syndrome(unsigned print = 1)
{

    for( unsigned i = 0; i < m_hrows; ++i )
    {
        if( print ) std::cout << "equation (" << i << ") = ";
        LDPC_M *mi = &(m_m[i]);

        mi->parity = 0;

        for( unsigned j = 0; j < mi->degree; ++j)
        {
            int node_index = mi->node_index[j];
            LDPC_N *ni = &(m_n[node_index]);

            short val = ( ni->llr.to_double() < 0 ) ? 1 : 0;

            if( print && j != 0 ) std::cout << " ^ ";
            if( print ) std::cout << val;

            mi->parity = val ^ mi->parity;
        }

        if( print ) std::cout << " = " << mi->parity << std::endl;
    }
}

// technically returns the sum of the syndrome
unsigned LDPCDecode::get_syndrome(void)
{
    unsigned syndrome = 0;  // "And IncrediBoy!!"

    for( unsigned i = 0; i < m_hrows; ++i )
    {
        syndrome += m_m[i].parity;
    }

    return syndrome;
}




void LDPCDecode::iteration()
{
    // Load up min 1,2.  This is the Q message stage
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        LDPC_M *mi = &(m_m[i]);
        LDPC_N *ni = &(m_n[mi->node_index[0]]);
        LDPC_N *ni_next = &(m_n[mi->node_index[1]]);

        // cook first two iterations of loop below
        if( std::abs(ni->llr.to_double()) < std::abs(ni_next->llr.to_double()) )
        {
            mi->min[0]       = ni->llr;
            mi->min_index[0] = mi->node_index[0];

            mi->min[1]       = ni_next->llr;
            mi->min_index[1] = mi->node_index[1];
        }
        else
        {
            mi->min[1]       = ni->llr;
            mi->min_index[1] = mi->node_index[0];

            mi->min[0]       = ni_next->llr;
            mi->min_index[0] = mi->node_index[1];
        }

        // start at 2
        for( unsigned j = 2; j < mi->degree; ++j)
        {
            unsigned node_index = mi->node_index[j];
            ni = &(m_n[node_index]);

            if( std::abs(ni->llr.to_double()) < std::abs(mi->min[0].to_double()) )
            {
                // this is the smallest yet.  bump the previous smallest to 2nd smallest
                mi->min[1] = mi->min[0];
                mi->min_index[1] = mi->min_index[0];

                // and assign
                mi->min[0] = ni->llr;
                mi->min_index[0] = node_index;
            }
            else if( std::abs(ni->llr.to_double()) < std::abs(mi->min[1].to_double()) )
            {
                // this is only smaller than the 2nd smallest, simply assign
                mi->min[1] = ni->llr;
                mi->min_index[1] = node_index;
            }
        }
    }


    // we can't modify n array directly because we assume the sign of llr on variable nodes stays the same throughout the whole loop
    SLFixedPoint<LDPC_LLR_FORMAT> additions[m_hcols];

    for( unsigned j = 0; j < m_hcols; ++j ) {
        additions[j] = 0.0;
    }

    // Pass R messages
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        LDPC_M *mi = &(m_m[i]);

        for( unsigned j = 0; j < mi->degree; ++j)
        {
            unsigned node_index = mi->node_index[j];
            LDPC_N *ni = &(m_n[node_index]);

            double val;

            // due to the exclusive property, check nodes alter the llr of variable nodes using all connected variable nodes except for the one being modified
            if( node_index == mi->min_index[0] )
            {
                val = std::abs(mi->min[1].to_double());
            }
            else
            {
                val = std::abs(mi->min[0].to_double());
            }

            int sign = ( ni->llr.to_double() < 0 ) ? -1 : 1;

            if( mi->parity )
            {
                // parity is failing, flip the bit
                sign *= -1;
            }
            else
            {
                // parity is ok, push the variable node stronger in it's current direction
                // don't flip sign
            }
            SLFixedPoint<LDPC_LLR_FORMAT> tmpVal(sign*val);
            additions[node_index] += tmpVal;
        }
    }

    // apply additions all at once
    for( unsigned j = 0; j < m_hcols; ++j )
    {
        LDPC_N *ni = &(m_n[j]);
        ni->llr += additions[j];
    }

}


std::vector<bool> LDPCDecode::get_message()
{
    std::vector<bool> message;
//    std::cout << "Assuming that the syndrome is 0, the message is: " << std::endl;

    unsigned degreek = m_hcols - m_hrows;

    for( unsigned j = 0; j < degreek; ++j )
    {
        LDPC_N *ni = &(m_n[j]);

        uint8_t val;
        if( ni->llr.to_double() < 0 )
        {
            val = 1;
        }
        else
        {
            val = 0;
        }

        message.push_back(val);

//        std::cout << val << std::endl;

    }

    return message;

}

void LDPCDecode::print_cw()
{
    std::cout << "Code word at this stage: " << std::endl;
    unsigned i;
    for(i = 0; i < 24; i++)
    {
        char c;
        c = m_n[i].llr.to_double() < 0 ? '1' : '0';
        std::cout << c << std::endl;
    }

    std::cout << std::endl;
}



void LDPCDecode::decode(const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > &cw, size_t iterations, bool& solved, size_t& solved_iterations)
{
    assert(cw.size() == m_n.size());
    assert(m_n.size() == m_hcols);

    unsigned count = 0;
    for(auto it = m_n.begin(); it != m_n.end(); ++it)
    {
        it->llr = cw[count];
        count++;
    }

    // These stay here if code is unsolved
    solved = false;
    solved_iterations = 0;


    unsigned syn;
    for(size_t i = 0; i < iterations; ++i)
    {
        calc_syndrome(0);
        syn = get_syndrome();

//        for (size_t j = 0; j < m_n.size(); j++) {
//            std::cout << m_n[j].llr << ", ";
//        }
//        std::cout << std::endl;

//        std::cout << "Syndrome starting at iteration " << i << " is " << syn << std::endl;

        if(syn == 0)
        {
           //std::cout << "Breaking after " << (signed)i-1 << " iteration" << std::endl;
           if (!solved) {
               solved_iterations = i;
           }
           solved = true;
           //return;
        } else {
            assert(!solved);
        }

        iteration();
    }
}




