#ifndef __HEADER__H
#define __HEADER__H


#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <time.h>
#include <alsa/asoundlib.h>


#define SOUNDRATE      48000
#define SOUNDCHANNELS  2
#define SOUNDFORMAT    SND_PCM_FORMAT_S16
#define SOUNDBUFFERSIZE 8192
#define SOUNDPERIODSIZE 2048
#define SOUNDFRAMES     SOUNDBUFFERSIZE

#define NUM_MLDATA      1000
#define NUM_DATA_X       100

#define M_APP_TYPE (M_app_get_type ())
G_DECLARE_FINAL_TYPE (MApp, M_app, M, APP, GtkApplication)

#define M_APP_WINDOW_TYPE (M_app_window_get_type ())
G_DECLARE_FINAL_TYPE (MAppWindow, M_app_window, M, APP_WINDOW, GtkApplicationWindow)

#define M_APP_PREFS_TYPE (M_app_prefs_get_type ())
G_DECLARE_FINAL_TYPE (MAppPrefs, M_app_prefs, M, APP_PREFS, GtkDialog)

#include "MApp.h"
#include "MAppwin.h" 

#include "menubar.h"
#include "scalSlider.h"
#include "button.h"
#include "widgets.h"
#include "drawArea.h"
#include "initVar.h"
#include "sndPCMThread.h"
#include "soundProcess.h"
#include "utility.h"
#include "mlDataProcess.h"




extern mSettings gSet;
#endif