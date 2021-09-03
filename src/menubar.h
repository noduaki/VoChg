#ifndef __MENUBAR__H
#define __MENUBAR__H
#include "header.h"

MAppPrefs * M_app_prefs_new(MAppWindow * win);

void activate_openfile(GSimpleAction * simple, GVariant * parameter, gpointer app);
void activate_color(GSimpleAction * simple, GVariant * parameter, gpointer app);
void choosescard(GSimpleAction* simple, GVariant* parameter, gpointer app);
void choosesframes(GSimpleAction* simple, GVariant* parameter, gpointer app);
void choosesrate(GSimpleAction* simple, GVariant* parameter, gpointer app);
#endif
