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
        printf("setFrequency rx fail: %s\n", SoapySDRDevice_lastError());
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
            printf("setupStream rx fail: %s\n", SoapySDRDevice_lastError());
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