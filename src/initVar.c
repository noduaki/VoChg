#include "header.h"

int initVar(VApp* data){
    int error = 0;
    GError* fileErr = NULL;
    GFileInputStream *istream;

    if(data->status.selNum == 1){
        data->dataBuf.read = (char*)malloc(SOUNDFRAMES * 2 * SOUNDCHANNELS);
        if(data->dataBuf.read == NULL) error = 1;
        data->dataBuf.write = (char*)malloc(SOUNDFRAMES * 2 * SOUNDCHANNELS);
        if(data->dataBuf.write == NULL)error = 1;
        data->dataBuf.sound = (double*)malloc(SOUNDFRAMES * SOUNDCHANNELS * sizeof(double));
        if(data->dataBuf.sound == NULL)error = 1;
        data->dataBuf.fft = (double*)malloc(SOUNDFRAMES * SOUNDCHANNELS * sizeof(double));
        if(data->dataBuf.fft == NULL)error = 1;

        

        data->settings = gSet;
        if(gSet.file != NULL){
            istream = g_file_read(gSet.file, NULL, &fileErr);
            if (fileErr != NULL){
                printf("Could not open %s for reading: %s \n", gSet.filename, fileErr->message);
                
                data->flag.soundFile = 0;
            }
            g_input_stream_close (G_INPUT_STREAM (istream), NULL, &fileErr);
            
            g_clear_object(&gSet.file);
        }
       
        data->soundRead.samples = (unsigned char*)malloc(data->settings.buffersize);
        if(data->soundRead.samples == NULL) error = 1;
        data->soundWrite.samples = (unsigned char*)malloc(data->settings.buffersize);
        if(data->soundWrite.samples == NULL) error = 1;
        data->soundRead.areas = (snd_pcm_channel_area_t*)malloc(sizeof(snd_pcm_channel_area_t) * 2);
        if(data->soundRead.areas == NULL)error = 1;
        data->soundWrite.areas = (snd_pcm_channel_area_t*)malloc(sizeof(snd_pcm_channel_area_t) * 2);
        if(data->soundWrite.areas == NULL)error = 1;
        data->soundRead.phase  = 0.0;
        data->soundWrite.phase = 0.0;
        data->soundRead.pos      = 0;
        data->soundWrite.pos     = 0;
        data->soundRead.ready    = 0;
        data->soundWrite.ready   = 0;
        

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
        free(data->dataBuf.read);data->dataBuf.read = NULL;
        free(data->dataBuf.write);data->dataBuf.write = NULL;
        free(data->dataBuf.sound); data->dataBuf.sound = NULL;
        free(data->dataBuf.fft); data->dataBuf.fft = NULL;

        free(data->soundRead.samples); data->soundRead.samples = NULL;
        free(data->soundWrite.samples); data->soundWrite.samples = NULL;
        free(data->soundRead.areas); data->soundRead.areas = NULL;
        free(data->soundWrite.areas); data->soundWrite.areas = NULL;
        data->status.selNum = 0;
        data->flag.soundFile = 0;

    }else if(data->status.selNum == 2){

    }else if(data->status.selNum == 3){

    }else{
        printf("Error in delVar");
        exit(1);
    }
    return error;

}