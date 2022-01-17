#include "header.h"

// Get ML Weights from csv file
void getWeight(GtkWidget* window, gpointer data) {

    VApp* da = (VApp*)data;

    GFile* W_file = NULL;
    char name[256];
    char* read = NULL;

    int i;
    int m;
    int k;
    int weightSize;
    int tmp = 0;
    int num[20] = {0};
    int charSize[20] = {0};

    char* string = NULL;
    char* stopstring = NULL;
    char* strtmp = NULL;
    errno = 0;
    double x;

    GError* fileErr = NULL;
    GFileInputStream* inStream = NULL;
    GFileInfo* info = NULL;
    int total_size = -1;
    gboolean err = 0;
    size_t length = 0;

    err = getFilePath(W_file, name);
    if (err) {
        printf("Error in Weight func.\n");
    }
    W_file = g_file_new_for_path(name);

    printf("%s\n", name);

    if (W_file != NULL) {
        inStream = g_file_read(W_file, NULL, &fileErr);
        if (fileErr != NULL) {
            printf("Could not open %s for reading: %s \n", name, fileErr->message);
        }
        info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(inStream), G_FILE_ATTRIBUTE_STANDARD_SIZE, NULL,
                                              &fileErr);
        if (info) {
            if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                total_size = g_file_info_get_size(info);
            printf("total_size = %d\n", total_size);

        } else {
            printf("Error in initVar -> Can't get info\n");
        }

        if (total_size > 0) {

            if (read == NULL) {
                read = (char*)calloc(total_size, 1);
                if ((length = g_input_stream_read(G_INPUT_STREAM(inStream), read, total_size, NULL, &fileErr)) != -1) {

                    printf("reading file length = %ld\n", length);
                } else {

                    printf("Error in initVar -> file read\n");
                }
            }
        }
        err = g_input_stream_close(G_INPUT_STREAM(inStream), NULL, &fileErr);
        if (err == FALSE) printf("Error in initVar -> stream_close failed\n");
        g_object_unref(inStream);
        g_object_unref(info);
        g_clear_object(&W_file);
    } else {
        return;
    }

    // k is num of w and b It's k-layers nn network
    // num[k] is size of each w or b

    k = 0;
    weightSize = 0;
    tmp = 0;
    for (i = 0; i < total_size; i++) {
        if (*(read + i) == 0x2c || *(read + i) == 0x0d) weightSize++; // 0x2c = ',' 0x0d = CR
        if (*(read + i) == 0x0a) {                                    // 0x0a = LF
            num[k] = weightSize;
            charSize[k] = i;
            k++;
            if (k > NUM_LAYER * 2) {
                printf("Error in getWeight -> too much Weights-%d\n", k);
                strcat(da->statusBuf, " Error-> too much Weights");
                statusprint(data);
                return;
            }
            weightSize = 0;
        }
        if (*(read + i) == 0x0) break;
    }
    // Error check ****
    if (charSize[k - 1] != total_size - 1) {
        printf("Error in getWeight -> readsize\n");
        strcat(da->statusBuf, " Error-> weght file reading");
        statusprint(data);
        return;
    }
    if (k % 2) {
        printf("Error in getWeight -> k is odd\n");
        strcat(da->statusBuf, " Error-> Not equal Weght and bias number");
        statusprint(data);
        return;
    }

    for (i = 0; i < k / 2; i++) {
        da->W[i].weight = (double*)malloc(num[i] * sizeof(double));
        da->W[i].bias = (double*)malloc(num[i + (k / 2)] * sizeof(double));
        printf("k = %i, size = %d,,,k = %i, size = %d\n", i, num[i], i + (k / 2), num[i + (k / 2)]);
    }
    strtmp = read;
    for (i = 0; i < k / 2; i++) {
        *(da->W[i].weight) = strtod(strtmp, &stopstring);
        for (m = 1; m < num[i]; m++) {
            string = stopstring + 1;
            *(da->W[i].weight + m) = strtod(string, &stopstring);
            if (errno == ERANGE) {
                printf("Error in getWeightFunc ->%d,,%d\n", errno, ERANGE);
                return;
            }
            if (string == stopstring) {
                printf("Error in getWeightFunc -> Can't Convert\n");
                return;
            }
        }
        strtmp = read + charSize[i] + 1;
    }

    for (i = 0; i < k / 2; i++) {
        *(da->W[i].bias) = strtod(strtmp, &stopstring);
        for (m = 1; m < num[i + (k / 2)]; m++) {
            string = stopstring + 1;
            *(da->W[i].bias + m) = strtod(string, &stopstring);
            if (errno == ERANGE) {
                printf("Error in getWeightFunc ->%d,,%d\n", errno, ERANGE);
                return;
            }
            if (string == stopstring) {
                printf("Error in getWeightFunc -> Can't Convert\n");
                return;
            }
        }
        strtmp = read + charSize[i + (k / 2)] + 1;
    }
    // printf("BIASSSS->%f,,,%f\n", *(da->W[2].bias), *(da->W[2].bias + 1));

    free(read);
}

void mlTest(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable) {
    VApp* da = (VApp*)data;
    int i;
    int k;
    int n;
    int m;

    int o = 0;
    int p = 0;

    int posCount1 = 0;
    int posCount2 = 0;
    int SCount = 0;
    int ECount = 0;
    int err;
    int judge = 0;
    int divi = 0;

    struct timespec req = {0, 200000000}; // 0.2 second

    int bufferCount = 0;
    int startFrames = da->settings.frames / 2;

    // DrawArea -> Select line for drawing
    da->draw1[0].on = 1;
    da->draw2[0].on = 1;
    da->crossPoint.on = 1;
    da->selCross.on = 1;

    double max;

    double in1 = 0.0;
    double in2 = 0.0;
    double out1 = 0.0;
    double out2 = 0.0;

    double* diffWave = (double*)malloc(NUM_DATA_X * sizeof(double));
    double* diffTmp1 = (double*)malloc(NUM_DATA_X * NEURONS_1 * sizeof(double));
    double* diffTmp2 = (double*)malloc(NUM_DATA_X * NEURONS_1 * sizeof(double));

    while (da->status.open) {

        if (da->flag.nextWave || da->flag.prevWave) {

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

            } else if (da->flag.prevWave) {
                da->flag.prevWave = 0;

                memmove(da->dataBuf.row + da->settings.frames, da->dataBuf.row, da->settings.frames * sizeof(double));
                bufferCount -= da->settings.frames * da->settings.channels * 3;
                if (bufferCount < 0) bufferCount = 0;
                for (i = 0; i < da->settings.frames; i++) {
                    *(da->dataBuf.row + i) = (double)(*(da->dataBuf.read + i * da->settings.channels + bufferCount));
                }
                bufferCount += da->settings.frames * da->settings.channels * 2;
            }
            da->selPointS.x = 0;
            da->selPointE.x = 0;
            da->nextPoint.x = 0;
            da->nextPoint.y = 0;
            posCount1 = 0;
            posCount2 = 0;
            k = 0;
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

        // Data is divided into each cycle
        // *******************************************************************************

        da->crossPoint.Width =
            getCross(da->dataBuf.sound + startFrames, da->crossPoint.pos, da->crossPoint.y, da->settings.frames);
        if (da->crossPoint.Width <= 0) {
            // printf("Error getCross\n");
            da->crossPoint.Width = 0;
        }
        if (da->crossPoint.Width > 5 && posCount1 == 0) {
            struct timespec ts, te;
            struct timespec start_time, end_time;
            clock_t start_clock, end_clock;
           
            timespec_get(&ts, TIME_UTC);
            start_clock = clock();
            while (k < da->crossPoint.Width - 1) {

                for (i = 0; (i + k) < da->crossPoint.Width; i++) {
                    if (*(da->crossPoint.pos + k + i) - *(da->crossPoint.pos + k) > 500) {
                        break;
                    }
                }
                if (i + k < da->crossPoint.Width) {
                    for (n = i; n >= 1; n--) {
                        if (*(da->crossPoint.pos + k + n) - *(da->crossPoint.pos + k) < NUM_DATA_X) {
                            k++;
                            break;
                        }
                        err = waveDiff(*(da->crossPoint.pos + k), *(da->crossPoint.pos + k + n),
                                       da->dataBuf.sound + startFrames, diffWave);
                        if (err < 0) printf("Error in diff calcuration -> diff\n");
                        judge = judgeWave(da->W, diffWave, diffTmp1, diffTmp2);
                        if (judge == 1) {
                            divi = (*(da->crossPoint.pos + k + n) - *(da->crossPoint.pos + k)) / 2;
                            m = n / 2;
                            o = (int)((double)(*(da->crossPoint.pos + k + m) - *(da->crossPoint.pos + k)) * 0.9);
                            p = (int)((double)(*(da->crossPoint.pos + k + m) - *(da->crossPoint.pos + k)) * 1.1);
                            if ((m == 1 || m == 2) && divi > NUM_DATA_X &&
                                (divi >
                                     (int)((double)(*(da->crossPoint.pos + k + m) - *(da->crossPoint.pos + k)) * 0.9) &&
                                 divi < (int)((double)(*(da->crossPoint.pos + k + m) - *(da->crossPoint.pos + k)) *
                                              1.1))) {
                                err = waveDiff(*(da->crossPoint.pos + k), *(da->crossPoint.pos + k + m),
                                               da->dataBuf.sound + startFrames, diffWave);
                                if (err < 0) printf("Error in diff calcuration -> diff\n");
                                judge = judgeWave(da->W, diffWave, diffTmp1, diffTmp2);
                                if (judge == 1) {
                                    *(da->draw1[1].pos + posCount2) = *(da->crossPoint.pos + k + m);
                                    posCount2++;
                                }
                            }
                            *(da->selCross.pos + posCount1) = *(da->crossPoint.pos + k);
                            posCount1++;
                            *(da->selCross.pos + posCount1) = *(da->crossPoint.pos + k + n);
                            posCount1++;
                            k += n;
                            break;

                        } else if (judge < 0) {
                            printf("Error in diff calcuration -> judge = %d\n", judge);
                            k++;
                            break;
                        }
                    }
                    if (n == 0) k++;
                } else
                    k++;
            }
             timespec_get(&te, TIME_UTC);
            end_clock = clock();

            /* 計測時間の表示 */
            printf("time:%ld\n", te.tv_nsec - ts.tv_nsec);

            printf("clock:%f\n", (double)(end_clock - start_clock) / CLOCKS_PER_SEC);
        }
        da->selCross.Width = posCount1;
        da->draw1[1].Width = posCount2;
        //****************************************************************************************************************
        da->flag.drawArea1 = 1; // write to 0 in drawArea()
        g_idle_add(update_drawArea3, da);
        da->flag.drawArea2 = 1; // write to 0 in drawArea()
        g_idle_add(update_drawArea4, da);

        nanosleep(&req, NULL);
    }

    // Closing Thread ********************
    free(diffWave);
    diffWave = NULL;
    free(diffTmp1);
    diffTmp1 = NULL;
    free(diffTmp2);
    diffTmp2 = NULL;

    if (da->flag.drawArea2) sleep(1); // waiting DrawArea is done

    i = 0;
    while (da->status.ref > 1) {
        sleep(1);
        i++;
        if (i > 10) {
            printf("Error in mlTest() ref > 1 ref=%d\n ", da->status.ref);
            exit(1);
        }
    }
    da->status.ref--;
    if (!da->status.ref) {
        delVar(data);
    } else
        printf("Error in mlTest() -> status.ref > 0 ref= %d\n", da->status.ref);
    printf("OutMLTEST\n");

    g_object_unref(stask);
    return;
}