#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <fstream>

#define BLOCK_SAMPLES 1920 
#define TIME_OFFSET_NS (4 * 1000 * 1000) 

typedef struct settings_sdr{
    
    SoapySDRKwargs args;
    SoapySDRDevice *sdr;
    SoapySDRStream *rxStream;
    SoapySDRStream *txStream;
    size_t rx_mtu;
    size_t tx_mtu;

}set_sdr;

int set_args(set_sdr *device);

void set_sample_rate(set_sdr *device, int direction, double rate);

void set_frequency(set_sdr *device, int direction, double frequency);

double get_frequency(set_sdr *device, int direction);

int setup_stream(set_sdr *device, int direction, size_t channels[], size_t channel_count);

void set_gain(set_sdr *device, int direction, size_t channels[], double gain);

void get_MTU(set_sdr *device);

void active_stream(set_sdr *device);

void shutdown(set_sdr *device);

void trx_samples(set_sdr *device, int16_t *samples, int size_sample);

