#include "header.h"

void mlDataProcess(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable) {
    VApp* da = (VApp*)data;

    int i = 0;
    int bufferCount = 0;
    struct timespec req = {0, 10000000};
    da->draw1[0].on = 1;
    da->status.ref++;

    while (da->status.open) {
        if (da->flag.nextWave) {
            da->flag.nextWave = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) =
                    (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            memcpy(da->draw1[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));
            bufferCount += da->settings.frames * da->settings.channels;
            if (bufferCount + da->settings.frames * da->settings.channels >= da->dataBuf.readSize / 2) bufferCount = 0;
        }else if(da->flag.prevWave){
            da->flag.prevWave = 0;
            bufferCount -= da->settings.frames * da->settings.channels * 2;
            if(bufferCount < 0)bufferCount = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) =
                    (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            memcpy(da->draw1[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));
            bufferCount += da->settings.frames * da->settings.channels;
        }else{
            nanosleep(&req, NULL);
        }
        g_idle_add(update_drawArea1, da);
    }
    da->status.ref--;
    if (!da->status.ref) {
        delVar(data);
    } else
        printf("Error mlDataProcess() -> status.ref > 0 ref= %d\n", da->status.ref);

    g_object_unref(stask);
    return;
}