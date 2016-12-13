#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/scrambler.hpp>
#include <utils/mcs.hpp>
#include <map>

#pragma once

typedef uint8_t                                 symbol_t;
typedef SLFixComplex                            constellation_t;
typedef std::map <symbol_t, constellation_t>    constellation_map_t;

class Mapper : public FilterChainElement
{
private: //constants
    static constexpr symbol_t NULL_SYMBOL = 0;

public:
    Mapper(MCS mcs);

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
    CircularBuffer<bool>    m_fifo;
    constellation_t         m_output;
    Scrambler               m_scrambler;

    bool                    m_outputReady;
};
