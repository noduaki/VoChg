#include "header.h"

void mlDataProcess(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable) {
    VApp* da = (VApp*)data;

    int i = 0;
    int bufferCount = 0;
    struct timespec req = {0, 100000000};
    da->draw1[0].on = 1;
    da->status.ref++;

    while (da->status.open) {
        if (da->flag.nextWave) {
            da->flag.nextWave = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            memcpy(da->draw1[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));
            bufferCount += da->settings.frames * da->settings.channels;
            if (bufferCount + da->settings.frames * da->settings.channels >= da->dataBuf.readSize / 2) bufferCount = 0;
        } else if (da->flag.prevWave) {
            da->flag.prevWave = 0;
            bufferCount -= da->settings.frames * da->settings.channels * 2;
            if (bufferCount < 0) bufferCount = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            memcpy(da->draw1[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));
            bufferCount += da->settings.frames * da->settings.channels;
        } else {
            
        }
        g_idle_add(update_drawArea1, da);
        nanosleep(&req, NULL);
    }
    // Close mlDataProcess************************************************
    i = 0;
    while (da->flag.drawArea) {
        sleep(1);
        i++;
        if (i > 10) {
            printf("Error in mlDataProcess() -> drawArea flag\n ");
            exit(1);
        }
    }
    i = 0;
    while (da->status.ref > 1) {
        sleep(1);
        i++;
        if (i > 10) {
            printf("Error in mlDataProcess() ref > 1 ref=%d\n ", da->status.ref);
            exit(1);
        }
    }
    da->status.ref--;
    if (!da->status.ref) {
        delVar(data);
    } else
        printf("Error in mlDataProcess() -> status.ref > 0 ref= %d\n", da->status.ref);
    printf("OutMLData\n");

    g_object_unref(stask);
    return;
}