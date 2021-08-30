#include "header.h"
int wavCheck(gpointer tmp) {
    VApp* da = (VApp*)tmp;

    char* data = (char*)da->dataBuf.read;

    int ok = 1;
    int setting = 0;
    int size;
    int i = 0;

    while (ok) {
        if (data[i] == 0x52 && data[i + 1] == 0x49 && data[i + 2] == 0x46 && data[i + 3] == 0x46)
            break; // seek "RIFF"
        else {
            i++;
            if (i > 1) {
                ok = 0;
                break;
            }
        }
    }
    while (ok) {
        if (data[i] == 0x57 && data[i + 1] == 0x41 && data[i + 2] == 0x56 && data[i + 3] == 0x45)
            break; // seek "WAVE"
        else {
            i++;
            if (i > 8) {
                ok = 0;
                break;
            }
        }
    }
    while (ok) {
        if (data[i] == 0x10 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x00)
            break; // seek "0x10000000" = 16
        else {
            i++;
            if (i > 16) {
                ok = 0;
                break;
            }
        }
    }
    if (ok) {
        setting = (0x000000ff & *(data + i + 6));

        if (setting == 2)
            da->settings.channels = setting;
        else
            ok = 1; // 0;
    }
    if (ok) {
        setting = (0xff000000 & (data[i + 11] << 24)) | (0x00ff0000 & (data[i + 10] << 16)) |
                  (0x00000ff00 & (data[i + 9] << 8)) | (0x000000ff & data[i + 8]);
        da->settings.rate = setting;
    }
    while (ok) {
        if (data[i] == 0x64 && data[i + 1] == 0x61 && data[i + 2] == 0x74 && data[i + 3] == 0x61)
            break; // seek "data"
        else {
            i++;
            if (i > 196) ok = 0;
        }
    }
    if (ok) {
        size = (0xff000000 & (data[i + 7] << 24)) | (0x00ff0000 & (data[i + 6] << 16)) |
               (0x00000ff00 & (data[i + 5] << 8)) | (0x000000ff & data[i + 4]);
        i += 8;
        if (size < da->dataBuf.readSize) da->dataBuf.readSize = size;
        printf("File size %d,\n", da->dataBuf.readSize);
    }
    if (ok) {
        memmove(da->dataBuf.read, da->dataBuf.read + i, da->dataBuf.readSize);
    }

    if (ok)
        return 0;
    else {
        printf("It's not WAV !\n");
        return 1;
    }
}