#include <types/fixedcomplex.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/scrambler.hpp>
#include <utils/mcs.hpp>
#include <map>
#include <queue>

#pragma once

typedef uint8_t                                 symbol_t;
typedef SLFixComplex                            constellation_t;
typedef std::map <symbol_t, constellation_t>    constellation_map_t;

class Mapper : public FilterChainElement
{
private: //constants
    static constexpr size_t INPUT_BUFFER_BITS_MAX = 8 * 512;
    static constexpr symbol_t NULL_SYMBOL = 0;

public:
    Mapper(unsigned int ticksPerSymbol, MCS mcs);

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
    symbol_t    generateRandomSymbol();

private:
    constellation_map_t     m_constellations; //mapping of symbol -> constellation vector
    size_t                  m_bitsPerSymbol;
    std::queue<bool>        m_inputBuffer;    //inefficient storage (8x inflated) but that's OK
    constellation_t         m_output;
    Scrambler               m_scrambler;

    unsigned int            m_tickCount;
    unsigned int            m_ticksPerSymbol;
    bool                    m_gotFirstSymbol;
};
