#include "soundProcess.h"

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

int octaveHi(VApp* da, int overlength, int prepos) {
    int n;
    int i;
    int odd;
    int num;

    odd = (da->settings.frames + *(da->crossPoint.pos) - prepos) % 2;
    num = (da->settings.frames + *(da->crossPoint.pos) - prepos) / 2;
    for (i = 0; i < num; i++) {
        *(da->dataBuf.sound + overlength + i) = *(da->dataBuf.tmp + prepos + i * 2);
        *(da->dataBuf.sound + overlength + num + i) = *(da->dataBuf.tmp + prepos + i * 2);
    }
    overlength += i * 2;
    if (odd) {

        *(da->dataBuf.sound + overlength) = *(da->dataBuf.tmp + prepos + i * 2 + 1);
        overlength++;
    }

    for (n = 0; n < da->crossPoint.Width - 1; n++) {
        odd = (*(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n)) % 2;
        num = (*(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n)) / 2;
        for (i = 0; i < num; i++) {
            *(da->dataBuf.sound + overlength + i) =
                *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i * 2);
            *(da->dataBuf.sound + overlength + num + i) =
                *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i * 2);
        }
        overlength += i * 2;
        if (odd) {

            *(da->dataBuf.sound + overlength) =
                *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i * 2 + 1);
            overlength++;
        }
    }
    n = 0;
    while (overlength < da->settings.frames) {
        for (i = 0; i < *(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n); i++) {
            *(da->dataBuf.sound + overlength + i) =
                *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i);
        }
        overlength += i;
        n++;
        if (n > 10) {
            break;
        }
    }
    overlength -= da->settings.frames;
    if (overlength >= da->settings.frames) {
        printf("Error in soundprocess octaveHi-limit over\n");
        overlength = 0;
    } else if (overlength < 0) {
        printf("Error in soundprocess octaveHi-underrun\n");
        overlength = 0;
    }
    return overlength;
}
int octaveLo(VApp* da, int overlength, int prepos) {
    int n;
    int i;
    int odd;
    int num;

    odd = (da->settings.frames + *(da->crossPoint.pos) - prepos) % 2;
    num = da->settings.frames + *(da->crossPoint.pos) - prepos;
    if (num * 2 < da->settings.frames * 2 - overlength - 4) {
        for (i = 0; i < num; i++) {
            *(da->dataBuf.sound + overlength + i * 2) = *(da->dataBuf.tmp + prepos + i);
            *(da->dataBuf.sound + overlength + i * 2 + 1) = *(da->dataBuf.tmp + prepos + i);
        }
        overlength += i * 2;

        if (odd) {
            *(da->dataBuf.sound + overlength) = *(da->dataBuf.tmp + prepos + i + 1);
            overlength++;
        }
    }
    if (overlength < da->settings.frames) {
        for (n = 0; n < da->crossPoint.Width - 1; n++) {
            odd = (*(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n)) % 2;
            num = *(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n);
            if (overlength + num * 2 > da->settings.frames * 2) break;
            for (i = 0; i < num; i++) {
                *(da->dataBuf.sound + overlength + i * 2) =
                    *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i);
                *(da->dataBuf.sound + overlength + i * 2 + 1) =
                    *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i);
            }
            overlength += i * 2;
            if (odd) {
                *(da->dataBuf.sound + overlength) =
                    *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i + 1);
                overlength++;
            }
        }
    }
    n = 0;
    while (overlength < da->settings.frames) {
        for (i = 0; i < *(da->crossPoint.pos + n + 1) - *(da->crossPoint.pos + n); i++) {
            *(da->dataBuf.sound + overlength + i) =
                *(da->dataBuf.tmp + da->settings.frames + *(da->crossPoint.pos + n) + i);
        }
        overlength += i;
        n++;
        if (n > 10) {
            break;
        }
    }
    overlength -= da->settings.frames;
    if (overlength >= da->settings.frames) {
        printf("Error in soundprocess octaveLo-limit over\n");
        overlength = 0;
    } else if (overlength < 0) {
        printf("Error in soundprocess octaveLo-underrun\n");
        overlength = 0;
    }
    return overlength;
}

void soundProcessPoll(VApp* da) {
    int i;

    _Thread_local static int n = 0;
    _Thread_local static int bufferCount = 0;
    _Thread_local static double in1 = 0.0;
    _Thread_local static double in2 = 0.0;
    _Thread_local static double out1 = 0.0;
    _Thread_local static double out2 = 0.0;

    _Thread_local static int overlength = 0;
    _Thread_local static int prepos = 0;

    if (da->flag.soundFile) {
        // Data Buffer has two room. Second room is used new data
        memmove(da->dataBuf.row, da->dataBuf.row + da->settings.frames, da->settings.frames * sizeof(double));
        for (i = 0; i < da->settings.frames; i++) {
            *(da->dataBuf.row + da->settings.frames + i) =
                (double)((*(da->dataBuf.read + i * 2 + bufferCount + 1) + *(da->dataBuf.read + i * 2 + bufferCount)) /
                         2);

            da->fftForward.dataIn[i][0] = *(da->dataBuf.row + da->settings.frames + i) * *(da->fftWindow + i);
            da->fftForward.dataIn[i][1] = 0.0;
        }
        if (!da->flag.pause) bufferCount += da->settings.frames * da->settings.channels;
        if (bufferCount + da->settings.frames * da->settings.channels > da->dataBuf.readSize / 2) bufferCount = 0;
    } else if (da->flag.soundMic) {
        // Data Buffer has two room. Second room is used new data
        memmove(da->dataBuf.row, da->dataBuf.row + da->settings.frames, da->settings.frames * sizeof(double));

        for (i = 0; i < da->settings.frames; i++) {
            if (!da->flag.pause) {
                *(da->dataBuf.row + da->settings.frames + i) =
                    (double)((*(da->soundRead.samples + i * 2 + 1) + *(da->soundRead.samples + i * 2)) / 2);
            }
            da->fftForward.dataIn[i][0] = *(da->dataBuf.row + da->settings.frames + i) * *(da->fftWindow + i);
            da->fftForward.dataIn[i][1] = 0.0;
        }
    }

    // Row -> IIR filter -> Sound *********
    // Data Buffer has two room. Second room is used new data
    memmove(da->dataBuf.tmp, da->dataBuf.tmp + da->settings.frames, da->settings.frames * sizeof(double));
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

        for (i = 0; i < da->settings.frames; i++) {
            *(da->dataBuf.tmp + da->settings.frames + i) =
                b0 / a0 * (*(da->dataBuf.row + da->settings.frames + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->settings.frames + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                         // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.tmp + da->settings.frames + i); // 1つ前の出力信号を更新
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

        for (i = 0; i < da->settings.frames; i++) {
            *(da->dataBuf.tmp + da->settings.frames + i) =
                b0 / a0 * (*(da->dataBuf.row + da->settings.frames + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->settings.frames + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                         // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.tmp + da->settings.frames + i); // 1つ前の出力信号を更新
        }

    } else { // No filter
        memcpy(da->dataBuf.tmp + da->settings.frames, da->dataBuf.row + da->settings.frames,
               da->settings.frames * sizeof(double));
    }

    // ML data get *********
    if (da->crossPoint.Width >= 1) {
        prepos = *(da->crossPoint.pos + (da->crossPoint.Width - 1));
    } else
        prepos = 0;
    da->crossPoint.Width = getCross(da->dataBuf.tmp + da->settings.frames, da->crossPoint.pos, da->settings.frames);
    if (da->crossPoint.Width <= 0) {
        // printf("Error getCross\n");
        da->crossPoint.Width = 0;
    }

    // Sound process ************
    memmove(da->dataBuf.sound, da->dataBuf.sound + da->settings.frames, da->settings.frames * sizeof(double));
    if (da->flag.octaveHi) {
        if (da->crossPoint.Width >= 2)
            overlength = octaveHi(da, overlength, prepos);
        else {
            memcpy(da->dataBuf.sound + overlength, da->dataBuf.tmp + da->settings.frames,
                   da->settings.frames * sizeof(double));
        }
        if (overlength > da->settings.frames) {
            overlength = 0;
            printf("Error in soundProcess()overlength limit over-Hi\n");
        }
    } else if (da->flag.octaveLo) {
        if (da->crossPoint.Width >= 2)
            overlength = octaveLo(da, overlength, prepos);
        else {
            memcpy(da->dataBuf.sound + overlength, da->dataBuf.tmp + da->settings.frames,
                   da->settings.frames * sizeof(double));
        }
        if (overlength > da->settings.frames) {
            overlength = 0;
            printf("Error in soundProcess()overlength limit over-Lo\n");
        }
    } else {
        memcpy(da->dataBuf.sound, da->dataBuf.tmp + da->settings.frames, da->settings.frames * sizeof(double));
    }

    // FFT Cal. **********

    fftw_execute(da->fftForward.fftPlan);

    for (i = 0; i < da->settings.frames; i++) {
        *(da->fftPower + i) = log((da->fftForward.dataOut[i][0] * da->fftForward.dataOut[i][0]) +
                                  (da->fftForward.dataOut[i][1] * da->fftForward.dataOut[i][1]) + 1.0) *
                              2000.0;
        da->fftBack.dataIn[i][0] = da->fftForward.dataOut[i][0];
        da->fftBack.dataIn[i][1] = da->fftForward.dataOut[i][1];
    }
    fftw_execute(da->fftBack.fftPlan);
    for (i = 0; i < da->settings.frames / 2; i++) {
        da->fftCep.dataIn[i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[da->settings.frames - 1 - i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[i][1] = 0.0;
        da->fftCep.dataIn[da->settings.frames - 1 - i][1] = 0.0;
    }

    fftw_execute(da->fftCep.fftPlan);

    for (i = 0; i < da->settings.frames; i++) {
        *(da->fftCepstrum + i) = (da->fftCep.dataOut[i][0] * da->fftCep.dataOut[i][0] +
                                  da->fftCep.dataOut[i][1] * da->fftCep.dataOut[i][1]) /
                                 100000.0;
    }
    memcpy(da->draw1[0].y, da->dataBuf.row + da->settings.frames, da->settings.frames * sizeof(double));
    memcpy(da->draw1[1].y, da->dataBuf.sound, da->settings.frames * sizeof(double));

    memcpy(da->draw2[0].y, da->fftPower, da->settings.frames * sizeof(double));
    memcpy(da->draw2[1].y, da->fftCepstrum, da->settings.frames * sizeof(double));

    // Processed sound FFT

    for (i = 0; i < da->settings.frames; i++) {
        da->fftForward.dataIn[i][0] = (double)(*(da->dataBuf.sound + i)) * *(da->fftWindow + i);
        da->fftForward.dataIn[i][1] = 0.0;
    }
    fftw_execute(da->fftForward.fftPlan);
    for (i = 0; i < da->settings.frames; i++) {
        *(da->fftPower + i) = log((da->fftForward.dataOut[i][0] * da->fftForward.dataOut[i][0]) +
                                  (da->fftForward.dataOut[i][1] * da->fftForward.dataOut[i][1]) + 1.0) *
                              2000.0;
    }
    for (i = 0; i < da->settings.frames / 2; i++) {
        da->fftCep.dataIn[i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[da->settings.frames - 1 - i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[i][1] = 0.0;
        da->fftCep.dataIn[da->settings.frames - 1 - i][1] = 0.0;
    }

    fftw_execute(da->fftCep.fftPlan);
    for (i = 0; i < da->settings.frames; i++) {
        *(da->fftCepstrum + i) = (da->fftCep.dataOut[i][0] * da->fftCep.dataOut[i][0] +
                                  da->fftCep.dataOut[i][1] * da->fftCep.dataOut[i][1]) /
                                 100000.0;
    }

    memcpy(da->draw2[2].y, da->fftPower, da->settings.frames * sizeof(double));
    memcpy(da->draw2[3].y, da->fftCepstrum, da->settings.frames * sizeof(double));

    // Sound data send to pcm sample

    for (i = 0; i < da->settings.frames; i++) {
        *(da->soundWrite.samples + i * 2) = (int16_t)(*(da->dataBuf.sound + i));
        *(da->soundWrite.samples + i * 2 + 1) = (int16_t)(*(da->dataBuf.sound + i));
    }

    n++;
    if (da->settings.rate / (da->settings.frames * 8) < n) {
        n = 0;
        da->flag.drawArea1 = 1;
        g_idle_add(update_drawArea1, da);

        da->flag.drawArea2 = 1;
        g_idle_add(update_drawArea2, da);
    }
}