#include "header.h"

int initVar(VApp* data){
    int error = 0;
    int ok = 0;
    GError* fileErr = NULL;
    GFileInputStream *inStream = NULL;
    GFileInfo *info = NULL;
    int total_size = -1;
    gboolean err = 0;
    size_t length = 0;

    if(data->status.selNum == 1 || data->status.selNum == 2){
        data->settings = gSet;

        data->dataBuf.sound = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if(data->dataBuf.sound == NULL)error = 1;
        data->dataBuf.row = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if(data->dataBuf.row == NULL)error = 1;
        data->dataBuf.fft = (double*)malloc(data->settings.frames * data->settings.channels * sizeof(double));
        if(data->dataBuf.fft == NULL)error = 1;

        
        if(gSet.file != NULL){
            inStream = g_file_read(gSet.file, NULL, &fileErr);
            if (fileErr != NULL){
                printf("Could not open %s for reading: %s \n", gSet.filename, fileErr->message);
                data->flag.soundFile = 0;
            }
            info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (inStream),G_FILE_ATTRIBUTE_STANDARD_SIZE,NULL, &fileErr);
            if (info){
                if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                    total_size = g_file_info_get_size (info);
                    printf( "total_size = %d\n", total_size);
                    
            }else {
                printf("Error in initVar -> Can't get info\n");
                data->flag.soundFile = 0;
            }

            if(total_size > 0){
                if(data->dataBuf.read == NULL){
                    data->dataBuf.read = (int16_t*) malloc(total_size * 2);
                    data->dataBuf.write = (int16_t*) malloc(total_size);
                    memset(data->dataBuf.read, 0, total_size);
                    if ((length = g_input_stream_read (G_INPUT_STREAM(inStream), data->dataBuf.read,
                                                                     total_size, NULL, &fileErr)) != -1) {
                            data->dataBuf.readSize = length;
                            printf( "reading file length = %ld\n", length);
                    }else{
                        data->dataBuf.readSize = 0;
                        data->flag.soundFile = 0;
                        printf("Error in initVar -> file read\n" );
                    }
                    
                }else {
                    data->flag.soundFile = 0;
                    printf("Error in initVar -> Buffer busy\n");
                }
                
            }
            err = g_input_stream_close (G_INPUT_STREAM (inStream), NULL, &fileErr);
            if(err == FALSE) printf("Error in initVar -> stream_close failed\n");
            g_object_unref(inStream);
            g_object_unref (info);
            g_clear_object(&gSet.file);
        }
        if(data->flag.soundFile == 1){
            ok = wavCheck(data);
            if(ok){
                data->flag.soundFile = 0;
                error = 1;
            }
        }
        
       
        data->soundRead.samples = (int16_t*)malloc(data->settings.frames * data->settings.channels * sizeof(short));
        if(data->soundRead.samples == NULL) error = 1;
        data->soundWrite.samples = (int16_t*)malloc(data->settings.frames * data->settings.channels * sizeof(short));
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
        

    }else if(data->status.selNum == 3){

    }else{
        printf("Error in initVar");
        exit(1);
    }
    return error;
}
int delVar(VApp* data){
    int error = 0;


    if(data->status.selNum == 1 || data->status.selNum == 2){
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

    }else if(data->status.selNum == 3){

    }else{
        printf("Error in delVar");
        exit(1);
    }
    return error;

}