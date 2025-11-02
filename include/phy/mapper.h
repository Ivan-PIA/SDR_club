#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <complex>

using complex_d = std::complex<double>; 
using complex_int16 = std::complex<int16_t>; 

std::vector<complex_d> bpsk(const std::vector <int8_t> &bits);

std::vector<complex_d> qpsk(const std::vector <int8_t> &bits);