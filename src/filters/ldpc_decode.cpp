
#include <filters/ldpc_decode.hpp>

#include <core/parameters.hpp>

#include <cassert>


bool LDPCDecode::input(const filter_io_t &data)
{
//    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW || data.type == IO_TYPE_COMPLEX_DOUBLE);
//    m_inputValid = true;
//    m_input = data.toComplexDouble();
    return true;
}

bool LDPCDecode::output(filter_io_t &data)
{
//    m_inputValid = false;
//    data = m_carrier * m_input;
    return true;
}

void LDPCDecode::tick(void)
{
//    double theta = (((2 * M_PI) / m_ticksPerPeriod) * m_count);
//
//    m_carrier = ComplexDouble(cos(theta), sin(theta));
//
//    if (m_upMix) {
//        m_count++;
//        if (m_count >= m_ticksPerPeriod) {
//            m_count -= m_ticksPerPeriod;
//        }
//    } else {
//        m_count--;
//        if (m_count <= 0) {
//            m_count += m_ticksPerPeriod;
//        }
//    }
}



LDPCDecode::LDPCDecode(vector<vector<uint8_t> > H, uint32_t rows, uint32_t cols):
FilterChainElement(std::string("LDPCDecode")),
m_hrows(rows),
m_hcols(cols),
m_H(H),
m_n(cols),
m_m(rows)
{
    assert(m_H.size() == rows);
    assert(m_H[0].size() == cols);

    // calculate stuff about H
    prep_once();
}

LDPCDecode::~LDPCDecode()
{

}


void LDPCDecode::print_h()
{
    cout << "H = " << endl;
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            cout << m_H[i][j] << ",";
        }
        cout << endl;
    }
    cout << endl;
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

//        cout << "Check node " << i << " has degree " << m_m[i].degree << endl;
    }
}


void LDPCDecode::calc_syndrome(unsigned print = 1)
{

    for( unsigned i = 0; i < m_hrows; ++i )
    {
        if( print ) cout << "equation (" << i << ") = ";
        LDPC_M *mi = &(m_m[i]);

        mi->parity = 0;

        for( unsigned j = 0; j < mi->degree; ++j)
        {
            int node_index = mi->node_index[j];
            LDPC_N *ni = &(m_n[node_index]);

            short val = ( ni->llr < 0 ) ? 1 : 0;

            if( print && j != 0 ) cout << " ^ ";
            if( print ) cout << val;

            mi->parity = val ^ mi->parity;
        }

        if( print ) cout << " = " << mi->parity << endl;
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
        if( std::abs(ni->llr) < abs(ni_next->llr) )
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
            int node_index = mi->node_index[j];
            ni = &(m_n[node_index]);

            if( abs(ni->llr) < abs(mi->min[0]) )
            {
                // this is the smallest yet.  bump the previous smallest to 2nd smallest
                mi->min[1] = mi->min[0];
                mi->min_index[1] = mi->min_index[0];

                // and assign
                mi->min[0] = ni->llr;
                mi->min_index[0] = node_index;
            }
            else if( abs(ni->llr) < abs(mi->min[1]) )
            {
                // this is only smaller than the 2nd smallest, simply assign
                mi->min[1] = ni->llr;
                mi->min_index[1] = node_index;
            }
        }
    }


    // we can't modify n array directly because we assume the sign of llr on variable nodes stays the same throughout the whole loop
    float additions[m_hcols];

    for( unsigned j = 0; j < m_hcols; ++j )
        additions[j] = 0;

    // Pass R messages
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        LDPC_M *mi = &(m_m[i]);

        for( unsigned j = 0; j < mi->degree; ++j)
        {
            int node_index = mi->node_index[j];
            LDPC_N *ni = &(m_n[node_index]);

            int val;

            // due to the exclusive property, check nodes alter the llr of variable nodes using all connected variable nodes except for the one being modified
            if( node_index == mi->min_index[0] )
            {
                val = abs(mi->min[1]);
            }
            else
            {
                val = abs(mi->min[0]);
            }

            int sign = ( ni->llr < 0 ) ? -1 : 1;

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

            additions[node_index] += sign*val;
        }
    }

    // apply additions all at once
    for( unsigned j = 0; j < m_hcols; ++j )
    {
        LDPC_N *ni = &(m_n[j]);
        ni->llr += additions[j];
    }

}


vector<uint8_t> LDPCDecode::get_message()
{
    vector<uint8_t> message;
//    cout << "Assuming that the syndrome is 0, the message is: " << endl;

    unsigned degreek = m_hcols - m_hrows;

    for( unsigned j = 0; j < degreek; ++j )
    {
        LDPC_N *ni = &(m_n[j]);

        uint8_t val;
        if( ni->llr < 0 )
        {
            val = 1;
        }
        else
        {
            val = 0;
        }

        message.push_back(val);

//        cout << val << endl;

    }

    return message;

}

void LDPCDecode::print_cw()
{
    cout << "Code word at this stage: " << endl;
    unsigned i;
    for(i = 0; i < 24; i++)
    {
        char c;
        c = m_n[i].llr < 0 ? '1' : '0';
        cout << c << endl;
    }

    cout << endl;
}



void LDPCDecode::decode(vector<int> cw, size_t iterations, bool& solved, size_t& solved_iterations)
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

        cout << "Syndrome starting at iteration " << i << " is " << syn << endl;

        if(syn == 0)
        {
           cout << "Breaking after " << (signed)i-1 << " iteration" << endl;
           solved = true;
           solved_iterations = i;
           return;
        }

        iteration();
    }
}

void LDPCDecode::run()
{
    /*
    print_h();

    // Load data in step (This is Channel to bit message "L")

    // positive is 0
    // negative represents a 1

    // example 1 from slides
//  n[0].llr = -9;
//  n[1].llr = +7;
//  n[2].llr = -12;
//  n[3].llr = +4;
//  n[4].llr = +7;
//  n[5].llr = +10;
//  n[6].llr = -11;
//
//  // example 2 from slides
//  n[0].llr = -9;
//  n[1].llr = -7;
//  n[2].llr = -12;
//  n[3].llr = -4;
//  n[4].llr = +7;
//  n[5].llr = +10;
//  n[6].llr = -11;

    n[0].llr = 4161 * -1;
    n[1].llr = 5953 * -1;
    n[2].llr = -9328 * -1;
    n[3].llr = 0 * -1;
    n[4].llr = -1188 * -1;
    n[5].llr = 0 * -1;
    n[6].llr = -1144 * -1;
    n[7].llr = 0 * -1;
    n[8].llr = -3925 * -1;
    n[9].llr = 0 * -1;
    n[10].llr = -6833 * -1;
    n[11].llr = 9005 * -1;
    n[12].llr = -4161 * -1;
    n[13].llr = 7449 * -1;
    n[14].llr = 965 * -1;
    n[15].llr = -7030 * -1;
    n[16].llr = 0 * -1;
    n[17].llr = 0 * -1;
    n[18].llr = 0 * -1;
    n[19].llr = -1967 * -1;
    n[20].llr = 0 * -1;
    n[21].llr = 0 * -1;
    n[22].llr = -9328 * -1;
    n[23].llr = -3154 * -1;

    print_cw();

    // Do check
    calc_syndrome();
    get_syndrome();
    cout << endl;


    // iteration
    iteration();

    // Do check
    calc_syndrome();
    get_syndrome();
    cout << endl;


    // iteration
    iteration();

    // Do check
    calc_syndrome();
    get_syndrome();
    cout << endl;

    iteration();

    calc_syndrome();
    get_syndrome();

    print_cw();

    get_message();

*/
}



