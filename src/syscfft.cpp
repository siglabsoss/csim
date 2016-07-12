// All systemc modules should include systemc.h header file
#include "syscfft.h"

void syscfft :: prc_state()
{
	if(rst)
	{
		melay_state = FFT_STATE_INITIAL;
//		z = sc_uint<32>(0);
		addr_write = 0;
		addr_read = 0;

	}
	else
	{
		melay_state = next_state;
	}
}

void syscfft::prc_output()
{
	switch(melay_state)
	{
	case FFT_STATE_INITIAL:
		en = 1;
		write = 1;
		data_in = x_in;
		if(addr_write.read()==N_STAGES.read()/2)
		{
			next_state = FFT_STATE_READ;
			addr_write = 0;

		}
		else
		{
			addr_write.write(addr_write.read() + sc_uint<32>(1));
		}


		break;
	case FFT_STATE_READ:
		en = 1;
		read = 1;
		//write = 1;
		data_in = result_1;
		x = data_out;
		y = x_in;
		z = result_0;
		if(addr_read.read()==N_STAGES.read()/2)
		{
			next_state = FFT_STATE_OUTPUT;
			addr_read = 0;
			addr_write = 0;
			write = 0;
		}
		else
		{
			addr_read.write(addr_read.read() + sc_uint<32>(1));
			addr_write.write(addr_write.read() + sc_uint<32>(1));
		}


		break;
	case FFT_STATE_OUTPUT:
		en = 1;
		read = 1;
		z = data_out;
		if(addr_read.read()==N_STAGES.read()/2)
		{
			next_state = FFT_STATE_INITIAL;
			addr_read = 0;

		}
		else
		{
			addr_read.write(addr_read.read() + sc_uint<32>(1));

		}

		break;
//	case FFT_STATE_IDLE:
//		read = 1;
//		en = 1;
//		z = data_out;
//		if(addr_read==N_STAGES/2)
//		{
//			next_state = FFT_STATE_INITIAL;
//			addr_read = 0;
//
//		}
	}
}
