#ifndef __MLDATAPROCESS__H
#define __MLDATAPROCESS__H

//#include "header.h"
int waveDiff(int startX, int endX, double* wave, double *diffWave);
void mlDataProcess(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable);
#endif