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
			if ( addr_read.read() < MEM_SIZE)
			{
				data_out.write() = ram[addr_read.read()];
			}
			else
			{
				data_out.write() = allxs;
			} // end of else
		} // end of read
		if(write)
		{ // write
			if ( addr_write.read() < MEM_SIZE)
			{
				ram[addr_write.read()] = data_in.read();
			}
		}
	}
	else // en is not active
	{
		data_out.write() = allzs;
	}


}
