#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

#include "../include/phy/filter.h"
#include "../include/phy/mapper.h"
#include "../include/device/soapy_api.hpp"

#define BLOCK_SAMPLES 1920 

template <typename T>
void print_vec(const std::vector<T>& vec) {
    for (const auto& v : vec) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
}

int main(void)
{
    set_sdr dev1;
    memset(&dev1, 0, sizeof(dev1)); // обнуляем
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(&dev1);
    set_sample_rate(&dev1, SOAPY_SDR_RX, 2e6);
    set_sample_rate(&dev1, SOAPY_SDR_TX, 2e6);
    set_frequency(&dev1, SOAPY_SDR_RX, 800e6);
    set_frequency(&dev1, SOAPY_SDR_TX, 800e6);
    setup_stream(&dev1, SOAPY_SDR_RX, channels, channel_count);
    setup_stream(&dev1, SOAPY_SDR_TX, channels, channel_count);
    set_gain(&dev1, SOAPY_SDR_RX, channels, 30);
    set_gain(&dev1, SOAPY_SDR_TX, channels, -90);
    get_MTU(&dev1);
    active_stream(&dev1);

    int smpl_in_symbl = 10;
    std::vector<int8_t> bits = {1,1,1,1,1,1,1,1,0,0,1,0,1,0,1,1,0,1,0,1,0,1,1,1,1,0};
    std::vector<complex_d> bpsk_sym = qpsk(bits);

    std::vector<complex_d> upbits = upsampling(bpsk_sym, smpl_in_symbl);

    print_vec(bpsk_sym);

    std::vector<complex_d> pulse_vec = rect_pulse(smpl_in_symbl);
    std::vector<complex_d> conv = convolve(upbits, pulse_vec);
    
    trx_samples_buff_repeat(&dev1, conv);

    shutdown(&dev1);
}