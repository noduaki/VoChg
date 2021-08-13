#ifndef __MApp__H
#define __MApp__H

#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define M_APP_TYPE (M_app_get_type ())
G_DECLARE_FINAL_TYPE (MApp, M_app, M, APP, GtkApplication)

#define M_APP_WINDOW_TYPE (M_app_window_get_type ())
G_DECLARE_FINAL_TYPE (MAppWindow, M_app_window, M, APP_WINDOW, GtkApplicationWindow)

#define M_APP_PREFS_TYPE (M_app_prefs_get_type ())
G_DECLARE_FINAL_TYPE (MAppPrefs, M_app_prefs, M, APP_PREFS, GtkDialog)

#include "MAppwin.h" //<-MAppwin.h use above G_DECLARTION so "MAppwin.h" be here
#include "menubar.h"
#include "scalSlider.h"
#include "button.h"
#include "widgets.h"
#include "drawArea.h"


MApp *M_app_new(void);

#endif