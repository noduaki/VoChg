#include "header.h"

snd_pcm_t *Phandle;
snd_pcm_t *Chandle; //for Connect stream 
GMutex mutex_sound;

void pcm_list(snd_pcm_stream_t stream, char* namedevice)
{
      void **hints, **n;
      char *name, *descr, *descr1, *io;
      const char *filter;

      if (snd_device_name_hint(-1, "pcm", &hints) < 0)
            return ;
      n = hints;
      filter = stream == SND_PCM_STREAM_CAPTURE ? "Input" : "Output";
      while (*n != NULL) {
            name = snd_device_name_get_hint(*n, "NAME");
            
            descr = snd_device_name_get_hint(*n, "DESC");
            io = snd_device_name_get_hint(*n, "IOID");
            if (io != NULL && strcmp(io, filter) == 0){
                goto __free;
            }
                 
			if(*(name) == 'h' && *(name + 1) == 'w'){	  
            	printf("%s\n", name);
                strcpy(namedevice, name);
                break;
            } 
              __free:
           
            if (name != NULL)
                   free(name);
            if (descr != NULL)
                   free(descr);
            if (io != NULL)
                   free(io);
            n++;     
    }
    if (name != NULL)
       free(name);
    if (descr != NULL)
       free(descr);
    if (io != NULL)
       free(io);
   
    return ;
}

int setparams_stream(snd_pcm_t *handle, snd_pcm_hw_params_t *params, const char *id, gpointer data)
{
    VApp* da = (VApp*)data;
    int err;
    unsigned int rrate;
    

    err = snd_pcm_hw_params_any(handle, params);
    if (err < 0) {
        printf("Broken configuration for %s PCM: no configurations available: %s\n", snd_strerror(err), id);
        return err;
    }
    err = snd_pcm_hw_params_set_rate_resample(handle, params, 1);
    if (err < 0) {
        printf("Resample setup failed for %s (val %i): %s\n", id, 1, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        printf("Access type not available for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_set_format(handle, params, da->settings.format);
    if (err < 0) {
        printf("Sample format not available for %s: %s\n", id, snd_strerror(err));
        return err;
    }

    if(handle == Phandle)err = snd_pcm_hw_params_set_channels(handle, params, da->settings.channels);
    else err = snd_pcm_hw_params_set_channels(handle, params, da->settings.channels);
   
    
    if (err < 0) {
        printf("Channels count (%i) not available for %s: %s\n", da->settings.channels, id, snd_strerror(err));
        return err;
    }

    rrate = da->settings.rate;
  
    err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
    if (err < 0) {
        printf("Rate %iHz not available for %s: %s\n", da->settings.rate, id, snd_strerror(err));
        return err;
    }
    if ((int)rrate != da->settings.rate) {
        printf("Rate doesn't match (requested %iHz, get %iHz)\n",da->settings.rate, rrate);
       return -EINVAL;
    }
    
    
    return 0;
}
int setparams_bufsize(snd_pcm_t *handle,
              snd_pcm_hw_params_t *params,
              snd_pcm_hw_params_t *tparams,
              const char *id, gpointer data)
             
{
    VApp* da = (VApp*)data;
    int err;
    static int i = 0;
    char srate[10] = {0};
    snd_pcm_uframes_t periodsize;
    snd_pcm_uframes_t buffersize;
    snd_pcm_hw_params_copy(params, tparams);
    buffersize = da->settings.pcm_buffer_size;
    err = snd_pcm_hw_params_set_buffer_size_near(handle, params, &buffersize);
    if(buffersize == da->settings.pcm_buffer_size)   
        printf("++++++++set buffer size %li \n", buffersize);
    else printf("Bferr Size faile!!!");

    da->soundRead.bufferSize = buffersize;
    da->soundWrite.bufferSize = buffersize;
   
    periodsize = da->settings.period_size;
    
  
   
    err = snd_pcm_hw_params_set_period_size_near(handle, params, &periodsize, 0);
    if(periodsize == da->settings.period_size)
        printf(">>>period size %li++++%ld \n", periodsize, da->settings.period_size);
    else {
        printf("PeriodeSizeFaile");
        exit(1);
    }
    da->soundRead.periodsize = periodsize;
    da->soundWrite.periodsize = periodsize;
    if(!i){
        strcat(da->statusBuf, " sound card buffer--");
        sprintf(srate, "%ld", buffersize);
        strcat(da->statusBuf,srate);
        
        strcat(da->statusBuf, "byes period--");
        sprintf(srate, "%ld", periodsize);
        strcat(da->statusBuf,srate);
        i++;
    }else i = 0;

    
    return 0;
}
int setparams_set(snd_pcm_t *handle,
          snd_pcm_hw_params_t *params,
          snd_pcm_sw_params_t *swparams,
          const char *id)
{

    int err;
    snd_pcm_uframes_t val;
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        printf("Unable to set hw params for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    err = snd_pcm_sw_params_current(handle, swparams);
    if (err < 0) {
        printf("Unable to determine current swparams for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    err = snd_pcm_sw_params_set_start_threshold(handle, swparams, 64);
    if (err < 0) {
        printf("Unable to set start threshold mode for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    
    val = snd_pcm_hw_params_get_period_size(params, &val, NULL);
    
    err = snd_pcm_sw_params_set_avail_min(handle, swparams, val);
    if (err < 0) {
        printf("Unable to set avail min for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    err = snd_pcm_sw_params(handle, swparams);
    if (err < 0) {
        printf("Unable to set sw params for %s: %s\n", id, snd_strerror(err));
        return err;
    }
    return 0;
}
int setparams(snd_pcm_t *phandle, snd_pcm_t *chandle,  gpointer data)
{
    VApp* da = (VApp*)data;
    int ok;
    int err, last_bufsize = da->settings.pcm_buffer_size;
    snd_pcm_hw_params_t *pt_params, *ct_params; // templates with rate, format and channels 
    snd_pcm_hw_params_t *p_params, *c_params;
    snd_pcm_sw_params_t *p_swparams, *c_swparams;
    snd_pcm_uframes_t p_size, c_size, p_psize, c_psize;
    unsigned int p_time, c_time;
    unsigned int val;
    snd_pcm_hw_params_alloca(&p_params);
    snd_pcm_hw_params_alloca(&c_params);
    snd_pcm_hw_params_alloca(&pt_params);
    snd_pcm_hw_params_alloca(&ct_params);
    snd_pcm_sw_params_alloca(&p_swparams);
    snd_pcm_sw_params_alloca(&c_swparams);
    if ((err = setparams_stream(phandle, pt_params, "playback", data)) < 0) {
        printf("Unable to set parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
    if ((err = setparams_stream(chandle, ct_params, "capture", data)) < 0) {
        printf("Unable to set parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
    
    if ((err = setparams_bufsize(phandle, p_params, pt_params,  "playback", data)) < 0) {
        printf("Unable to set sw parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
    if ((err = setparams_bufsize(chandle, c_params, ct_params,  "capture", data)) < 0) {
        printf("Unable to set sw parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
  
  
    if ((err = setparams_set(phandle, p_params, p_swparams, "playback")) < 0) {
        printf("Unable to set sw parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
    if ((err = setparams_set(chandle, c_params, c_swparams, "capture")) < 0) {
        printf("Unable to set sw parameters for playback stream: %s\n", snd_strerror(err));
        exit(0);
    }
    if ((err = snd_pcm_prepare(phandle)) < 0) {
        printf("Prepare error: %s\n", snd_strerror(err));
        exit(0);
    }
    ok = snd_pcm_hw_params_can_pause(p_params);
    if(ok) printf("Pause Ok read\n");
    else printf("NO read\n");
    ok = snd_pcm_hw_params_can_pause(c_params);
    if(ok) printf("Pause Ok write\n");
    else printf("NO write\n");

    printf(">>>period size ++++%ld \n",  da->settings.period_size );
    
    return 0;
}



void gettimestamp(snd_pcm_t *handle, snd_timestamp_t *timestamp)
{
    int err;
    snd_pcm_status_t *status;
    snd_pcm_status_alloca(&status);
    if ((err = snd_pcm_status(handle, status)) < 0) {
        printf("Stream status error: %s\n", snd_strerror(err));
        exit(0);
    }
    snd_pcm_status_get_trigger_tstamp(status, timestamp);
}

 
static int xrun_recovery(snd_pcm_t *handle, int err)
{
    if(handle == Phandle) printf("Playback handle stream recovery\n");
    else printf("Captuer handle stream recovery\n");   
   
    if (err == -EPIPE) {    //* under-run 
        err = snd_pcm_prepare(handle);
        if (err < 0)
            printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
        return 0;
    } else if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);   //* wait until the suspend flag is released 
        if (err < 0) {
            err = snd_pcm_prepare(handle);
            if (err < 0)
                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
        }
        return 0;
    }

    if(handle != Phandle) if((snd_pcm_drop(handle)) < 0) printf("recovery Error \n");
    return err;
}


void async_read_callback(snd_async_handler_t *ahandler)
{ 
    snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
    mAsyncData* asyData = snd_async_handler_get_callback_private(ahandler);
   
   
    int length = asyData->periodsize * 4;
    int pos = 0;
    int charBufferSize;
    charBufferSize = asyData->bufferSize * 4;
    
    snd_pcm_sframes_t avail;
    int err;
    g_mutex_lock(&mutex_sound);
    avail = snd_pcm_avail(handle);
    while (avail >= asyData->periodsize){
        if(pos < charBufferSize - length && pos >= 0){ 
            
            err = snd_pcm_readi(handle, asyData->samples + pos, asyData->periodsize);
            if(err != asyData->periodsize) printf("Read number is under period ->>%d", err);
            pos += length;         
            
        }else if(pos >= charBufferSize - length && pos < charBufferSize){
            
            err = snd_pcm_readi(handle, asyData->samples, asyData->periodsize); 
            if(err != asyData->periodsize) printf("Read number is under period ->>%d", err);
            pos += length;
            
        }else{
            err = snd_pcm_readi(handle, asyData->samples, asyData->periodsize); 
        
            printf("async-READ BUFFER ERROR %d\n", asyData->pos);
            pos = 0;
        } 
        
        if (err < 0) {
            if ((err = xrun_recovery(handle, err)) < 0) {
                printf("async-read Recovery error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
        avail = snd_pcm_avail(handle);
    } 
    asyData->pos = pos;
    g_mutex_unlock(&mutex_sound);
    asyData->ready = 1;
      
}

void async_write_callback(snd_async_handler_t *ahandler)

{
    snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
    mAsyncData* data = snd_async_handler_get_callback_private(ahandler);
    snd_pcm_sframes_t avail;
   
    
    int err;
    int length = data->periodsize * 4;
    int pos = data->pos;
    int charBufferSize;
    charBufferSize = data->bufferSize * 4;
    
    avail = snd_pcm_avail(handle);
    g_mutex_lock(&mutex_sound);    
    while (avail >= data->periodsize) {
            
        err = snd_pcm_writei(handle, data->samples + data->pos, data->periodsize);
        if (err != data->periodsize) {
            printf("async-Write error: written %i expected %d\n", err, data->periodsize);
            exit(EXIT_FAILURE);
        }
        if (err < 0) {
            printf("async-Write error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }
        
        pos += length;
        if(pos >= charBufferSize)pos = charBufferSize - pos;
        
        avail = snd_pcm_avail(handle);
            
        if (err < 0) {
            if ((err = xrun_recovery(handle, err)) < 0) {
                printf("async-write Recovery error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
    }
    data->pos = pos;
    g_mutex_unlock(&mutex_sound);
    
}

void initSound(GTask *stask, gpointer source_object, gpointer data, GCancellable *cancellable){

    VApp* da = (VApp*)data;
    
    snd_async_handler_t *ahandler;
    snd_async_handler_t *whandler; 
    int latency, morehelp;
    int ok;
    int ok2;
    int err;
    int i;
   
    snd_timestamp_t p_tstamp, c_tstamp;
    ssize_t r;
    size_t frames_in, frames_out, in_max, frames;
    char statustext[40] = {0};
    
    char srate[10] = {0};
    char sformat[15] = {0};
   
    char cardperiod[6] = {0};
    struct timespec  req  = {0, 100000};

    da->status.ref++;
    signed short tmp;
    

    for(i = 0; i < SOUNDFRAMES; i++){
        tmp = (signed short)((sin(((6.283185 * 32.0) / SOUNDFRAMES) * (double)i)) * 10000.0);
        
        *(da->soundWrite.samples + (i * 4) ) = (char)(tmp & 0x00ff);
        *(da->soundWrite.samples + (i * 4) + 1) = (char)((tmp & 0xff00) >> 8);
        *(da->soundWrite.samples + (i * 4) + 2) = (char)(tmp & 0x00ff);
        *(da->soundWrite.samples + (i * 4) + 3) = (char)((tmp & 0xff00) >> 8);
    }
    //pcm_list(SND_PCM_STREAM_CAPTURE, da->settings.deviceName);  
   
    if ((err = snd_pcm_open(&Phandle, da->settings.deviceName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        statusprint("Playback open error", data);

        return ;
    }
    
    if ((err = snd_pcm_open(&Chandle, da->settings.deviceName, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        printf("Record open error: %s\n", snd_strerror(err));
        statusprint("Record open error", data);
        return ;
    }
    
    //statusbar shown after strings
    strcpy(statustext, da->settings.deviceName);
    strcpy(da->statusBuf, "Device--");
    strcat(da->statusBuf, statustext);
    
    
    strcpy(sformat, "  LE16bit  ");
    strcat(da->statusBuf, sformat);
   
    sprintf(srate, "%d", da->settings.channels);
    strcat(da->statusBuf,srate);
    strcat(da->statusBuf, "ch  ");   
    sprintf(srate, "%d", da->settings.rate);
    strcat(da->statusBuf,srate);
    strcat(da->statusBuf, "hz  frames--");
    sprintf(srate, "%d", da->settings.frames);
    strcat(da->statusBuf,srate);

    frames_in = frames_out = 0;
    
    if (setparams(Phandle, Chandle,  data) < 0){
        printf("Sound Card Settings Error\n");
    }

    if(!(da->flag.soundFile) ){    
        if ((err = snd_pcm_link(Phandle, Chandle)) < 0) {
            printf("Streams link error: %s\n", snd_strerror(err));
            exit(0);
        }
    }
    if (snd_pcm_format_set_silence(da->settings.format, da->dataBuf.read, 
                                da->settings.channels * da->settings.frames) < 0) {
        fprintf(stderr, "silence error\n");
    } 
    gettimestamp(Phandle, &p_tstamp);
    gettimestamp(Chandle, &c_tstamp);
    ok = 1;
   
    err = snd_async_add_pcm_handler(&ahandler, Chandle, async_read_callback, &da->soundRead);
    if (err < 0) {
        printf("Unable to register async handler>>>>>>>>>>>>>read.\n");
        statusprint("Unable to register async handler Read Sound Card fail", data);
        goto err;
    }
    
    err = snd_async_add_pcm_handler(&whandler, Phandle, async_write_callback, &da->soundWrite);
    if (err < 0) {
        printf("Unable to register async handler>>>>>>>>>>>>>write.\n");
        statusprint("Unable to register async handler Write Sound Card Fail", data);
        goto err;
    }
    //Write Buffer initial writeing twice
    err = snd_pcm_writei(Phandle, da->dataBuf.write, da->settings.period_size);
        
    if (err < 0) {
        printf("Initial write error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    if (err != da->settings.period_size) {
        printf("Initial write error: written %i expected %ld\n", err, da->settings.period_size);
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_writei(Phandle, da->dataBuf.write, da->settings.period_size);
        
    if (err < 0) {
        printf("Initial write error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    if (err != da->settings.period_size) {
        printf("Initial write error: written %i expected %ld\n", err, da->settings.period_size);
        exit(EXIT_FAILURE);
    }
    if(da->flag.soundFile){
        
        if (snd_pcm_state(Phandle) == SND_PCM_STATE_PREPARED) {
            
            err = snd_pcm_start(Phandle);
            if (err < 0) {
                printf("Start error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
        if(da->flag.soundFile){
            strcat(da->statusBuf, " open file -->");
            strcat(da->statusBuf, da->settings.filename);
        }else{
             strcat(da->statusBuf, "--TestWaveMode");
        }
    }else{
       if (snd_pcm_state(Chandle) == SND_PCM_STATE_PREPARED) {
            
            err = snd_pcm_start(Chandle);
            if (err < 0) {
                printf("Start error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        } 
        
    }
    
    printf("Start \n");

    statusprint(da->statusBuf, data);
   

    //START SOUND CARD*************************************************      
    int posr = 0;
    int posw = 0;
    int postmp; 
    int charBufferSize = da->soundRead.bufferSize * 4;  
    while(da->status.open) {
        posr = da->soundRead.pos;
        
        g_mutex_lock(&mutex_sound);
        if(da->soundRead.ready){
            if(posw + posr <= charBufferSize && posw + posr >= 0){
                memcpy(da->soundWrite.samples + posw, da->soundRead.samples, posr);
                posw = posw + posr;

            }else if(posw + posr > charBufferSize){
                postmp = charBufferSize - posw;
                if(postmp > 0)
                    memcpy(da->soundWrite.samples + posw, da->soundRead.samples, postmp);
                posw = posr - postmp;
                memcpy(da->soundWrite.samples, da->soundRead.samples + postmp, posw);
                
            }else{
                printf("Error soundinit()-> RW data transfer");
                exit(1);
            }
            
        }
        g_mutex_unlock(&mutex_sound);
        
        da->soundRead.ready = 0;
        
        nanosleep(&req, NULL);
    }


    //Stop soundinit************************************************
    
    while(da->status.ref > 1){
        sleep(1);
        i++;
        if(i > 10){
            printf("Error in soundinit() ref > 1\n ");
            exit(1);
        }
    }
    da->status.ref--;
    if(ok)
        printf("Success\n");
    else
        printf("Failure\n"); 
err:
    
    ok = snd_pcm_close(Phandle);
    ok2 = snd_pcm_close(Chandle);
    if(err > 0){
        if(ok || ok2) statusprint("Sound Card Close Error!!", data);
        else statusprint("Sound Card Close!!", data);
    }
   
    
    printf("OutSound()\n");
    
    if(!da->status.ref){
        delVar(data);
    }else printf("Error soundinit() -> status.ref > 0");
    fflush(stdout);
    g_object_unref (stask);
    return ;
}
    


