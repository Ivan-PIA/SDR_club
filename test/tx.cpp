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

void set_parametrs(set_sdr *dev, const char *usb){
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(dev, usb);
    printf("SET ARG\n");
    set_sample_rate(dev, SOAPY_SDR_RX, 1e6);
    set_sample_rate(dev, SOAPY_SDR_TX, 1e6);
    set_frequency(dev, SOAPY_SDR_RX, 800e6);
    set_frequency(dev, SOAPY_SDR_TX, 800e6);
    setup_stream(dev, SOAPY_SDR_RX, channels, channel_count);
    setup_stream(dev, SOAPY_SDR_TX, channels, channel_count);
    set_gain(dev, SOAPY_SDR_RX, channels, 30);
    set_gain(dev, SOAPY_SDR_TX, channels, -90);
    get_MTU(dev);
    active_stream(dev);


}

void set_parametrs_RX(set_sdr *dev, const char *usb){
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(dev, usb);
    printf("SET ARG\n");
    set_sample_rate(dev, SOAPY_SDR_RX, 1e6);
    set_frequency(dev, SOAPY_SDR_RX, 870e6);
    setup_stream_RX(dev, SOAPY_SDR_RX, channels, channel_count);
    set_gain(dev, SOAPY_SDR_RX, channels, 25);
    get_MTU_RX(dev);
    active_stream_RX(dev);

}

void set_parametrs_TX(set_sdr *dev, const char *usb){
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(dev, usb);
    printf("SET ARG\n");
    set_sample_rate(dev, SOAPY_SDR_TX, 1e6);
    set_frequency(dev, SOAPY_SDR_TX, 870e6);
    setup_stream_TX(dev, SOAPY_SDR_TX, channels, channel_count);
    set_gain(dev, SOAPY_SDR_TX, channels, -10);
    get_MTU_TX(dev);
    active_stream_TX(dev);
    
}

std::vector<int> gen_rand(int size){
    std::vector<int> mass;
    for (int i = 0; i < size; i++){
        mass.push_back(rand()%2);
    }
    return mass;
}

int main(int argc, char* argv[])
{

    set_sdr dev_tx;
    memset(&dev_tx, 0, sizeof(dev_tx)); 

    printf("TX enable !!!!\n");
    set_parametrs_TX(&dev_tx, "ip:192.168.2.1"); 

    int smpl_in_symbl = 10;
    // std::vector<int8_t> bits = {1,1,1,1,1,1,1,1,0,0,1,0,1,0,1,1,0,1,0,1,0,1,1,1,1,0};

    std::vector<int> bits = gen_rand(1920);

    std::vector<complex_d> bpsk_sym = qpsk(bits);

    std::vector<complex_d> upbits = upsampling(bpsk_sym, smpl_in_symbl);

    // print_vec(bpsk_sym);

    std::vector<complex_d> pulse_vec = rect_pulse(smpl_in_symbl);
    // std::vector<complex_d> pulse_vec = raised_cosine(smpl_in_symbl);
    std::vector<complex_d> conv = convolve3(upbits, pulse_vec);
    // print_vec(bits, bits.size());
    // print_vec(conv, 10);

    tx_loop(&dev_tx, conv);
    shutdown_TX(&dev_tx);


    // trx_samples_buff_2sdr(&dev_rx, &dev_tx, conv, 1);
    // // trx_samples_buff_repeat(&dev1, conv); 
}