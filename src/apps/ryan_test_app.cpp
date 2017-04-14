#include <core/filter_chain.hpp>
#include <filters/scrambler_block.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    ScramblerBlock *scrambler = new ScramblerBlock;

    FilterChain txChain = *scrambler;

    std::cout << "Hello" << std::endl;

    return 0;
}
