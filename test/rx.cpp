#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

#include "../include/device/soapy_api.hpp"

#define BLOCK_SAMPLES 1920 

template <typename T>
void print_vec(const std::vector<T>& vec, int in_row) {

    for (int i = 0; i < (int)vec.size(); i++){
        
        if(i%in_row == 0 && i!=0){
            std::cout << std::endl;
        }
        std::cout << vec[i] << ", ";

    }
    std::cout << std::endl;
}


void set_parametrs(set_sdr *dev, const char *usb){
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(dev, usb);
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
    set_sample_rate(dev, SOAPY_SDR_TX, 1e6);
    set_frequency(dev, SOAPY_SDR_TX, 870e6);
    setup_stream_TX(dev, SOAPY_SDR_TX, channels, channel_count);
    set_gain(dev, SOAPY_SDR_TX, channels, -10);
    get_MTU_TX(dev);
    active_stream_TX(dev);
    
}

int main(int argc, char* argv[])
{
    set_sdr dev_rx;
    memset(&dev_rx, 0, sizeof(dev_rx)); 

    printf("RX enable !!!!\n");
    set_parametrs_RX(&dev_rx, "ip:192.168.3.1"); 

    rx_loop(&dev_rx);
    shutdown_RX(&dev_rx);
}
