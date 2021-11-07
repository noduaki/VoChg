#include "header.h"

int waveDiff(int startX, int endX, double* wave, double *diffWave) {

    int i;
    int n;
    int row = NUM_DATA_X;
    double ratio;
    double diff;
    if (endX - startX > row) {
        diff = (double)(endX - startX) / (double)(row);
        *(diffWave) = (*(wave + endX) - *(wave + startX));
        *(diffWave + (row - 1)) = (*(wave + endX + (endX - startX)) - *(wave + endX));

        for (i = 1; i < (row - 1); i++) {
            n = (int)((double)i * diff);
            ratio = (double)i * diff - (double)n;
            *(diffWave + i) = ((*(wave + endX + n) + (*(wave + endX + n + 1) - *(wave + endX + n)) * ratio) -
                              (*(wave + startX + n) + (*(wave + startX + n + 1) - *(wave + startX + n)) * ratio));
        }
        return 0;
    }else return -1;

}
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
    int k = 0;
    int selX = 0;
    int tmpPos;
    int tmpi;
    int change = 1;
    int selectCount = 1;
    int bufferCount = 0;
    int autoCount = 0;
    int ECount = 1;
    int diffCount = 0;
    int nextpoint = 0;
    int writeCount = 0;
    int dataCount = 0;
    int writeSize;
    int fileCount = 0;
    int startFrames = da->settings.frames / 2;

    int row = NUM_DATA_X;
    int column = NUM_MLDATA;
    double add = 0.0;
    double diff = 0.0;
    double ratio = 0.0;
    double tmpddd1;
    double tmpddd2;
    struct timespec req = {0, 200000000}; // 0.2 second
    da->draw1[0].on = 1;
    da->draw2[0].on = 1;
    da->draw2[1].on = 1;
    da->draw2[1].Width = row;
    da->crossPoint.on = 1;

    double in1 = 0.0;
    double in2 = 0.0;
    double out1 = 0.0;
    double out2 = 0.0;

    int err;
    GError* fileErr = NULL;
    GFileIOStream* outStream = NULL;
    GInputStream* inStrm;
    GOutputStream* outStrm;
    GIOStream* iostream;

    char csvDataX[NUM_MLDATA * NUM_DATA_X * 9] = {0};
    char csvDataT[NUM_MLDATA * NUM_DATA_X * 9] = {0};
    int csvCount = 0;

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

        if (da->status.filter == 0) {
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
        } else if (da->status.filter == 1) {
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

        memcpy(da->draw1[0].y, da->dataBuf.sound + startFrames, da->settings.frames * sizeof(double));
        memcpy(da->draw2[0].y, da->dataBuf.sound + startFrames, da->settings.frames * sizeof(double));

        // ML data get *********

        da->crossPoint.Width = getCross(da->dataBuf.sound + startFrames, da->crossPoint.pos, da->settings.frames);
        if (da->crossPoint.Width <= 0) {
            // printf("Error getCross\n");
            da->crossPoint.Width = 0;
        }

        // Select MLData point manually

        if (da->flag.selPos && !da->mlFlag.on) { //  selPos comes from drawArea click-relese-cb()
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
            waveDiff(da->selPointS.x, da->selPointE.x, da->dataBuf.sound + startFrames, da->draw2[1].y);
            if (da->mlFlag.enter0) {
                da->mlFlag.enter0 = 0;

                if (nextpoint) {
                    nextpoint = 0;


                    if (da->selPointE.x - da->selPointS.x > row) {
                        waveDiff(da->selPointS.x, da->selPointE.x, da->dataBuf.sound + startFrames, da->draw2[1].y);

                  
                        // Diff data write

                        memcpy(da->sData[writeCount].xData, da->draw2[1].y, row * sizeof(double));

                        da->sData[writeCount].yData = 0.0;

                        if (writeCount < NUM_MLDATA - 1) {
                            writeCount++;
                        } else {
                            printf("ML Data full\n");
                            strcat(da->statusBuf, "ML Data full");
                            g_idle_add(statusprint, data);
                        }
                        sprintf(da->statusBuf, "ML Data %d -> 0 # next %d", writeCount - 1, writeCount);
                        g_idle_add(statusprint, data);

                    } else {
                        strcat(da->statusBuf, "Error -> Select points length is under 128pt");
                        g_idle_add(statusprint, data);
                    }
                } else {
                    printf("Error -> No nextpoint so Can't calculate difference  \n");
                    strcat(da->statusBuf, "No nextpoint so Can't calculate difference");
                    g_idle_add(statusprint, data);
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

                    if (da->selPointE.x - da->selPointS.x > row) {
                        waveDiff(da->selPointS.x, da->selPointE.x, da->dataBuf.sound + startFrames, da->draw2[1].y);
                      
                        // Diff data write *****************

                        memcpy(da->sData[writeCount].xData, da->draw2[1].y, (row - 1) * sizeof(double));

                        da->sData[writeCount].yData = 1.0;

                        if (writeCount < NUM_MLDATA - 1) {
                            writeCount++;
                        } else {
                            printf("ML Data full\n");
                            strcat(da->statusBuf, "ML Data full");
                            g_idle_add(statusprint, data);
                        }
                        sprintf(da->statusBuf, "ML Data %d -> 1 # next %d ", writeCount - 1, writeCount);
                        g_idle_add(statusprint, data);
                    } else {
                        strcat(da->statusBuf, "Error -> Select points length is under 128pt");
                        g_idle_add(statusprint, data);
                    }

                } else {
                    strcpy(da->statusBuf, "No nextpoint so Can't calculate difference");
                    g_idle_add(statusprint, data);
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
                if (ECount > 1) {
                    ECount--;
                    change = 1;
                }
            } else if (da->mlFlag.dele1) {
                da->mlFlag.dele1 = 0;
                writeCount--;

            } else if (da->mlFlag.deleAll) {
                da->mlFlag.deleAll = 0;
                writeCount = 0;
            } else if (da->mlFlag.right1) {
                da->mlFlag.right1 = 0;
                writeCount++;
            } else if (da->mlFlag.left1) {
                da->mlFlag.left1 = 0;
                writeCount--;
            } else if (da->flag.writeFile) {
                if (writeCount == NUM_MLDATA - 1) {
                    int err;

                    // Write csv text
                    for (k = 0; k < NUM_MLDATA; k++) {
                        for (i = 0; i < NUM_DATA_X; i++) {
                            sprintf(csvDataX + (k * NUM_DATA_X * 9) + i * 9, "%.1f", da->sData[k].xData[i]);
                            *(csvDataX + (k * NUM_DATA_X * 9) + i * 9 + 8) = 0x2c;
                        }
                        sprintf(csvDataT + k * 4, "%f", da->sData[k].yData);
                        *(csvDataT + k * 4 + 3) = 0x0a;
                        *(csvDataX + (k * NUM_DATA_X * 9) + (NUM_DATA_X - 1) * 9 + 8) = 0x0a;
                    }

                    for (k = 0; k < NUM_DATA_X * NUM_MLDATA; k++) {
                        for (i = 0; i < 8; i++) {
                            if (*(csvDataX + k * 9 + i) < 0x2d || *(csvDataX + k * 9 + i) > 0x39) {
                                *(csvDataX + k * 9 + i) = 0x30;
                            }
                        }
                    }
                    // Select files to write data
                    if (da->settings.writefileX == NULL) {
                        g_idle_add(getWritefile, data);
                        i = 0;
                        while (da->settings.writefileT == NULL) {
                            sleep(1);
                            i++;
                            if (i > 100)
                                break;
                            else if (da->flag.writeFile == 0)
                                break;
                        }
                    }

                    if (da->settings.writefileX) {
                        strcat(da->statusBuf, "Data write to ");
                        strcat(da->statusBuf, da->settings.writefileXname);
                        g_idle_add(statusprint, data);

                        // Write X Data
                        outStream = g_file_open_readwrite(da->settings.writefileX, NULL, &fileErr);
                        if (fileErr != NULL) {
                            sprintf(da->statusBuf, "Can't open %s for writing X: %s \n", gSet.readfilename,
                                    fileErr->message);
                            g_idle_add(statusprint, data);
                        }
                        iostream = G_IO_STREAM(outStream);
                        inStrm = g_io_stream_get_input_stream(iostream);
                        outStrm = g_io_stream_get_output_stream(iostream);

                        err = g_seekable_tell(G_SEEKABLE(iostream));
                        printf("seek offset X %d\n", err);
                        err = g_seekable_seek(G_SEEKABLE(iostream), fileCount, G_SEEK_SET, NULL, &fileErr);
                        if (fileErr != NULL) {
                            printf("Error in mlData -> X seek\n");
                        }

                        writeSize = g_output_stream_write(G_OUTPUT_STREAM(outStrm), csvDataX,
                                                          NUM_DATA_X * NUM_MLDATA * 9, NULL, &fileErr);

                        if (fileErr != NULL) {
                            printf("Error in mlDataX -> %s ->> writesize %d\n", fileErr->message, writeSize);
                        } else if (writeSize != NUM_DATA_X * NUM_MLDATA * 9) {
                            printf("Error in mlDataX -> writesize %d buffsize %d\n", writeSize,
                                   NUM_DATA_X * NUM_MLDATA * 9);
                        }
                        fileCount += writeSize;
                        err = g_seekable_tell(G_SEEKABLE(iostream));
                        printf("seek offset X %d\n", err);

                        g_io_stream_close(iostream, NULL, &fileErr);
                        if (fileErr != NULL) {
                            printf("Error in mlDataX ->%s \n", fileErr->message);
                        } else if (err == FALSE) {
                            printf("Error in mlDataX -> outputstream close\n");
                        }
                        g_object_unref(outStream);

                        // Write T Data
                        fileCount = 0;
                        outStream = g_file_open_readwrite(da->settings.writefileT, NULL, &fileErr);
                        if (fileErr != NULL) {
                            sprintf(da->statusBuf, "Can't open %s for writingT: %s \n", gSet.readfilename,
                                    fileErr->message);
                            g_idle_add(statusprint, data);
                        }
                        iostream = G_IO_STREAM(outStream);
                        inStrm = g_io_stream_get_input_stream(iostream);
                        outStrm = g_io_stream_get_output_stream(iostream);

                        err = g_seekable_tell(G_SEEKABLE(iostream));
                        printf("seek offset T %d\n", err);
                        err = g_seekable_seek(G_SEEKABLE(iostream), fileCount, G_SEEK_SET, NULL, &fileErr);
                        if (fileErr != NULL) {
                            printf("Error in mlDataT-> seek\n");
                        }

                        writeSize =
                            g_output_stream_write(G_OUTPUT_STREAM(outStrm), csvDataT, NUM_MLDATA * 4, NULL, &fileErr);
                        if (fileErr != NULL) {
                            printf("Error in mlDataT -> %s ->> writesize %d\n", fileErr->message, writeSize);
                        } else if (writeSize != NUM_MLDATA * 4) {
                            printf("Error in mlDataT -> writesize %d buffsize %d\n", writeSize, NUM_MLDATA * 4);
                        }
                        fileCount += writeSize;
                        err = g_seekable_tell(G_SEEKABLE(iostream));
                        printf("seek offset T %d\n", err);

                        g_io_stream_close(iostream, NULL, &fileErr);
                        if (fileErr != NULL) {
                            printf("Error in mlDataT ->%s \n", fileErr->message);
                        } else if (err == FALSE) {
                            printf("Error in mlDataT -> outputstream close\n");
                        }
                        g_object_unref(outStream);
                    } else {
                        strcat(da->statusBuf, "File dose not exist");
                        g_idle_add(statusprint, data);
                    }

                } else {
                    strcat(da->statusBuf, "Write Data is not filled");
                    g_idle_add(statusprint, data);
                }
                da->flag.writeFile = 0;
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