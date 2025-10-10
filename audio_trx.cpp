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

int tx_rx(set_sdr *device){

    printf("MTU - TX: %lu, RX: %lu\n", device->tx_mtu, device->rx_mtu);
    int16_t tx_buff[2*device->tx_mtu];
    // заполнение tx_buff значениями сэмплов первые 16 бит - I, вторые 16 бит - Q.
    float x = -96;
    for (int i = 0; i < 2 * int(device->rx_mtu); i+=2)
    {
        tx_buff[i] = x * x;
        tx_buff[i+1] = x * x;
        x += 0.1;
    }

    //here goes
    printf("Start test...\n");
    const long  timeoutUs = 400000; // arbitrarily chosen (взяли из srsRAN)

    int16_t *buffer = (int16_t *)malloc(2 * device->rx_mtu * sizeof(int16_t));
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    long long last_time = 0;

    // Создаем файл для записи сэмплов с rx_buff
    FILE *file = fopen("txdata.pcm", "w");
    
    // Количество итерация
    size_t iteration_count = 10;
    // Начинается работа с получением и отправкой сэмплов
    for (size_t buffers_read = 0; buffers_read < iteration_count; buffers_read++)
    {
        void *rx_buffs[] = {buffer};
        int flags;        // flags set by receive operation
        long long timeNs; //timestamp for receive buffer

        int sr = SoapySDRDevice_readStream(device->sdr, device->rxStream, rx_buffs, device->rx_mtu, &flags, &timeNs, timeoutUs);


        printf("Buffer: %lu - Samples: %i, Flags: %i, Time: %lli, TimeDiff: %lli\n", buffers_read, sr, flags, timeNs, timeNs - last_time);
        fwrite(buffer, 2* device->rx_mtu * sizeof(int16_t), 1, file);
        last_time = timeNs;

        // Calculate transmit time 4ms in future
        long long tx_time = timeNs + (4 * 1000 * 1000);

        // Send buffer
        void *tx_buffs[] = {tx_buff};
        if( (buffers_read == 2) ){
            flags = SOAPY_SDR_HAS_TIME;
            int st = SoapySDRDevice_writeStream(device->sdr, device->txStream, (const void * const*)tx_buffs, device->tx_mtu, &flags, tx_time, 400000);
            if ((size_t)st != device->tx_mtu)
            {
                printf("TX Failed: %i\n", st);
            }
        }
        
    }

    fclose(file);

    //all done
    printf("test complete!\n");

    return EXIT_SUCCESS;
}



void trx_samples(set_sdr *device, int16_t *samples, int size_sample)
{
    const long timeoutUs = 400000;
    const size_t block_len = 2 * BLOCK_SAMPLES; // I + Q
    const size_t total_blocks = size_sample / block_len;

    if (total_blocks == 0) {
        printf("trx_samples: not enough samples to process (need >= %zu)\n", block_len);
        return;
    }

    int16_t *rx_buff = (int16_t *)malloc(2 * device->rx_mtu * sizeof(int16_t));
    if (!rx_buff) {
        printf("RX buffer allocation failed\n");
        return;
    }

    FILE *file = fopen("../file/out/rxdata_audio.pcm", "wb");
    if (!file) {
        printf("Failed to open rxdata.pcm\n");
        free(rx_buff);
        return;
    }

    printf("Starting TX/RX streaming... (%zu blocks)\n", total_blocks);
    long long last_time = 0;

    for (size_t blk = 0; blk < total_blocks; blk++)
    {
        void *rx_buffs[] = {rx_buff};
        int flags_rx = 0;
        long long timeNs = 0;

        int sr = SoapySDRDevice_readStream(
            device->sdr, device->rxStream,
            rx_buffs, device->rx_mtu,
            &flags_rx, &timeNs, timeoutUs
        );

        fwrite(rx_buff, sizeof(int16_t), 2 * device->rx_mtu, file);

        printf("RX[%zu]: %d samples, Δt=%lli ns\n", blk, sr, timeNs - last_time);
        last_time = timeNs;

        void *tx_buffs[] = {samples + blk * block_len};
        int flags_tx = SOAPY_SDR_HAS_TIME;
        long long tx_time = timeNs + TIME_OFFSET_NS; 

        int st = SoapySDRDevice_writeStream(
            device->sdr, device->txStream,
            (const void *const *)tx_buffs,
            BLOCK_SAMPLES,
            &flags_tx, tx_time, timeoutUs
        );

        if (st < 0)
            printf("TX[%zu] failed: %d\n", blk, st);
        else
            printf("TX[%zu]: %d samples sent\n", blk, st);
    }

    fclose(file);
    free(rx_buff);

    printf("TX/RX complete!\n");
}

int16_t *read_pcm(const char *filename, size_t *sample_count)
{
    FILE *file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int16_t *samples = (int16_t *)malloc(file_size);

    *sample_count = file_size / sizeof(int16_t);

    size_t read_count = fread(samples, sizeof(int16_t), *sample_count, file);
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
    int16_t *pcm = read_pcm("../file/in/Smeshariki.pcm", &count);
    

    trx_samples(&dev1, pcm, count);
    shutdown(&dev1);
    
    // FILE *file = fopen("../file/out/rxdata_etalon.pcm", "w");
    
    // fwrite(pcm, count * sizeof(int16_t), 1, file);
    // fclose(file);

    free(pcm);
}