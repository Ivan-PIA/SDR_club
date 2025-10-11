#include "../include/soapy_api.hpp"

int set_args(set_sdr *device){
    
    device->args = {};
    SoapySDRKwargs_set(&device->args, "driver", "plutosdr");
    if (1) {
        SoapySDRKwargs_set(&device->args, "uri", "usb:");
    } else {
        SoapySDRKwargs_set(&device->args, "uri", "ip:192.168.2.1");
    }

    SoapySDRKwargs_set(&device->args, "direct", "1");
    SoapySDRKwargs_set(&device->args, "timestamp_every", "1920");
    SoapySDRKwargs_set(&device->args, "loopback", "0");

    device->sdr = SoapySDRDevice_make(&device->args);
    SoapySDRKwargs_clear(&device->args);

    if (device->sdr == NULL)
    {
        printf("SoapySDRDevice_make fail: %s\n", SoapySDRDevice_lastError());
        return EXIT_FAILURE;
    }
    return 0;
}

void set_sample_rate(set_sdr *device, int direction, double rate){

    if (SoapySDRDevice_setSampleRate(device->sdr, direction, 0, rate) != 0)
    {
        printf("setSampleRate fail: %s\n", SoapySDRDevice_lastError());
    }   
}

void set_frequency(set_sdr *device, int direction, double frequency){

    if (SoapySDRDevice_setFrequency(device->sdr, direction, 0, frequency, NULL) != 0)
    {
        printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
    }  
}

double get_frequency(set_sdr *device, int direction){

    double freq = SoapySDRDevice_getFrequency(device->sdr, direction, 0);

    return freq;
}

int setup_stream(set_sdr *device, int direction, size_t channels[], size_t channel_count){


    if (direction == SOAPY_SDR_RX){
        device->rxStream = SoapySDRDevice_setupStream(device->sdr, direction, SOAPY_SDR_CS16, channels, channel_count, NULL);
        if (device->rxStream == NULL){
            printf("setupStream rx fail: %s\n", SoapySDRDevice_lastError());
            SoapySDRDevice_unmake(device->sdr);
            return EXIT_FAILURE;
        }
    }

    if (direction == SOAPY_SDR_TX){
        device->txStream = SoapySDRDevice_setupStream(device->sdr, direction, SOAPY_SDR_CS16, channels, channel_count, NULL);
        if (device->txStream == NULL){
            printf("setupStream tx fail: %s\n", SoapySDRDevice_lastError());
            SoapySDRDevice_unmake(device->sdr);
            return EXIT_FAILURE;
        }
    }
    return 0;
}

void set_gain(set_sdr *device, int direction, size_t channels[], double gain){

    if(SoapySDRDevice_setGain(device->sdr, direction, channels[0], gain) !=0 ){
        printf("setGain fail: %s\n", SoapySDRDevice_lastError());
    }    
}

void get_MTU(set_sdr *device){
    device->rx_mtu = SoapySDRDevice_getStreamMTU(device->sdr, device->rxStream);
    device->tx_mtu = SoapySDRDevice_getStreamMTU(device->sdr, device->txStream);
}

void active_stream(set_sdr *device){
    SoapySDRDevice_activateStream(device->sdr, device->rxStream, 0, 0, 0); //start streaming
    SoapySDRDevice_activateStream(device->sdr, device->txStream, 0, 0, 0); //start streaming
}

void shutdown(set_sdr *device){

    //stop streaming
    SoapySDRDevice_deactivateStream(device->sdr, device->rxStream, 0, 0);
    SoapySDRDevice_deactivateStream(device->sdr, device->txStream, 0, 0);

    //shutdown the stream
    SoapySDRDevice_closeStream(device->sdr, device->rxStream);
    SoapySDRDevice_closeStream(device->sdr, device->txStream);

    //cleanup device handle
    SoapySDRDevice_unmake(device->sdr);
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

    FILE *file = fopen("../resurse/out/rxdata_audio_2.pcm", "wb");
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

        // if (st < 0)
        //     printf("TX[%zu] failed: %d\n", blk, st);
        // else
        //     printf("TX[%zu]: %d samples sent\n", blk, st);
    }

    fclose(file);
    free(rx_buff);

    printf("TX/RX complete!\n");
}