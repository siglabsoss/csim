/*
 * memory.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "memory.h"



void memory::prc_memory()
{
	sc_lv<WORD_SIZE> allzs (sc_logic_Z);
	sc_lv<WORD_SIZE> allxs (sc_logic_X);

	if(en)
	{
		if(read)
		{ //read
			if ( addr.read() < MEM_SIZE)
			{
				data = ram[addr.read()];
			}
			else
			{
				data = allxs;
			} // end of else
		} // end of read
		if(write)
		{ // write
			if ( addr.read() < MEM_SIZE)
			{
				ram[addr.read()] = data;
			}
		}
	}
	else // en is not active
	{
		data = allzs;
	}


}
