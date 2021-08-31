#ifndef __MAPPWIN_H
#define __MAPPWIN_H
#include "header.h"

typedef struct _MAppWindowPrivate {
    GSettings *settings;    // settings
    GtkWidget *content_box; // UI file Widget Registrasion
    GtkWidget *content_boxIn;
    GtkWidget *textview;

    GtkWidget *entrytext;
    GtkWidget *entry1;
    GtkWidget *draw1;
    GtkWidget *draw2;

    GtkWidget *selectButton1;
    GtkWidget *selectButton2;
    GtkWidget *selectButton3;
    GtkWidget *stopButton;
    GtkWidget *selLowerButton1;
    GtkWidget *selLowerButton2;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button3;
    GtkWidget *button4;
    GtkWidget *button5;
    GtkWidget *button6;
    GtkWidget *button7;
    GtkWidget *button8;
    GtkWidget *button9;
    GtkWidget *button10;
    GtkWidget *button11;
    GtkWidget *button12;
    GtkWidget *button13;
    GtkWidget *button14;
    GtkWidget *button15;
    GtkWidget *button16;
    GtkWidget *button17;
    GtkWidget *button18;
    GtkWidget *button19;
    GtkWidget *button20;

    GtkWidget *scale1;
    GtkWidget *scale2;
    GtkWidget *scale3;
    GtkWidget *scale4;
    GtkWidget *scale5;
    GtkWidget *scale6;
    GtkWidget *scale7;
    GtkWidget *scale8;
    GtkWidget *scale9;
    GtkWidget *scale10;
    GtkWidget *scale11;
    GtkWidget *scale12;
    GtkWidget *scale13;
    GtkWidget *scale14;
    GtkWidget *scale15;

    GtkWidget *popuplist1;
    GtkWidget *popuplist2;
    GtkWidget *popuplist3;
    GtkWidget *popuplist4;
    GtkWidget *popuplist5;

    GtkWidget *popupMenu;

} MAppWindowPrivate;

typedef struct _mInt {
    int x;
    int y;
    int z;
    int *pData;
} mInt;

typedef struct _mFloat {
    float data1;
    float data2;
    float data3;
    float *pData;
} mFloat;

typedef struct _mBuffer {
    int16_t *read;
    int readSize;
    int16_t *write;
    int writeSize;
    double *sound;
    int soundSize;
    double *row;
    int rowSize;

} mBuffer;

typedef struct _mFFTdata {
    fftw_complex *dataIn;
    fftw_complex *dataOut;
    fftw_plan fftPlan;
    int size;
} mFFTdata;

typedef struct async_private_data {
    int16_t *samples;
    snd_pcm_channel_area_t *areas;
    double phase;
    int pos;
    int avail;
    int ready;
    int periodsize;
    int bufferSize;
} mAsyncData;

typedef struct _mFlag {
    int soundFile;
    int soundMic;
    int drawArea1;
    int drawArea2;
    int drawResize;
    int drawReSlider;
    int pause;
    int nextWave;
    int prevWave;
    int selPos;
} mFlag;

typedef struct _mScale {
    double slider1;
    double slider2;
    double slider3;
    double slider4;
    double slider5;
    double slider6;
    double slider7;
    double slider8;
    double slider9;
    double slider10;
    double slider11;
    double slider12;
    double slider13;
    double slider14;
    double slider15;

} mScale;

typedef struct _mSetting {

    unsigned long pcm_buffer_size;
    unsigned long period_size;
    snd_pcm_format_t format;
    int    channels;
    int    rate;
    int    frames;
    char   deviceName[256];
    char   filename[256];
    GFile  *file;
    int    filter;
} mSettings;

typedef struct _mDraw {
    double *x;
    double *y;
    int Width;
    int Height;
    int on;
    int log;
    int bar;

} mDraw;

typedef struct _mStatus {
    int open;
    int selNum;
    int ref;
} mStatus;

typedef struct _mlData{
    double xData[NUM_DATA_X];
    double yData;
}mlData;

typedef struct _VApp {
    MAppWindowPrivate *priv;
    mStatus status;
    mSettings settings;
    mInt dataInt;
    mFloat dataFloat;
    mBuffer dataBuf;
    mFlag flag;
    mScale scale;
    mDraw drawstatus1;
    mDraw drawstatus2;
    mDraw draw1[5];
    mDraw draw2[5];
    mDraw crossPoint;
    double entry1;

    mAsyncData soundRead;
    mAsyncData soundWrite;
    char *statusBuf;

    mFFTdata fftForward;
    mFFTdata fftBack;
    mFFTdata fftCep;
    double *fftWindow;
    double *fftPower;
    double *fftCepstrum;

    mlData* sData;
    mInt    clickPos;
    mInt    selPointS;
    mInt    selPointE;
    mInt    nextPoint;
    int     drawStartPos;
    int     drawEndPos;
} VApp;

void soundThread(GtkWidget *window, gpointer data);
void mlDataThread(GtkWidget *window, gpointer data);
void SB3Thread(GtkWidget *window, gpointer data);
MAppWindow *M_app_window_new(MApp *app);
void M_app_text_open(MAppWindow *win, GFile *file);

#endif