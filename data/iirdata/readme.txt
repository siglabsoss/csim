



Command History:
 pkg load signal
in2 = exp((1:1024)/2.33*1j)/10;
 pkg load communications
 b = fir1(40,0.3);
 a = fir1(40,0.1);
 a(1) = 1;
 out2 = filter(b,a,in2);
 in2c = [real(in2.') imag(in2.')];
 csvwrite('data2_in.csv',in2c);
 out2c = [real(out2.') imag(out2.')];
 csvwrite('data2_out.csv',out2c);
csvwrite('ataps.csv',a.');
csvwrite('btaps.csv',b.');
