#include "soundProcess.h"

void soundProcess(VApp* da) {
    int i;
    _Thread_local static int bufferCount = 0;
    _Thread_local static double in1 = 0.0;
    _Thread_local static double in2 = 0.0;
    _Thread_local static double out1 = 0.0;
    _Thread_local static double out2 = 0.0;

    if (da->flag.soundFile) {
        // Data Buffer has two room. Second room for new data
        memmove(da->dataBuf.row, da->dataBuf.row + da->soundWrite.bufferSize,
                da->soundWrite.bufferSize * sizeof(double));
        for (i = 0; i < da->soundWrite.bufferSize; i++) {
            *(da->dataBuf.row + da->soundWrite.bufferSize + i) = (double)(*(da->dataBuf.read + i * 2 + bufferCount));
            da->fftForward.dataIn[i][0] = (double)(*(da->dataBuf.read + i * 2 + bufferCount)) * *(da->fftWindow + i);
            da->fftForward.dataIn[i][1] = 0.0;
        }
        if(!da->flag.pause) bufferCount += da->soundWrite.bufferSize * da->settings.channels;
        if (bufferCount + da->soundWrite.bufferSize * da->settings.channels > da->dataBuf.readSize / 2)
            bufferCount = 0;
    } else if (da->flag.soundMic) {
        memmove(da->dataBuf.row, da->dataBuf.row + da->soundWrite.bufferSize,
                da->soundWrite.bufferSize * sizeof(double));
        for (i = 0; i < da->soundWrite.bufferSize; i++) {
            *(da->dataBuf.row + da->soundWrite.bufferSize + i) = (double)(*(da->soundRead.samples + i * 2));
            da->fftForward.dataIn[i][0] = (double)(*(da->soundRead.samples + i * 2)) * *(da->fftWindow + i);
            da->fftForward.dataIn[i][1] = 0.0;
        }
    }

    // Row -> IIR filter -> Sound *********
    // Data Buffer has two room. Second room for new data
    memmove(da->dataBuf.sound, da->dataBuf.sound + da->soundWrite.bufferSize,
            da->soundWrite.bufferSize * sizeof(double));
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

        for (i = 0; i < da->settings.pcm_buffer_size; i++) {
            *(da->dataBuf.sound + da->soundWrite.bufferSize + i) =
                b0 / a0 * (*(da->dataBuf.row + da->soundWrite.bufferSize + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                      // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->soundWrite.bufferSize + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                                 // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.sound + da->soundWrite.bufferSize + i); // 1つ前の出力信号を更新
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

        for (i = 0; i < da->settings.pcm_buffer_size; i++) {
            *(da->dataBuf.sound + da->soundWrite.bufferSize + i) =
                b0 / a0 * (*(da->dataBuf.row + da->soundWrite.bufferSize + i) * 2.0) + b1 / a0 * in1 + b2 / a0 * in2 -
                a1 / a0 * out1 - a2 / a0 * out2;

            in2 = in1;                                                      // 2つ前の入力信号を更新
            in1 = *(da->dataBuf.row + da->soundWrite.bufferSize + i) * 2.0; // 1つ前の入力信号を更新

            out2 = out1;                                                 // 2つ前の出力信号を更新
            out1 = *(da->dataBuf.sound + da->soundWrite.bufferSize + i); // 1つ前の出力信号を更新
        }

    } else {
        memcpy(da->dataBuf.sound + da->soundWrite.bufferSize, da->dataBuf.row + da->soundWrite.bufferSize,
               da->soundWrite.bufferSize * sizeof(double));
    }

    // FFT Cal. **********

    fftw_execute(da->fftForward.fftPlan);

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->fftPower + i) = log((da->fftForward.dataOut[i][0] * da->fftForward.dataOut[i][0]) +
                                  (da->fftForward.dataOut[i][1] * da->fftForward.dataOut[i][1]) + 1.0) *
                              2000.0;
        da->fftBack.dataIn[i][0] = da->fftForward.dataOut[i][0];
        da->fftBack.dataIn[i][1] = da->fftForward.dataOut[i][1];
    }
    fftw_execute(da->fftBack.fftPlan);
    for (i = 0; i < da->soundWrite.bufferSize / 2; i++) {
        da->fftCep.dataIn[i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[da->soundWrite.bufferSize - 1 - i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[i][1] = 0.0;
        da->fftCep.dataIn[da->soundWrite.bufferSize - 1 - i][1] = 0.0;
    }

    fftw_execute(da->fftCep.fftPlan);

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->fftCepstrum + i) = (da->fftCep.dataOut[i][0] * da->fftCep.dataOut[i][0] +
                                  da->fftCep.dataOut[i][1] * da->fftCep.dataOut[i][1]) /
                                 100000.0;
    }
    memcpy(da->draw1[0].x, da->dataBuf.row + (da->soundWrite.bufferSize / 2),
           da->soundWrite.bufferSize * sizeof(double));
    memcpy(da->draw1[1].x, da->dataBuf.sound + (da->soundWrite.bufferSize / 2),
           da->soundWrite.bufferSize * sizeof(double));

    memcpy(da->draw2[0].x, da->fftPower, da->soundWrite.bufferSize * sizeof(double));
    memcpy(da->draw2[1].x, da->fftCepstrum, da->soundWrite.bufferSize * sizeof(double));

    // Processed sound FFT

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        da->fftForward.dataIn[i][0] = (double)(*(da->dataBuf.sound + i)) * *(da->fftWindow + i);
        da->fftForward.dataIn[i][1] = 0.0;
    }
    fftw_execute(da->fftForward.fftPlan);
    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->fftPower + i) = log((da->fftForward.dataOut[i][0] * da->fftForward.dataOut[i][0]) +
                                  (da->fftForward.dataOut[i][1] * da->fftForward.dataOut[i][1]) + 1.0) *
                              2000.0;
    }
    for (i = 0; i < da->soundWrite.bufferSize / 2; i++) {
        da->fftCep.dataIn[i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[da->soundWrite.bufferSize - 1 - i][0] = *(da->fftPower + i) / 100.0;
        da->fftCep.dataIn[i][1] = 0.0;
        da->fftCep.dataIn[da->soundWrite.bufferSize - 1 - i][1] = 0.0;
    }

    fftw_execute(da->fftCep.fftPlan);
    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->fftCepstrum + i) = (da->fftCep.dataOut[i][0] * da->fftCep.dataOut[i][0] +
                                  da->fftCep.dataOut[i][1] * da->fftCep.dataOut[i][1]) /
                                 100000.0;
    }

    memcpy(da->draw2[2].x, da->fftPower, da->soundWrite.bufferSize * sizeof(double));
    memcpy(da->draw2[3].x, da->fftCepstrum, da->soundWrite.bufferSize * sizeof(double));

    // Sound data send to pcm sample

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->soundWrite.samples + i * 2) = (int16_t)(*(da->dataBuf.sound + i));
        *(da->soundWrite.samples + i * 2 + 1) = (int16_t)(*(da->dataBuf.sound + i));
    }
    

    g_idle_add(update_drawArea1, da);
    g_idle_add(update_drawArea2, da);
}