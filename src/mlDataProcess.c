#include "header.h"

static int getCross(double* sound, int* pos, int n) {
    int i;
    int count = 0;

    for (i = 0; i < n - 1; i++) {
        if (*(sound + i) > 0.0 && *(sound + i + 1) < 0.0) {
            *(pos + count) = i;
            count++;
        }
    }
    return count;
}

void mlDataProcess(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable) {
    VApp* da = (VApp*)data;

    int i = 0;
    int n = 0;
    int selX = 0;
    
    int tmpPos;
    int tmpi;
    int change      = 1;
    int selectCount = 1;
    int bufferCount = 0;
    int autoCount   = 0;
    int ECount      = 1;
    int diffCount   = 0;
    int nextpoint   = 0;
    double diff  = 0.0;
    double ratio = 0.0;
    double tmpddd1;
    double tmpddd2;
    struct timespec req = {0, 200000000}; // 0.2 second
    da->draw1[0].on = 1;
    da->draw2[0].on = 1;
    da->draw2[1].on = 1;
    da->draw2[1].Width = 128;
    da->crossPoint.on = 1;

    double in1 = 0.0;
    double in2 = 0.0;
    double out1 = 0.0;
    double out2 = 0.0;

    da->status.ref++;

    while (da->status.open) {
        if (da->flag.nextWave || da->flag.prevWave) {
            autoCount = 0;
            if (da->flag.nextWave) {
                da->flag.nextWave = 0;

                memmove(da->dataBuf.row, da->dataBuf.row + da->settings.frames, da->settings.frames * sizeof(double));
                for (i = 0; i < da->settings.frames; i++) {
                    *(da->dataBuf.row + da->settings.frames + i) =
                        (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
                }
                bufferCount += da->settings.frames * da->settings.channels;
                if (bufferCount + da->settings.frames * da->settings.channels >= da->dataBuf.readSize / 2)
                    bufferCount = 0;
                da->selPointS.x = 0;
                da->selPointE.x = 0;
                da->nextPoint.x = 0;
                da->nextPoint.y = 0;
            } else if (da->flag.prevWave) {
                da->flag.prevWave = 0;

                memmove(da->dataBuf.row + da->settings.frames, da->dataBuf.row, da->settings.frames * sizeof(double));
                bufferCount -= da->settings.frames * da->settings.channels * 3;
                if (bufferCount < 0) bufferCount = 0;
                for (i = 0; i < da->settings.frames; i++) {
                    *(da->dataBuf.row + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
                }
                bufferCount += da->settings.frames * da->settings.channels * 2;
                da->selPointS.x = 0;
                da->selPointE.x = 0;
                da->nextPoint.x = 0;
                da->nextPoint.y = 0;
            }
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

            for (i = 0; i < da->settings.frames * 2; i++) {
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

            for (i = 0; i < da->settings.frames * 2; i++) {
                *(da->dataBuf.sound + i) = b0 / a0 * (*(da->dataBuf.row + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                                           a1 / a0 * out1 - a2 / a0 * out2;

                in2 = in1;                          // 2つ前の入力信号を更新
                in1 = *(da->dataBuf.row + i) * 2.0; // 1つ前の入力信号を更新

                out2 = out1;                     // 2つ前の出力信号を更新
                out1 = *(da->dataBuf.sound + i); // 1つ前の出力信号を更新
            }

        } else { // No filter
            memcpy(da->dataBuf.sound, da->dataBuf.row, da->settings.frames * sizeof(double) * 2);
        }

        memcpy(da->draw1[0].y, da->dataBuf.sound + da->settings.frames / 2, da->settings.frames * sizeof(double));
        memcpy(da->draw2[0].y, da->dataBuf.sound + da->settings.frames / 2, da->settings.frames * sizeof(double));

        // ML data get *********

        da->crossPoint.Width =
            getCross(da->dataBuf.sound + da->settings.frames / 2, da->crossPoint.pos, da->settings.frames);
        if (da->crossPoint.Width <= 0) {
            // printf("Error getCross\n");
            da->crossPoint.Width = 0;
        }

        if (da->flag.selPos && !da->mlFlag.on) { // Click to select point
            da->flag.selPos = 0;

            nextpoint = 0;
            if (da->crossPoint.Width >= 1) {
                selX = da->drawStartPos + (int)((double)da->clickPos.x * (double)(da->drawEndPos - da->drawStartPos) /
                                                (double)da->drawstatus1.Width);
                tmpi = 0;
                tmpPos = abs(*(da->crossPoint.pos) - selX);
                for (i = 1; i < da->crossPoint.Width; i++) {
                    if (abs(*(da->crossPoint.pos + i) - selX) < tmpPos) {
                        tmpPos = abs(*(da->crossPoint.pos + i) - selX);
                        tmpi = i;
                    } else if (abs(*(da->crossPoint.pos + i) - selX) > tmpPos)
                        break;
                }

                if (selectCount == 1) { // Get start point
                    selectCount = 2;

                    da->selPointS.x = *(da->crossPoint.pos + tmpi);
                    da->selPointE.x = 0;
                    da->nextPoint.x = 0;
                    da->nextPoint.y = 0;

                } else if (selectCount == 2) { // Get end and next point

                    if (da->selPointS.x < *(da->crossPoint.pos + tmpi)) {
                        selectCount = 1;
                        da->selPointE.x = *(da->crossPoint.pos + tmpi);
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2 < da->settings.frames) {
                            da->nextPoint.x = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2;
                            nextpoint = 1;
                        } else {
                            da->nextPoint.x = 0;
                            nextpoint = 0;
                        }
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3 < da->settings.frames) {
                            da->nextPoint.y = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3;
                        } else {
                            da->nextPoint.y = 0;
                        }
                    } else if (da->selPointS.x > *(da->crossPoint.pos + tmpi)) {
                        selectCount = 1;
                        da->selPointE.x = da->selPointS.x;
                        da->selPointS.x = *(da->crossPoint.pos + tmpi);
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2 < da->settings.frames) {
                            da->nextPoint.x = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2;
                            nextpoint = 1;
                        } else {
                            da->nextPoint.x = 0;
                            nextpoint = 0;
                        }
                        if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3 < da->settings.frames) {
                            da->nextPoint.y = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3;
                        } else {
                            da->nextPoint.y = 0;
                        }

                    } else {
                        strcpy(da->statusBuf, "Please select another point");
                        g_idle_add(statusprint, data);
                    }
                }
            } else {
                da->selPointS.x = 0;
                da->selPointE.x = 0;
                da->nextPoint.x = 0;
                da->nextPoint.y = 0;
                printf("mlData No cross point\n");
            }
        }

        // Auto move to select point **************************************************
        if (da->mlFlag.on) {
            if (da->mlFlag.enter0) {
                da->mlFlag.enter0 = 0;

                if (nextpoint) {
                    nextpoint = 0;
                    diffCount = (da->selPointE.x - da->selPointS.x) / 128;
                    if (diffCount) {
                        diff = (double)(da->selPointE.x - da->selPointS.x) / 128.0;
                        *(da->draw2[1].y) = *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x);
                        *(da->draw2[1].y + 127) = *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + 1);

                        for (i = 1; i < 127; i++) {
                            n = (int)((double)i * diff);
                            ratio = (double)i * diff - (double)n;
                            *(da->draw2[1].y + i) =
                                (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n) +
                                 (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n + 1) -
                                  *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n)) *
                                     ratio) -
                                (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n) +
                                 (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n + 1) -
                                  *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n)) *
                                     ratio);
                        }
                    }
                }

                if (autoCount + ECount * 2 < da->crossPoint.Width) {
                     da->selPointS.y = da->selPointS.x;
                    autoCount += ECount;
                    change = 1;
                }
            } else if (da->mlFlag.enter1) {
                da->mlFlag.enter1 = 0;
                if (nextpoint) {
                    nextpoint = 0;
                    diffCount = (da->selPointE.x - da->selPointS.x) / 128;
                    if (diffCount) {
                        diff = (double)(da->selPointE.x - da->selPointS.x) / 128.0;
                        *(da->draw2[1].y) = *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x) -
                                            *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x);
                        *(da->draw2[1].y + 127) = *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x +
                                                    (da->selPointE.x - da->selPointS.x)) -
                                                  *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x);

                        for (i = 1; i < 127; i++) {
                            n = (int)((double)i * diff);
                            ratio = (double)i * diff - (double)n;
                            *(da->draw2[1].y + i) =
                                (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n) +
                                 (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n + 1) -
                                  *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointE.x + n)) *
                                     ratio) -
                                (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n) +
                                 (*(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n + 1) -
                                  *(da->dataBuf.sound + da->settings.frames / 2 + da->selPointS.x + n)) *
                                     ratio);
                        }
                    }
                }
                if (autoCount + ECount * 2 < da->crossPoint.Width) {
                     da->selPointS.y = da->selPointS.x;
                    autoCount += ECount;
                    change = 1;
                }
            } else if (da->mlFlag.SRight) {
                da->mlFlag.SRight = 0;
                if (autoCount + ECount < da->crossPoint.Width - 1) {
                    autoCount++;
                    change = 1;
                }

            } else if (da->mlFlag.SLeft) {
                da->mlFlag.SLeft = 0;
                if (autoCount > 0) {
                    autoCount--;
                    change = 1;
                }

            } else if (da->mlFlag.ERight) {
                da->mlFlag.ERight = 0;
                if (autoCount + ECount < da->crossPoint.Width - 1) {
                    ECount++;
                    change = 1;
                }
            } else if (da->mlFlag.ELeft) {
                da->mlFlag.ELeft = 0;
                if (ECount > 1){
                     ECount--;
                     change = 1;
                }
            }

            if (change) {
                change = 0;
                if (autoCount >= da->crossPoint.Width - 1) autoCount = 0;
               
                da->selPointS.x = *(da->crossPoint.pos + autoCount);
                da->selPointE.x = *(da->crossPoint.pos + autoCount + ECount);
                if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2 < da->settings.frames) {
                    da->nextPoint.x = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 2;
                    nextpoint = 1;
                } else {
                    da->nextPoint.x = 0;
                    nextpoint = 0;
                }
                if (da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3 < da->settings.frames) {
                    da->nextPoint.y = da->selPointS.x + (da->selPointE.x - da->selPointS.x) * 3;
                } else {
                    da->nextPoint.y = 0;
                }
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