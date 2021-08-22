#include "header.h"

static snd_pcm_t *Phandle = NULL;
static snd_pcm_t *Chandle = NULL; //for Connect stream 
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
        printf("PeriodeSizeFaile %d >> %s\n", err, snd_strerror(err));
        exit(1);
    }
    da->soundRead.periodsize = periodsize;
    da->soundWrite.periodsize = periodsize;
    if(!i){
        strcat(da->statusBuf, " buffer--");
        sprintf(srate, "%ld", buffersize);
        strcat(da->statusBuf,srate);
        
        strcat(da->statusBuf, " period--");
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
    snd_pcm_sw_params_set_start_threshold(handle, swparams, 4096 - 512);
    err = snd_pcm_sw_params_set_avail_min(handle, swparams, 2048);
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


    if ((err = snd_pcm_prepare(chandle)) < 0) {
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
   // if(handle == Phandle) printf("Playback handle stream recovery\n");
    //else printf("Captuer handle stream recovery\n");   
    printf("%s>>>>\n", snd_strerror(err));
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
    

    return err;
}


void async_read_callback(snd_async_handler_t *ahandler)
{ 
    snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
    mAsyncData* asyData = snd_async_handler_get_callback_private(ahandler);
   
   
    int length = asyData->periodsize * 2;
    int pos = 0;
    int readLength = 0;
    int shortBufferSize;
    shortBufferSize = asyData->bufferSize * 2;
    
    snd_pcm_sframes_t avail;
    int err;
    g_mutex_lock(&mutex_sound);
    avail = snd_pcm_avail_update(handle);
    while (avail >= asyData->periodsize){
        if(pos < shortBufferSize - length && pos >= 0){ 
            
            err = snd_pcm_readi(handle, asyData->samples + pos, asyData->periodsize);
            if(err != asyData->periodsize) printf("Read number is under period ->>%d", err);
            pos += length; 
            readLength += err;        
            
        }else if(pos >= shortBufferSize - length && pos < shortBufferSize){
            
            err = snd_pcm_readi(handle, asyData->samples, asyData->periodsize); 
            if(err != asyData->periodsize) printf("Read number is under period ->>%d", err);
            pos += length;
            readLength += err; 
            
        }else{
            err = snd_pcm_readi(handle, asyData->samples, asyData->periodsize); 
        
            printf("async-READ BUFFER ERROR %d\n", asyData->pos);
            pos = 0;
            readLength += err; 
        } 
        
        
        avail = snd_pcm_avail_update(handle);
        if (err < 0 || avail == -EPIPE) {
            if ((err = xrun_recovery(handle, err)) < 0) {
                printf("async-read Recovery error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
    } 
    asyData->avail = readLength;
    g_mutex_unlock(&mutex_sound);
    asyData->ready = 1;
      
}

void async_write_callback(snd_async_handler_t *ahandler){
    snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
    mAsyncData* data = snd_async_handler_get_callback_private(ahandler);
    snd_pcm_sframes_t avail;
    
    int err;
    int length = data->periodsize * 2;
    int pos = data->pos;
    int16_t* ptr = data->samples + pos; 
    int writeAvail = 0;
    int shortBufferSize;
    shortBufferSize = data->bufferSize * 2;
    
    avail = snd_pcm_avail_update(handle);
    g_mutex_lock(&mutex_sound);    
    while (avail >= data->periodsize) {
            
        err = snd_pcm_writei(handle, ptr, data->periodsize);
        if (err != data->periodsize) {
            printf("async-Write error: written %i expected %d\n", err, data->periodsize);
            exit(EXIT_FAILURE);
        }
        if (err < 0) {
            printf("async-Write error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }
        ptr += length;
        pos += length;
        writeAvail += err;
        if(pos >= shortBufferSize){
            pos = 0;
            ptr -= shortBufferSize;
        }
        
        avail = snd_pcm_avail_update(handle);
            
        if (err < 0 || avail == -EPIPE) {
            if ((err = xrun_recovery(handle, err)) < 0) {
                printf("async-write Recovery error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
    }
    data->pos = pos;
    data->avail = writeAvail;
    
    if(data->avail > data->bufferSize) data->avail = data->bufferSize;
    g_mutex_unlock(&mutex_sound);
    data->ready = 1;
    
}


 
/*
 *   Transfer method - write and wait for room in buffer using poll
 */
 
static int wait_for_poll(snd_pcm_t *handle, struct pollfd *ufds, unsigned int count)
{
    unsigned short revents;
 
    while (1) {
        poll(ufds, count, -1);
        snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
        if (revents & POLLERR)
            return -EIO;
        if (revents & POLLIN)
            return 0;
    }
}
 
int mic_poll_loop(snd_pcm_t *chandle, snd_pcm_t *phandle, 
                                        VApp* da, int* open){
    struct pollfd *ufds;
    double phase = 0;
    int16_t *cptr;
    int16_t *pptr;
    int cerr, perr, count, cperiod, init; 
    int readCount = 0;
    int writeCount = 0;

    count = snd_pcm_poll_descriptors_count (chandle);
    if (count <= 0) {
        printf("Invalid poll descriptors count\n");
        return count;
    }
 
    ufds = malloc(sizeof(struct pollfd) * count);
    if (ufds == NULL) {
        printf("No enough memory\n");
        return -ENOMEM;
    }
    if ((cerr = snd_pcm_poll_descriptors(chandle, ufds, count)) < 0) {
        printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(cerr));
        return cerr;
    }
 
    init = 1;
    while (*open) {
        if (!init) {
            cerr = wait_for_poll(chandle, ufds, count);
            if (cerr < 0) {
                if (snd_pcm_state(chandle) == SND_PCM_STATE_XRUN ||
                    snd_pcm_state(chandle) == SND_PCM_STATE_SUSPENDED) {
                    cerr = snd_pcm_state(chandle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                    printf("1>>>>>>>\n");
                    if (xrun_recovery(chandle, cerr) < 0) {
                        printf("Write error: %s\n", snd_strerror(cerr));
                        exit(EXIT_FAILURE);
                    }
                    init = 1;
                } else {
                    printf("Wait for poll failed\n");
                    return cerr;
                }
            }
        }

        cptr = da->soundRead.samples + readCount;
        pptr = da->soundWrite.samples + writeCount;
        cperiod = da->soundRead.periodsize;
        while (cperiod > 0 && *open) {
            cerr = snd_pcm_readi(chandle, cptr, cperiod);
            perr = snd_pcm_writei(phandle, pptr, cperiod);
            if (cerr < 0) {
                if (xrun_recovery(chandle, cerr) < 0) {
                    printf("Write error: %s\n", snd_strerror(cerr));
                    exit(EXIT_FAILURE);
                }
                init = 1;
                break;  /* skip one period */
            }
            if (snd_pcm_state(chandle) == SND_PCM_STATE_RUNNING) init = 0;

            readCount += cerr * da->settings.channels;
            writeCount += perr * da->settings.channels;
            if(readCount >= da->soundRead.bufferSize * da->settings.channels){
                readCount = 0;
                writeCount = 0;
                memcpy(da->soundWrite.samples, da->soundRead.samples, 
                                    da->soundRead.bufferSize * da->settings.channels * sizeof(uint16_t));
            }
            cperiod -= cerr;
            if (cperiod <= 0){               
                break;
            }else{
                cerr = snd_pcm_readi(chandle, cptr, cperiod);
                init = 0;
                break;
            }
        }
    }
}

int file_poll_loop(snd_pcm_t *chandle, snd_pcm_t *phandle, 
                                        VApp* da, int* open){
    struct pollfd *ufds;
    double phase = 0;
    int16_t *cptr;
    int16_t *pptr;
    int cerr, perr, count, cperiod, init; 
    int readCount = 0;
    int writeCount = 0;

    count = snd_pcm_poll_descriptors_count (chandle);
    if (count <= 0) {
        printf("Invalid poll descriptors count\n");
        return count;
    }
 
    ufds = malloc(sizeof(struct pollfd) * count);
    if (ufds == NULL) {
        printf("No enough memory\n");
        return -ENOMEM;
    }
    if ((cerr = snd_pcm_poll_descriptors(chandle, ufds, count)) < 0) {
        printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(cerr));
        return cerr;
    }
 
    init = 1;
    while (*open) {
        if (!init) {
            cerr = wait_for_poll(chandle, ufds, count);
            if (cerr < 0) {
                if (snd_pcm_state(chandle) == SND_PCM_STATE_XRUN ||
                    snd_pcm_state(chandle) == SND_PCM_STATE_SUSPENDED) {
                    cerr = snd_pcm_state(chandle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                    printf("1>>>>>>>\n");
                    if (xrun_recovery(chandle, cerr) < 0) {
                        printf("Write error: %s\n", snd_strerror(cerr));
                        exit(EXIT_FAILURE);
                    }
                    init = 1;
                } else {
                    printf("Wait for poll failed\n");
                    return cerr;
                }
            }
        }

        cptr = da->soundRead.samples + readCount;
        pptr = da->soundWrite.samples + writeCount;
        cperiod = da->soundRead.periodsize;
        while (cperiod > 0 && *open) {
            cerr = snd_pcm_readi(chandle, cptr, cperiod);
            perr = snd_pcm_writei(phandle, pptr, cperiod);
            if (cerr < 0) {
                if (xrun_recovery(chandle, cerr) < 0) {
                    printf("Write error: %s\n", snd_strerror(cerr));
                    exit(EXIT_FAILURE);
                }
                init = 1;
                break;  /* skip one period */
            }
            if (snd_pcm_state(chandle) == SND_PCM_STATE_RUNNING) init = 0;

            readCount += cerr * da->settings.channels;
            writeCount += perr * da->settings.channels;
            if(readCount >= da->soundRead.bufferSize * da->settings.channels){
                readCount = 0;
                writeCount = 0;
                memcpy(da->soundWrite.samples, da->soundRead.samples, 
                                    da->soundRead.bufferSize * da->settings.channels * sizeof(uint16_t));
            }
            cperiod -= cerr;
            if (cperiod <= 0){               
                break;
            }else{
                cerr = snd_pcm_readi(chandle, cptr, cperiod);
                init = 0;
                break;
            }
        }
    }
}

void asyncPoll(VApp* da){

    int i;
    static uint32_t posr = 0;
    static uint32_t posw = 0;
    static uint32_t postmp = 0; 
    static uint32_t count  = 0;
    static uint32_t dataCount = 0;
    uint32_t v;

    static int testCount = 0;
   
    double  tmpDuble;
    int     tmpData   = 0;
    uint32_t frames = da->settings.frames;
    uint32_t charBufferSize = da->settings.pcm_buffer_size * da->settings.channels * sizeof(short); 

    if(da->flag.soundFile == 1){
        if(da->soundWrite.ready){
             
            g_mutex_lock(&mutex_sound);
            posr = da->soundWrite.avail;
            if(posr + posw > frames){
                posw = 0;
                printf("Error in asyPoll -> over frames\n");
            }

            //write to pcm playback sample 
            
            for(i = 0; i < posr; i++){
                
                *(da->soundWrite.samples + (i * 2) + (posw * 2)) = (int16_t)(*(da->dataBuf.sound + i + posw)); 
                *(da->soundWrite.samples + (i * 2) + (posw * 2) + 1) = (int16_t)(*(da->dataBuf.sound + i + posw));            
               
                
            }
            
            g_mutex_unlock(&mutex_sound);
            if(count + (posr * 2) >= da->dataBuf.readSize / 2)
                count = 0;
           
            for(i = 0; i < posr; i++){
               
                *(da->dataBuf.row + i + posw) = (double)(*(da->dataBuf.read  + count + (i * 2)));
                
            }
            count += posr * 2;
            
            posw += posr;
           
           
            if(posw >= da->soundWrite.bufferSize){
                posw = 0;
                memcpy(da->dataBuf.sound, da->dataBuf.row, sizeof(double) * da->soundWrite.bufferSize);
                
            }
            da->soundWrite.ready = 0;
        } 
      
    }else if(da->flag.soundFile == 0){
        if(da->soundRead.ready){

            g_mutex_lock(&mutex_sound);
            posr = da->soundRead.avail;
            if(posr + posw > frames){
                posw = 0;
                printf("Error in asyPoll -> over frames\n");
            }

            //write to pcm playback sample 
            
            for(i = 0; i < posr; i++){
                
                *(da->soundWrite.samples + (i * 2) + (posw * 2)) = (int16_t)(*(da->dataBuf.sound + i + posw)); 
                *(da->soundWrite.samples + (i * 2) + (posw * 2) + 1) = (int16_t)(*(da->dataBuf.sound + i + posw));            
               
                
            }
            
           
            if(count + (posr * 2) >= da->dataBuf.readSize / 2)
                count = 0;
           
            for(i = 0; i < posr; i++){
               
                *(da->dataBuf.row + i + posw) = (double)(*(da->soundRead.samples + (i * 2)));
                
            }
            count += posr * 2;
            
            posw += posr;
           
            g_mutex_unlock(&mutex_sound);
            if(posw >= da->soundRead.bufferSize){
                posw = 0;
                memcpy(da->dataBuf.sound, da->dataBuf.row, sizeof(double) * da->soundRead.bufferSize);
                
            }
            da->soundRead.ready = 0;
        } 
    }
}


void initSound(GTask *stask, gpointer source_object, gpointer data, GCancellable *cancellable){

    VApp* da = (VApp*)data;
    
    snd_async_handler_t *ahandler = NULL;
    snd_async_handler_t *whandler = NULL; 
    int latency, morehelp;
    int ok;
    int ok2;
    int err;
    int pcmErr = 0;
    int i;
    int deviceflag = 0;

    if(*da->settings.deviceName == 'd' && *(da->settings.deviceName + 1) == 'e' &&
        *(da->settings.deviceName + 2) == 'f' && *(da->settings.deviceName + 3) == 'a' && 
        *(da->settings.deviceName + 4) == 'u' && *(da->settings.deviceName + 5) == 'l') deviceflag = 1;
   
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


    

    for(i = 0; i < da->settings.pcm_buffer_size; i += 2){
        tmp = (signed short)((sin(((6.283185 * 32.0) / da->settings.pcm_buffer_size) * (double)i)) * 10000.0);
        *(da->soundWrite.samples + i) = tmp;
        *(da->soundWrite.samples + i + 1) = tmp;
    }
    //pcm_list(SND_PCM_STREAM_CAPTURE, da->settings.deviceName);  
   
    if ((err = snd_pcm_open(&Phandle, da->settings.deviceName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        statusprint("Playback open error", data);
        pcmErr = 1;
        goto err;
    }
    
    if ((err = snd_pcm_open(&Chandle, da->settings.deviceName, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        printf("Record open error: %s\n", snd_strerror(err));
        statusprint("Record open error", data);
        pcmErr = 1;
        goto err;
    }
    
    //statusbar shown 
   
    strcat(da->statusBuf, da->settings.deviceName);
    strcpy(sformat, "  LE16bit  ");
    strcat(da->statusBuf, sformat);
    sprintf(srate, "%d", da->settings.channels);
    strcat(da->statusBuf,srate);
    strcat(da->statusBuf, "ch  ");   
    sprintf(srate, "%d", da->settings.rate);
    strcat(da->statusBuf,srate);
    strcat(da->statusBuf, "hz ");
   
    frames_in = frames_out = 0;
    
    if (setparams(Phandle, Chandle,  data) < 0){
        printf("Sound Card Settings Error\n");
    }

    if(!deviceflag){
        if(!(da->flag.soundFile) ){    
            if ((err = snd_pcm_link(Phandle, Chandle)) < 0) {
                printf("Streams link error: %s\n", snd_strerror(err));
                exit(0);
            }
        }
    }
    
    if (snd_pcm_format_set_silence(da->settings.format, da->soundRead.samples, 
                                da->settings.channels * da->settings.pcm_buffer_size) < 0) {
        fprintf(stderr, "silence error\n");
    } 
    gettimestamp(Phandle, &p_tstamp);
    gettimestamp(Chandle, &c_tstamp);
    ok = 1;
   
   
    
    if(!deviceflag){
        err = snd_async_add_pcm_handler(&whandler, Phandle, async_write_callback, &da->soundWrite);
        if (err < 0) {
            printf("Unable to register async handler>>write. %s\n", snd_strerror (err));
            statusprint("Unable to register async handler Write Sound Card Fail", data);
            goto err;
        }
        err = snd_async_add_pcm_handler(&ahandler, Chandle, async_read_callback, &da->soundRead);
        if (err < 0) {
            printf("Unable to register async handler>>read. %s\n", snd_strerror (err));
            statusprint("Unable to register async handler Read Sound Card fail", data);
            goto err;
        }
        
    }else if(deviceflag){
        printf("poll write\n");
    }
    //Write Buffer initial writeing twice
    err = snd_pcm_writei(Phandle, da->soundWrite.samples, da->settings.period_size);
        
    if (err < 0) {
        printf("Initial write error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    if (err != da->settings.period_size) {
        printf("Initial write error: written %i expected %ld\n", err, da->settings.period_size);
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_writei(Phandle, da->soundWrite.samples, da->settings.period_size);
        
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
            strcat(da->statusBuf, " open file ");
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
        if (snd_pcm_state(Phandle) == SND_PCM_STATE_PREPARED) {
            err = snd_pcm_start(Phandle);
            if (err < 0) {
                printf("Start error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
    }
    
    printf("Start \n");

    statusprint(da->statusBuf, data);
   

    //START SOUND CARD************************************************* 
    if(deviceflag){ 
        if(da->flag.soundMic)    
            err = mic_poll_loop(Chandle, Phandle, da, &da->status.open);
        else if(da->flag.soundFile)
            err = mic_poll_loop(Chandle, Phandle, da, &da->status.open);
        if (err < 0)
            printf("Error in soundinit() Transfer failed: %s\n", snd_strerror(err));
    }else{              
        while(da->status.open) {
            asyncPoll(da);            
            nanosleep(&req, NULL);
        }
    }
    //Stop soundinit************************************************
    
    while(da->status.ref > 1){
        sleep(1);
        i++;
        if(i > 10){
            printf("Error in soundinit() ref > 1 ref=%d\n " , da->status.ref);
            exit(1);
        }
    }
    
    if(ok)
        printf("Success\n");
    else
        printf("Failure\n"); 
err:
    da->status.ref--;
    if(!pcmErr){
        
        ok = snd_pcm_close(Phandle);
        ok2 = snd_pcm_close(Chandle);
        if(err > 0){
            if(ok || ok2) statusprint("Sound Card Close Error!!", data);
            else statusprint("Sound Card Close!!", data);
        }
    }
    Phandle = NULL;
    Chandle = NULL;
    
    printf("OutSound()\n");
    
    if(!da->status.ref){
        delVar(data);
    }else printf("Error soundinit() -> status.ref > 0 ref= %d\n", da->status.ref);
    fflush(stdout);
    g_object_unref (stask);
    return ;
}
    


