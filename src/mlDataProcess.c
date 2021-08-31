#include "header.h"

static int getCross(double* sound, double* x, int n) {
    int i;
    int count = 0;

    for (i = 0; i < n - 1; i++) {
        if (*(sound + i) > 0.0 && *(sound + i + 1) < 0.0) {
            *(x + count) = (double)i;
            count++;
        }
    }
    return count;
}

void mlDataProcess(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable) {
    VApp* da = (VApp*)data;

    int i = 0;
    int selX = 0;
    int selectCount = 1;
    int tmpPos;
    int tmpi;
    int bufferCount = 0;
    struct timespec req = {0, 300000000}; // 0.3 second
    da->draw1[0].on = 1;
    da->draw2[0].on = 1;
    da->crossPoint.on = 1;

    double in1 = 0.0;
    double in2 = 0.0;
    double out1 = 0.0;
    double out2 = 0.0;

    da->status.ref++;

    while (da->status.open) {

        if (da->flag.nextWave) {
            da->flag.nextWave = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.row + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            bufferCount += da->settings.frames * da->settings.channels;
            if (bufferCount + da->settings.frames * da->settings.channels >= da->dataBuf.readSize / 2) bufferCount = 0;
            da->selPointS.x = 0;
            da->selPointE.x = 0;
            da->nextPoint.x = 0;
            da->nextPoint.y = 0;
        } else if (da->flag.prevWave) {
            da->flag.prevWave = 0;
            bufferCount -= da->settings.frames * da->settings.channels * 2;
            if (bufferCount < 0) bufferCount = 0;
            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.row + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
            }
            bufferCount += da->settings.frames * da->settings.channels;
            da->selPointS.x = 0;
            da->selPointE.x = 0;
            da->nextPoint.x = 0;
            da->nextPoint.y = 0;
        }

        // IIR Filter

        if (da->settings.filter == 0) {
            // Lo pass filter coefficient
            double omega = 2.0 * G_PI * da->scale.slider1 / (double)da->settings.rate;
            double alpha = sin(omega) / (2.0 * da->scale.slider2);

            double a0 = 1.0 + alpha;
            double a1 = -2.0 * cos(omega);
            double a2 = 1.0 - alpha;
            double b0 = (1.0 - cos(omega)) / 2.0;
            double b1 = 1.0 - cos(omega);
            double b2 = (1.0 - cos(omega)) / 2.0;

            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) = b0 / a0 * (*(da->dataBuf.row + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                                           a1 / a0 * out1 - a2 / a0 * out2;

                in2 = in1;                          // 2つ前の入力信号を更新
                in1 = *(da->dataBuf.row + i) * 2.0; // 1つ前の入力信号を更新

                out2 = out1;                     // 2つ前の出力信号を更新
                out1 = *(da->dataBuf.sound + i); // 1つ前の出力信号を更新
            }
        } else if (da->settings.filter == 1) {
            // Hi pass filter coefficient
            double omega = 2.0 * G_PI * da->scale.slider1 / (double)da->settings.rate;
            double alpha = sin(omega) / (2.0 * da->scale.slider2);

            double a0 = 1.0 + alpha;
            double a1 = -2.0 * cos(omega);
            double a2 = 1.0 - alpha;
            double b0 = (1.0 + cos(omega)) / 2.0;
            double b1 = -(1.0 + cos(omega));
            double b2 = (1.0 + cos(omega)) / 2.0;

            for (i = 0; i < da->settings.frames; i++) {
                *(da->dataBuf.sound + i) = b0 / a0 * (*(da->dataBuf.row + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                                           a1 / a0 * out1 - a2 / a0 * out2;

                in2 = in1;                          // 2つ前の入力信号を更新
                in1 = *(da->dataBuf.row + i) * 2.0; // 1つ前の入力信号を更新

                out2 = out1;                     // 2つ前の出力信号を更新
                out1 = *(da->dataBuf.sound + i); // 1つ前の出力信号を更新
            }

        } else { // No filter
            memcpy(da->dataBuf.sound, da->dataBuf.row, da->settings.frames * sizeof(double));
        }

        memcpy(da->draw1[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));
        memcpy(da->draw2[0].y, da->dataBuf.sound, da->settings.frames * sizeof(double));

        // ML data get *********

        da->crossPoint.Width = getCross(da->dataBuf.sound, da->crossPoint.x, da->settings.frames);
        if (da->crossPoint.Width <= 0) {
            printf("Error getCross\n");
            da->crossPoint.Width = 0;
        }

        if (da->flag.selPos) {
            da->flag.selPos = 0;
            if (da->crossPoint.Width >= 1) {
                selX = da->drawStartPos + da->clickPos.x * (da->drawEndPos - da->drawStartPos) / da->drawstatus1.Width;
                tmpi = 0;
                tmpPos = abs((int)*(da->crossPoint.x) - selX);
                for (i = 1; i < da->crossPoint.Width; i++) {
                    if (abs((int)*(da->crossPoint.x + i) - selX) < tmpPos) {
                        tmpPos = abs((int)*(da->crossPoint.x + i) - selX);
                        tmpi = i;
                    }
                }
                if (selectCount == 1) {
                    selectCount = 2;
                    da->selPointS.x = (int)*(da->crossPoint.x + tmpi);
                    da->selPointE.x = 0;
                    da->nextPoint.x = 0;
                    da->nextPoint.y = 0;
                } else if (selectCount == 2) {
                    selectCount = 1;
                    if (da->selPointS.x < (int)*(da->crossPoint.x + tmpi)) {
                        da->selPointE.x = (int)*(da->crossPoint.x + tmpi);
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2 < da->settings.frames) {
                            da->nextPoint.x = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2;
                        }
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3 < da->settings.frames) {
                            da->nextPoint.y = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3;
                        }
                    }
                }
            } else {
                da->selPointS.x = 0;
                da->selPointE.x = 0;
                printf("mlData No cross point\n");
            }
        }

        nanosleep(&req, NULL);

        da->flag.drawArea1 = 1; // write 0 in drawArea()
        g_idle_add(update_drawArea3, da);
        da->flag.drawArea2 = 1; // write 0 in drawArea()
        g_idle_add(update_drawArea4, da);
    }
    // Close mlDataProcess************************************************

    if (da->flag.drawArea1) sleep(1);
    if (da->flag.drawArea2) sleep(1);

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