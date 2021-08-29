#include "soundProcess.h"

void soundProcess(VApp* da) {
    int i;
    _Thread_local static int bufferCount = 0;

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->dataBuf.row + i) = (double)(*(da->dataBuf.read + i * 2 + bufferCount));
        da->fftForward.dataIn[i][0] = (double)(*(da->dataBuf.read + i * 2 + bufferCount)) * *(da->fftWindow + i);
        da->fftForward.dataIn[i][1] = 0.0;
    }
    memcpy(da->dataBuf.sound, da->dataBuf.row, da->soundWrite.bufferSize * sizeof(double));

    for (i = 0; i < da->soundWrite.bufferSize; i++) {
        *(da->soundWrite.samples + i * 2) = (int16_t)(*(da->dataBuf.row + i));
        *(da->soundWrite.samples + i * 2 + 1) = (int16_t)(*(da->dataBuf.sound + i));
    }
    bufferCount += da->soundWrite.bufferSize * da->settings.channels;

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

    memcpy(da->draw1[0].x, da->dataBuf.sound, da->soundWrite.bufferSize * sizeof(double));
    memcpy(da->draw1[1].x, da->dataBuf.row, da->soundWrite.bufferSize * sizeof(double));

    memcpy(da->draw2[0].x, da->fftPower, da->soundWrite.bufferSize * sizeof(double));
    memcpy(da->draw2[1].x, da->fftCepstrum, da->soundWrite.bufferSize * sizeof(double));

    g_idle_add(update_drawArea1, da);
    g_idle_add(update_drawArea2, da);
    if (bufferCount + da->soundWrite.bufferSize * da->settings.channels > da->dataBuf.readSize / 2)
        bufferCount = 0;
}