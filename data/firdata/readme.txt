Command History:
 pkg load signal
 in2 = exp((1:1024)/2.33*1j)/10;
 pkg load communications
 b = fir1(40,0.3);
 out2 = filter(b,1,in2);
 in3 = exp((1:1024)/11.33*1j);
 out3 = filter(b,1,in3);
 in2c = [real(in2') imag(in2')];
 csvwrite('data2_in.csv',in2c);
 out2c = [real(out2.') imag(out2.')];
 csvwrite('data2_out.csv',out2c);
 in2c = [real(in2.') imag(in2.')];
 csvwrite('data2_in.csv',in2c);
 in3c = [real(in3.') imag(in3.')];
 csvwrite('data3_in.csv',in3c);
 out3c = [real(out3.') imag(out3.')];
 csvwrite('data3_out.csv',out3c);
