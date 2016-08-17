#include <types/fixedcomplex.hpp>
#include <core/filter_chain_element.hpp>
#include <map>
#include <queue>

#pragma once

typedef uint8_t                                 symbol_t;
typedef FixedComplex32                          constellation_t;
typedef std::map <symbol_t, constellation_t>    constellation_map_t;

class Modulator : public FilterChainElement
{
private: //constants
    static constexpr size_t INPUT_BUFFER_BITS_MAX = 8 * 512;
    static constexpr symbol_t NULL_SYMBOL = 0;
public: //types
    enum mod_scheme_t {
        MOD_SCHEME_BPSK = 0,
        MOD_SCHEME_QPSK,
        MOD_SCHEME_8PSK,
        MOD_SCHEME_QAM16
    };
public:
    Modulator(unsigned int ticksPerSymbol, mod_scheme_t scheme);

    /**
     * Input is expected to be a digital bit stream (IO_TYPE_BYTE)
     */
    bool input(const filter_io_t &data) override;

    /**
     * Output is a complex number with components ranging from [-1, 1] and norm = 1
     */
    bool output(filter_io_t &data) override;
    void tick(void) override;

public: //static methods

    //obtain symbol/constellation mappings for various modulation schemes
    static constellation_map_t getBPSKConstellations();
    static constellation_map_t getQPSKConstellations();
    static constellation_map_t get8PSKConstellations();
    static constellation_map_t getQAM16Constellations();

private: //methods
    symbol_t    getNextSymbol();

private:
    constellation_map_t     m_constellations; //mapping of symbol -> constellation vector
    size_t                  m_bitsPerSymbol;
    std::queue<bool>        m_inputBuffer;    //inefficient storage (8x inflated) but that's OK
    constellation_t         m_output;

    unsigned int            m_tickCount;
    unsigned int            m_ticksPerSymbol;

};
