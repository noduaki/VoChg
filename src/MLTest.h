#ifndef __MLTEST__H
#define __MLTEST__H

#include "header.h"

void getWeight(GtkWidget* window, gpointer data);
void mlTest(GTask* stask, gpointer source_object, gpointer data, GCancellable* cancellable);
#endif