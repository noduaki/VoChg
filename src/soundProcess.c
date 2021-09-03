#include "soundProcess.h"

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

void soundProcessPoll(VApp* da) {
    int i;

    _Thread_local static int n = 0;
    _Thread_local static int bufferCount = 0;
    _Thread_local static double in1 = 0.0;
    _Thread_local static double in2 = 0.0;
    _Thread_local static double out1 = 0.0;
    _Thread_local static double out2 = 0.0;

    if (da->flag.soundFile) {
        // Data Buffer has two room. Second room is used new data
        memmove(da->dataBuf.row, da->dataBuf.row + da->settings.frames,
                da->settings.frames * sizeof(double));
        for (i = 0; i < da->settings.frames; i++) {
            *(da->dataBuf.row + da->settings.frames + i) =
                (double)((*(da->dataBuf.read + i * 2 + bufferCount + 1) + *(da->dataBuf.read + i * 2 + bufferCount)) / 2);
                
            da->fftForward.dataIn[i][0] = *(da->dataBuf.row + da->settings.frames + i) * *(da->fftWindow + i);
            da->fftForward.dataIn[i][1] = 0.0;
        }
        if (!da->flag.pause) bufferCount += da->settings.frames * da->settings.channels;
        if (bufferCount + da->settings.frames * da->settings.channels > da->dataBuf.readSize / 2) bufferCount = 0;
    } else if (da->flag.soundMic) {
        // Data Buffer has two room. Second room is used new data
        memmove(da->dataBuf.row, da->dataBuf.row + da->settings.frames,
                da->settings.frames * sizeof(double));
       // if (da->flag.deviceflag) {
            for (i = 0; i < da->settings.frames; i++) {
                if (!da->flag.pause) {
                    *(da->dataBuf.row + da->settings.frames + i) =
                        (double)((*(da->soundRead.samples + i * 2 + 1) + *(da->soundRead.samples + i * 2)) / 2);
                }
                da->fftForward.dataIn[i][0] = (double)(*(da->soundRead.samples + i * 2)) * *(da->fftWindow + i);
                da->fftForward.dataIn[i][1] = 0.0;
            }
       // }
    }

    // Row -> IIR filter -> Sound *********
    // Data Buffer has two room. Second room is used new data
    memmove(da->dataBuf.sound, da->dataBuf.sound + da->settings.frames,
            da->settings.frames * sizeof(double));
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
            *(da->dataBuf.sound + da->settings.frames + i) =
                b0 / a0 * (*(da->dataBuf.row + da->settings.frames + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                      // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->settings.frames + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                                 // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.sound + da->settings.frames + i); // 1つ前の出力信号を更新
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
            *(da->dataBuf.sound + da->settings.frames + i) =
                b0 / a0 * (*(da->dataBuf.row + da->settings.frames + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                      // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->settings.frames + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                                 // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.sound + da->settings.frames + i); // 1つ前の出力信号を更新
        }

    } else { // No filter
        memcpy(da->dataBuf.sound + da->settings.frames, da->dataBuf.row + da->settings.frames,
               da->settings.frames * sizeof(double));
    }

    // ML data get *********

    da->crossPoint.Width =
        getCross(da->dataBuf.sound + da->settings.frames, da->crossPoint.x, da->settings.frames);
    if (da->crossPoint.Width <= 0) {
        //printf("Error getCross\n");
        da->crossPoint.Width = 0;
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
    memcpy(da->draw1[0].y, da->dataBuf.row + (da->settings.frames), da->settings.frames * sizeof(double));
    memcpy(da->draw1[1].y, da->dataBuf.sound + (da->settings.frames), da->settings.frames * sizeof(double));

    memcpy(da->draw2[0].y, da->fftPower, da->settings.frames * sizeof(double));
    memcpy(da->draw2[1].y, da->fftCepstrum, da->settings.frames * sizeof(double));

    // Processed sound FFT

    for (i = 0; i < da->settings.frames; i++) {
        da->fftForward.dataIn[i][0] =
            (double)(*(da->dataBuf.sound + da->settings.frames + i)) * *(da->fftWindow + i);
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
   // if (da->flag.deviceflag) {
        for (i = 0; i < da->settings.frames; i++) {
            *(da->soundWrite.samples + i * 2) = (int16_t)(*(da->dataBuf.sound + i));
            *(da->soundWrite.samples + i * 2 + 1) = (int16_t)(*(da->dataBuf.sound + i));
        }
   // }
    n++;
    if (da->settings.rate / (da->settings.frames * 8) < n) {
        n = 0;
        da->flag.drawArea1 = 1;
        g_idle_add(update_drawArea1, da);

        da->flag.drawArea2 = 1;
        g_idle_add(update_drawArea2, da);
    }
}