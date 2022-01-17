#ifndef __DRAWAREA__H
#define __DRAWAREA__H
//#include "header.h"

GMutex mutex_drawArea1;
GMutex mutex_drawArea2;

gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean button_press_event_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean button_release_event_cb(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean motion_notify_event_cb(GtkWidget *widget, GdkEventMotion *event, gpointer data);

gboolean update_drawArea1(gpointer data);
gboolean update_drawArea2(gpointer data);
gboolean update_drawArea3(gpointer data);
gboolean update_drawArea4(gpointer data);
#endif