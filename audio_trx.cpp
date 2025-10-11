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

#include "include/soapy_api.hpp"

#define BLOCK_SAMPLES 1920 
#define TIME_OFFSET_NS (4 * 1000 * 1000) 


int16_t *read_pcm(const char *filename, size_t *sample_count)
{
    FILE *file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("file_size = %ld\n", file_size);
    int16_t *samples = (int16_t *)malloc(file_size);

    *sample_count = file_size / sizeof(int16_t);

    fread(samples, sizeof(int16_t), *sample_count, file);
    fclose(file);

    return samples;
}

int main(void)
{
    set_sdr dev1;
    memset(&dev1, 0, sizeof(dev1)); // обнуляем
    size_t channels[] = {0};
    size_t channel_count = sizeof(channels) / sizeof(channels[0]);

    set_args(&dev1);
    set_sample_rate(&dev1, SOAPY_SDR_RX, 1e6);
    set_sample_rate(&dev1, SOAPY_SDR_TX, 1e6);
    set_frequency(&dev1, SOAPY_SDR_RX, 800e6);
    set_frequency(&dev1, SOAPY_SDR_TX, 800e6);
    setup_stream(&dev1, SOAPY_SDR_RX, channels, channel_count);
    setup_stream(&dev1, SOAPY_SDR_TX, channels, channel_count);
    set_gain(&dev1, SOAPY_SDR_RX, channels, 10);
    set_gain(&dev1, SOAPY_SDR_TX, channels, -50);

    get_MTU(&dev1);
    active_stream(&dev1);
    // tx_rx(&dev1);

    size_t count = 0;
    int16_t *pcm = read_pcm("../resurse/in/ANNA_ASTI.pcm", &count);
    printf("count = %ld\n", count );

    trx_samples(&dev1, pcm, count);
    shutdown(&dev1);
    
    // FILE *file = fopen("../file/out/rxdata_etalon.pcm", "w");
    
    // fwrite(pcm, count * sizeof(int16_t), 1, file);
    // fclose(file);

    free(pcm);
}