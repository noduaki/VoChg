#ifndef __DRAWAREA__H
#define __DRAWAREA__H

#include <gtk/gtk.h>
#include "MApp.h"
gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer  data);
gboolean draw_cb (GtkWidget *widget, cairo_t   *cr, gpointer data);
gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean button_release_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data);
#endif