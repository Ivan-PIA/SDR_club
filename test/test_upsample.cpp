#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

#include "include/phy/filter.h"
#include "include/phy/mapper.h"

template <typename T>
void print_vec(const std::vector<T>& vec) {
    for (const auto& v : vec) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
}

int main(void)
{
    int smpl_in_symbl = 10;
    std::vector<int8_t> bits = {0,0,1,0,1,0,1,1,0,1,0,1,0,1,1,1,1,0};
    std::vector<complex_d> bpsk_sym = bpsk(bits);

    std::vector<complex_d> upbits = upsampling(bpsk_sym, smpl_in_symbl);

    print_vec(bpsk_sym);

    for (int i = 0; i < upbits.size(); i++){
        if (i%10 == 0){
            std::cout << std::endl;
        }
        std::cout << upbits[i] << ", ";
    }
    std::cout << std::endl;

    std::vector<complex_d> pulse_vec = rect_pulse(smpl_in_symbl);

    std::vector<complex_d> conv = convolve(upbits, pulse_vec);

    std::cout << std::endl;
    
    for (int i = 0; i < conv.size(); i++){
        if (i%10 == 0){
            std::cout << std::endl;
        }
        std::cout << conv[i] << ", ";
    }
    std::cout << std::endl;

}