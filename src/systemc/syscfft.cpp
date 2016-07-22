// All systemc modules should include systemc.h header file
#include <systemc/syscfft.hpp>
#include <systemc/driver.hpp>
#include <systemc/monitor.hpp>

void syscfft::prc_state()
{

    if (rst) {

        en = 0;
        read = 0;
        valid_Data_OUT = 0;

        melay_state = FFT_STATE_INITIAL;

        x_r = 0;
        y_r = 0;
        x_im = 0;
        y_im = 0;

    } else if (valid_Data_IN.read()) {
        ctrl_4.write(2);
        ctrl_5.write(3);
        melay_state = next_state;

        switch (melay_state) {
            case FFT_STATE_INITIAL:
                en = 1;
                read = 0;
                //write = 1;
                temp1 = sc_int<2 * WORD_SIZE>(x_in_r) << 32;

                temp1 = temp1 | sc_int<2 * WORD_SIZE>(x_in_im);
                data_in.write(temp1);

                temp1 = data_out;
                temp_r = (temp1 >> 32) & ~(~0 << (63 - 32 + 1));
                temp_im = (temp1 >> 0) & ~(~0 << (31 - 0 + 1));

                z_r = comp_r;

                z_im = comp_im;

                //			if (N_STAGES == 2)
                //			{
                //				//next_trigger(20,SC_NS);
                //				if(addr_write.read()==((N_STAGES/2)-1))
                //				{
                //
                //					read = 1;
                //					temp1 = data_out;
                //					x_r = (temp1 >> 32) & ~(~0 << (32));
                //					x_im = (temp1 >> 0) & ~(~0 << (32));
                //					y_r = x_in_r;
                //					y_im = x_in_im;
                //					z_r = result_0_r;
                //					z_im = result_0_im;
                //					valid_Data_OUT = 1;
                //				}
                //
                //			}
                //			if (N_STAGES == 2)
                //			{
                //				x_r.write(register_r.read());
                //				x_im = register_im;
                //			}

                if (addr_write.read() == ((N_STAGES / 2) - 1)) {
                    //				if (N_STAGES == 2)
                    //					next_trigger(20,SC_NS);

                    read = 1;
                    //				if (N_STAGES == 2)
                    //					temp1 = data_in;
                    //				else
                    //				temp1 = data_out;
                    //				if (N_STAGES == 2)
                    //				{
                    //					x_r.write(register_r.read());
                    //					x_im = register_im;
                    //				}
                    //				else
                    //				{
                    x_r = (temp1 >> 32) & ~(~0 << (32));
                    x_im = (temp1 >> 0) & ~(~0 << (32));
                    //				}
                    y_r = x_in_r;
                    y_im = x_in_im;

                    if (N_STAGES == 2) {
                        z_r = comp_r;

                        z_im = comp_im;
                    } else {
                        z_r = result_0_r;
                        z_im = result_0_im;
                    }
                    valid_Data_OUT = 1;
                }

                break;
            case FFT_STATE_READ:
                en = 1;
                temp4 = 4294967295;
                valid_Data_OUT = 1;
                temp2 = sc_int<2 * WORD_SIZE>(result_1_r) << 32;
                read = 1;
                temp3 = temp4 & sc_int<2 * WORD_SIZE>(result_1_im);
                temp2 = temp2 | temp3;
                data_in.write(temp2);

                //(x >> lsb) & ~(~0 << (msb-lsb+1));
                temp1 = data_out;
                if (N_STAGES == 2) {
                    x_r.write(register_r.read());
                    x_im = register_im;
                } else {
                    x_r = (temp1 >> 32) & ~(~0 << (32));
                    x_im = (temp1 >> 0) & ~(~0 << (32));
                }
                y_r = x_in_r;
                y_im = x_in_im;
                z_r = result_0_r;
                z_im = result_0_im;
                temp_r = (temp1 >> 32) & ~(~0 << (63 - 32 + 1));
                temp_im = (temp1 >> 0) & ~(~0 << (31 - 0 + 1));
                if ((addr_write.read() == (N_STAGES / 2) - 1))
                    ready_out = 0;

                break;
            case FFT_STATE_OUTPUT:
                if (N_STAGES == 2)
                    valid_Data_OUT = 0;
                en = 1;
                read = 1;
                temp1 = data_out;
                temp_r = (temp1 >> 32) & ~(~0 << (63 - 32 + 1));
                temp_im = (temp1 >> 0) & ~(~0 << (31 - 0 + 1));
                z_r = comp_r;
                z_im = comp_im;
                if (addr_read.read() == ((N_STAGES / 2) - 1)) {
                    ready_out = 1;
                }
                break;

            case FFT_STATE_IDLE:
                read = 0;
                z_r = comp_r;
                z_im = comp_im;
                break;

        }
    }
    cout << melay_state << endl;
}

void syscfft::prc_output()
{
    scale_r.write(32);
    scale_im.write(0);
    if (rst) {
        addr_write = -1;
        if (N_STAGES == 2)
            addr_read = -1;
        else
            addr_read = 0;
        //		en = 0;
        //		read = 0;
        write = 0;
        //		N = N_STAGES;
        k = 0;
        next_state = melay_state;
    } else if (valid_Data_IN.read()) {

        switch (melay_state) {
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
                if (N_STAGES == 2) {
                    //temp1 = data_in;
                    //register_r = (temp1 >> 32) & ~(~0 << (32));
                    //register_im = (temp1 >> 0) & ~(~0 << (32));
                    register_r = x_in_r;
                    register_im = x_in_im;
                    addr_read.write(addr_read.read() + sc_uint<32>(1));
                    //				next_trigger(20,SC_NS);
                    if (addr_read.read() == ((N_STAGES / 2) - 1)) {
                        //					next_trigger(20,SC_NS);
                        next_state = FFT_STATE_READ;
                        addr_write = 0;
                        addr_read.write(0);

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
                    } else {
                        addr_write.write(addr_write.read() + sc_uint<32>(1));
                    }

                } else if (addr_write.read() == ((N_STAGES / 2) - 1)) {
                    //				if (N_STAGES == 2)
                    //					next_trigger(20,SC_NS);
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
                } else {
                    addr_write.write(addr_write.read() + sc_uint<32>(1));
                }
                //			}
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
                if ((addr_write.read() == ((N_STAGES / 2) - 1))
                        && ready_in.read()) {

                    next_state = FFT_STATE_OUTPUT;
                    if ((addr_read.read() == 0) & (N_STAGES != 2)) {
                        addr_read.write(addr_read.read() + sc_uint<32>(1));
                        k.write(1);
                    } else {
                        k.write(0);
                        addr_read = 0;
                    }
                    addr_write = 0;
                    write = 0;

                } else if ((addr_write.read() == ((N_STAGES / 2) - 1))
                        && (ready_in.read() == 0)) {
                    next_state = FFT_STATE_IDLE;
                    write = 0;

                } else {
                    if (addr_read.read() == ((N_STAGES / 2) - 1)) {
                        addr_read.write(0);
                    } else {
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
                if (addr_read.read() == ((N_STAGES / 2) - 1)) {
                    //								k = ((N_STAGES/2)-1);

                    if (N_STAGES == 2) {
                        k.write(0);

                    } else
                        //if(N_STAGES != 4)
                        k.write(k.read() + 1);
                    next_state = FFT_STATE_INITIAL;
                    addr_read = 0;
                    write = 1;
                    //				ready_out = 1;

                    //				}
                    //				else
                    //				{
                    //					next_state = FFT_STATE_IDLE;
                    //					write = 0;
                    //					//					k.write(k.read() + 1);
                    //				}
                    //
                } else {
                    addr_read.write(addr_read.read() + sc_uint<32>(1));
                    if (N_STAGES == 2) {
                        k.write(0);

                    } else
                        k.write(k.read() + 1);

                }
                //
                break;
            case FFT_STATE_IDLE:
                if (ready_in.read() == 1) {
                    next_state = FFT_STATE_OUTPUT;
                    addr_read = 1;
                    addr_write = 0;
                    write = 0;
                    k.write(1);
                    //				addr_read = 0;
                    //				write = 1;
                    //				k = ((N_STAGES/2)-1);
                    ////				ready_out.write(1);
                }
                break;
        }
    }
    //sc_trace_file *wf = sc_create_vcd_trace_file("counter");

}

