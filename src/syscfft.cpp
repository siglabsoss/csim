// All systemc modules should include systemc.h header file
#include "syscfft.h"
#include "driver.h"
#include "monitor.h"
void syscfft :: prc_state()
{
	if(rst)
	{
		//		addr_write = -1;
		//				addr_read = 0;
		en = 0;
		read = 0;
		//		write = 0;
		N = N_STAGES;
		//				next_state = melay_state;
		melay_state = FFT_STATE_INITIAL;
		//next_state = melay_state;
		//		z = sc_uint<32>(0);
		x_r = 0;
		y_r = 0;
		x_im = 0;
		y_im = 0;


	}
	else
	{
		melay_state = next_state;
		sc_int<2*WORD_SIZE> temp1;
		switch(melay_state)
		{
		case FFT_STATE_INITIAL:
			en = 1;
			//			write = 1;
			temp1 = sc_int<2*WORD_SIZE>(x_in_r)<<32;
			//		data_in = sc_int<2*WORD_SIZE>(x_in_r)<<32;
			//		data_in = data_in<<32;
			temp1 = temp1 | sc_int<2*WORD_SIZE>(x_in_im);
			data_in.write(temp1);
			//		data_in = data_in | sc_int<2*WORD_SIZE>(x_in_im);
			//			if(addr_write.read()==((N_STAGES.read()/2)-2))
			//				read = 1;
			if(addr_write.read()==((N_STAGES.read()/2)-1))
			{
				//				next_state = FFT_STATE_READ;
				//				addr_write = 0;
				//				addr_read.write(addr_read.read() + sc_uint<32>(1));
				//				temp1 = sc_int<2*WORD_SIZE>(result_1_r)<<32;
				//				temp1 = temp1 | sc_int<2*WORD_SIZE>(result_1_im);
				//				data_in.write(temp1);
				read = 1;
				temp1 = data_out;
				x_r = (temp1 >> 32) & ~(~0 << (32));
				x_im = (temp1 >> 0) & ~(~0 << (32));
				y_r = x_in_r;
				y_im = x_in_im;
				z_r = result_0_r;
				z_im = result_0_im;

			}
			//			else
			//			{
			//				addr_write.write(addr_write.read() + sc_uint<32>(1));
			//			}


			break;
		case FFT_STATE_READ:
			en = 1;

			//write = 1;
			temp1 = sc_int<2*WORD_SIZE>(result_1_r)<<32;
			//		data_in = result_1_r;
			//		data_in = data_in<<32;
			//		data_in = data_in + result_1_im;
			temp1 = temp1 | sc_int<2*WORD_SIZE>(result_1_im);
			data_in.write(temp1);

			//(x >> lsb) & ~(~0 << (msb-lsb+1));
			temp1 = data_out;
			x_r = (temp1 >> 32) & ~(~0 << (32));
			x_im = (temp1 >> 0) & ~(~0 << (32));
			y_r = x_in_r;
			y_im = x_in_im;
			z_r = result_0_r;
			z_im = result_0_im;
			if(addr_read.read()==((N_STAGES.read()/2)-1))
			{
				//				next_state = FFT_STATE_OUTPUT;
				//				addr_read = 0;
				//				addr_write = 0;
				//				write = 0;
			}
			//			else
			//			{
			//				addr_read.write(addr_read.read() + sc_uint<32>(1));
			//				addr_write.write(addr_write.read() + sc_uint<32>(1));
			//			}


			break;
		case FFT_STATE_OUTPUT:
			en = 1;
			read = 1;
			temp1 = data_out;
			z_r = (temp1 >> 32) & ~(~0 << (63-32+1));
			z_im = (temp1 >> 0) & ~(~0 << (31-0+1));
			if(addr_read.read()==((N_STAGES.read()/2)-1))
			{
				//				next_state = FFT_STATE_INITIAL;
				//				addr_read = 0;
				//				write = 1;

			}
			//			else
			//			{
			//				addr_read.write(addr_read.read() + sc_uint<32>(1));
			//
			//			}

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
	cout <<melay_state<<endl;
}

void syscfft::prc_output()
{
	if(rst)
	{
		addr_write = -1;
		addr_read = 0;
		//		en = 0;
		//		read = 0;
		write = 0;
		//		N = N_STAGES;
		k = 0;
		next_state = melay_state;
	}
	else
	{
		sc_int<2*WORD_SIZE> temp1;
		switch(melay_state)
		{
		case FFT_STATE_INITIAL:
			//			en = 1;
			write = 1;
			k = 0;
			//						temp1 = sc_int<2*WORD_SIZE>(x_in_r)<<32;
			//						//		data_in = sc_int<2*WORD_SIZE>(x_in_r)<<32;
			//						//		data_in = data_in<<32;
			//						temp1 = temp1 | sc_int<2*WORD_SIZE>(x_in_im);
			//						data_in.write(temp1);
			//			//		data_in = data_in | sc_int<2*WORD_SIZE>(x_in_im);
			//			if(addr_write.read()==((N_STAGES.read()/2)-2))
			//				read = 1;
			if(addr_write.read()==((N_STAGES.read()/2)-1))
			{
				next_state = FFT_STATE_READ;
				addr_write = 0;
				addr_read.write(addr_read.read() + sc_uint<32>(1));

				//								temp1 = sc_int<2*WORD_SIZE>(result_1_r)<<32;
				//								temp1 = temp1 | sc_int<2*WORD_SIZE>(result_1_im);
				//								data_in.write(temp1);
				//
				//				temp1 = data_out;
				//				x_r = (temp1 >> 32) & ~(~0 << (32));
				//				x_im = (temp1 >> 0) & ~(~0 << (32));
				//				y_r = x_in_r;
				//				y_im = x_in_im;
				//				z_r = result_0_r;
				//				z_im = result_0_im;
				//
			}
			else
			{
				addr_write.write(addr_write.read() + sc_uint<32>(1));
			}
			//
			//
			break;
		case FFT_STATE_READ:
			//			en = 1;
			//
			//			//write = 1;
			//						temp1 = sc_int<2*WORD_SIZE>(result_1_r)<<32;
			//						//		data_in = result_1_r;
			//						//		data_in = data_in<<32;
			//						//		data_in = data_in + result_1_im;
			//						temp1 = temp1 | sc_int<2*WORD_SIZE>(result_1_im);
			//						data_in.write(temp1);
			//
			//			//(x >> lsb) & ~(~0 << (msb-lsb+1));
			//			temp1 = data_out;
			//			x_r = (temp1 >> 32) & ~(~0 << (32));
			//			x_im = (temp1 >> 0) & ~(~0 << (32));
			//			y_r = x_in_r;
			//			y_im = x_in_im;
			//			z_r = result_0_r;
			//			z_im = result_0_im;
			if(addr_write.read()==((N_STAGES.read()/2)-1))
			{
				next_state = FFT_STATE_OUTPUT;
				addr_read = 1;
				addr_write = 0;
				write = 0;
			}
			else
			{
				if(addr_read.read()==((N_STAGES.read()/2)-1))
				{
					addr_read.write(0);
				}
				else
				{
					addr_read.write(addr_read.read() + sc_uint<32>(1));
				}
				addr_write.write(addr_write.read() + sc_uint<32>(1));
			}

			//
			break;
		case FFT_STATE_OUTPUT:
			//			en = 1;
			//			read = 1;
			//			temp1 = data_out;
			//			z_r = (temp1 >> 32) & ~(~0 << (63-32+1));
			//			z_im = (temp1 >> 0) & ~(~0 << (31-0+1));
			if(addr_read.read()==((N_STAGES.read()/2)-1))
			{
				next_state = FFT_STATE_INITIAL;
				addr_read = 0;
				write = 1;
				k = ((N_STAGES.read()/2)-1);
				//
			}
			else
			{
				addr_read.write(addr_read.read() + sc_uint<32>(1));
				k.write(k.read() + 1);

			}
			//
			break;
			//			//	case FFT_STATE_IDLE:
			//			//		read = 1;
			//			//		en = 1;
			//			//		z = data_out;
			//			//		if(addr_read==N_STAGES/2)
			//			//		{
			//			//			next_state = FFT_STATE_INITIAL;
			//			//			addr_read = 0;
			//			//
			//			//		}
		}
	}
	//sc_trace_file *wf = sc_create_vcd_trace_file("counter");

}



