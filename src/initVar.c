#include "header.h"

int initVar(VApp* data){
    int error = 0;

    if(data->status.selNum == 1){
        data->dataBuf.read = (char*)malloc(SOUNDFRAMES * 2 * SOUNDCHANNELS);
        if(data->dataBuf.read == NULL) error = 1;

        data->dataBuf.write = (char*)malloc(SOUNDFRAMES * 2 * SOUNDCHANNELS);
        if(data->dataBuf.write == NULL)error = 1;

        data->dataBuf.sound = (double*)malloc(SOUNDFRAMES * SOUNDCHANNELS * sizeof(double));
        if(data->dataBuf.sound == NULL)error = 1;

        data->dataBuf.fft = (double*)malloc(SOUNDFRAMES * SOUNDCHANNELS * sizeof(double));
        if(data->dataBuf.fft == NULL)error = 1;

    }else if(data->status.selNum == 2){

    }else if(data->status.selNum == 3){

    }else{
        printf("Error in initVar");
        exit(1);
    }
    return error;
}
int delVar(VApp* data){
    int error = 0;

    if(data->status.selNum == 1){

    }else if(data->status.selNum == 2){

    }else if(data->status.selNum == 3){

    }else{
        printf("Error in initVar");
        exit(1);
    }
    return error;

}