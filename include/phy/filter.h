#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <complex>
#include <fstream>

using complex_d = std::complex<double>; 
using complex_int16 = std::complex<int16_t>; 

std::vector<complex_d> upsampling(const std::vector<complex_d>& symbols, int count_smpl_symbol);

std::vector<complex_d> rect_pulse(int count_smpl_symbol);

std::vector<complex_d> raised_cosine(int N);

std::vector <complex_d> convolve(const std::vector <complex_d> &symb, const std::vector <complex_d> &pulse);

std::vector<complex_d> convolve2(const std::vector<complex_d>& x, const std::vector<complex_d>& h);