#include "header.h"

cairo_surface_t* surface1 = NULL;
cairo_surface_t* surface2 = NULL;

static void clear_surface(int i) {
    cairo_t* cr;
    if (i == 1) {
        cr = cairo_create(surface1);
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.1);
    } else {
        cr = cairo_create(surface2);
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
    }
    cairo_paint(cr);
    cairo_destroy(cr);
}

gboolean configure_event_cb(GtkWidget* widget, GdkEventConfigure* event, gpointer data) {
    int i;
    VApp* da = (VApp*)data;
    da->drawstatus2.Width = gtk_widget_get_allocated_width(da->priv->draw2);
    da->drawstatus2.Height = gtk_widget_get_allocated_height(da->priv->draw2);
    da->drawstatus1.Width = gtk_widget_get_allocated_width(da->priv->draw1);
    da->drawstatus1.Height = gtk_widget_get_allocated_height(da->priv->draw1);

    if (widget == da->priv->draw1) {
        i = 1;
        cairo_surface_destroy(surface1);
        surface1 = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR_ALPHA,
                                                     gtk_widget_get_allocated_width(widget),
                                                     gtk_widget_get_allocated_height(widget));
    } else {
        i = 2;
        cairo_surface_destroy(surface2);
        surface2 = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR_ALPHA,
                                                     gtk_widget_get_allocated_width(widget),
                                                     gtk_widget_get_allocated_height(widget));
    }

    /* Initialize the surface to white */
    clear_surface(i);

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}
gboolean draw_cb(GtkWidget* widget, cairo_t* cr, gpointer data) {
    VApp* da = (VApp*)data;

    if (widget == da->priv->draw1)
        cairo_set_source_surface(cr, surface1, 0, 0);
    else
        cairo_set_source_surface(cr, surface2, 0, 0);

    cairo_paint(cr);

    return FALSE;
}

gboolean button_press_event_cb(GtkWidget* widget, GdkEventButton* event, gpointer data) {
    int a = 0;
    VApp* da = (VApp*)data;

    if (widget == da->priv->draw1) {
        a = 1;
        if (surface1 == NULL)
            return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL)
            return FALSE;
    }
    if (a == 1) {
        if (event->button == GDK_BUTTON_PRIMARY) {
            statusprint("Text Draw1 Press", da);
        } else if (event->button == GDK_BUTTON_SECONDARY) {
        }
    }
    if (a == 2) {
        if (event->button == GDK_BUTTON_PRIMARY) {
        } else if (event->button == GDK_BUTTON_SECONDARY) {
        }
    }
    /* We've handled the event, stop processing */
    return TRUE;
}

gboolean button_release_event_cb(GtkWidget* widget, GdkEventButton* event, gpointer data) {
    int a = 0;
    VApp* da = (VApp*)data;
    /* paranoia check, in case we haven't gotten a configure event */
    if (widget == da->priv->draw1) {
        a = 1;
        if (surface1 == NULL)
            return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL)
            return FALSE;
    }
    if (a == 2) {
        if (event->button == GDK_BUTTON_PRIMARY) {
            statusprint("Text Draw Release", da);
        } else if (event->button == GDK_BUTTON_SECONDARY) {
        }
    }

    /* We've handled the event, stop processing */
    return TRUE;
}

gboolean motion_notify_event_cb(GtkWidget* widget, GdkEventMotion* event, gpointer data) {
    VApp* da = (VApp*)data;
    static double start = 0.0;
    static double end = 0.0;
    int a = 0;
    int x;
    double xx;
    double y;

    // paranoia check, in case we haven't gotten a configure event
    if (widget == da->priv->draw1) {
        a = 1;
        if (surface1 == NULL)
            return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL)
            return FALSE;
    }
    return TRUE;
}

GMutex mutex_drawArea1;
GMutex mutex_drawArea2;

gboolean update_drawArea1(gpointer data) {
    int i;
    int n;
    int tmp;
    int v;
    double sitmp = 0;
    double ttmp = 0;
    cairo_t* cr;
    VApp* da = (VApp*)data;

    static double t = 0;
    static double si;

    cr = cairo_create(surface1);
    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 1.0);
    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 13);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "WAVE");
    da->flag.drawArea = 1;

    g_mutex_lock(&mutex_drawArea1);
    for (n = 0; n < 5; n++) {
        switch (n) {
        case 0:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        case 1:
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        case 2:
            cairo_set_source_rgba(cr, 0.5, 1.0, 0.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        case 3:
            cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        case 4:
            cairo_set_source_rgba(cr, 1.0, 0.0, 1.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;

        default:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        }

        sitmp = si;
        ttmp = 0;
        if (da->draw1[n].on) {
            for (i = 0; i < da->draw1[n].Width; i++) {

                si = (double)((da->drawstatus1.Height) / 2) -
                     (*(da->draw1[n].x + i) * (double)(da->drawstatus1.Height) / da->draw1[n].Height);
                t = (double)i * (double)da->drawstatus1.Width / (double)(da->draw1[n].Width);

                if (si < 0.0)
                    si = 0.0;
                else if (si > da->drawstatus1.Height)
                    si = da->drawstatus1.Height;
                if (t < 0.0)
                    t = 0.0;
                if (t > da->drawstatus1.Width - 6.0)
                    t = da->drawstatus1.Width - 6.0;

                cairo_move_to(cr, (ttmp + 5.0), sitmp);
                cairo_line_to(cr, (t + 5.0), si);
                sitmp = si;
                ttmp = t;
            }
            cairo_stroke(cr);
        }
    }
    g_mutex_unlock(&mutex_drawArea1);
    
    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw1);
    da->flag.drawArea = 0;
    return FALSE;
}

gboolean update_drawArea2(gpointer data) {
    int i;
    int n;
    int tmp;
    int v;
    double sitmp = 0;
    double ttmp = 0;
    cairo_t* cr;
    VApp* da = (VApp*)data;

    static double t = 0;
    static double si;

    cr = cairo_create(surface2);
    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 1.0);
    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 13);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "FFT");
    cairo_set_font_size(cr, 8);
    if (da->draw2[0].log) {
        
        cairo_move_to(cr, ((log(608) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "7k");
        cairo_move_to(cr, ((log(304) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "3.5k");
        cairo_move_to(cr, ((log(152) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "1.7k");
        cairo_move_to(cr, ((log(76) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "880");
        cairo_move_to(cr, ((log(38) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "440");
        cairo_move_to(cr, ((log(18.5) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "220");
        cairo_move_to(cr, ((log(9.3) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "110");
        cairo_move_to(cr, ((log(4.6) - 0.693) * (da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height );
        cairo_show_text(cr, "55");
        cairo_move_to(cr, 5.0, (double)da->drawstatus2.Height);
        cairo_show_text(cr, "20");
    }else{
        cairo_move_to(cr, 1024.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "24k");
        cairo_move_to(cr, 512.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width) + 5.0, (double)da->drawstatus2.Height);
        cairo_show_text(cr, "12k");
        cairo_move_to(cr, 256.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width) + 5, (double)da->drawstatus2.Height);
        cairo_show_text(cr, "6k");
        cairo_move_to(cr, 128.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "3k");
        
    }
    da->flag.drawArea = 1;
    g_mutex_lock(&mutex_drawArea2);
    for (n = 0; n < 5; n++) {
        switch (n) {
        case 0:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);
            cairo_set_line_width(cr, 2.0);
            break;
        case 1:
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.5);
            cairo_set_line_width(cr, 2.0);
            break;
        case 2:
            cairo_set_source_rgba(cr, 0.5, 1.0, 0.0, 0.6);
            cairo_set_line_width(cr, 2.0);
            break;
        case 3:
            cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.3);
            cairo_set_line_width(cr, 2.0);
            break;
        case 4:
            cairo_set_source_rgba(cr, 1.0, 0.0, 1.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;

        default:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.2);
            cairo_set_line_width(cr, 2.0);
            break;
        }

        sitmp = si;
        ttmp = 0;
        if (da->draw2[n].on) {
            for (i = 0; i < da->draw2[n].Width; i++) {

                si = (double)(da->drawstatus2.Height) -
                     (*(da->draw2[n].x + i) * (double)(da->drawstatus2.Height) / da->draw2[n].Height);
                if (da->draw2[n].log)
                    t = (log((double)i) - 0.693) * (da->drawstatus2.Width / 6.5);
                else
                    t = (double)i * (double)da->drawstatus2.Width / (double)(da->draw2[n].Width);

                if (si < 10.0)
                    si = 10.0;
                else if (si > da->drawstatus2.Height)
                    si = da->drawstatus2.Height;
                if (t < 0.0)
                    t = 0.0;
                if (t > da->drawstatus2.Width - 6.0)
                    t = da->drawstatus2.Width - 6.0;

                if (da->draw2[n].bar) {
                    cairo_move_to(cr, (t + 5.0), (double)(da->drawstatus2.Height) - 10.0);
                    cairo_line_to(cr, (t + 5.0), si - 10.0);

                } else {

                    cairo_move_to(cr, (ttmp + 5.0), sitmp - 10.0);
                    cairo_line_to(cr, (t + 5.0), si - 10.0);
                }
                sitmp = si;
                ttmp = t;
            }
            cairo_stroke(cr);
        }
    }

    g_mutex_unlock(&mutex_drawArea2);
    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw2);
    da->flag.drawArea = 0;
    return FALSE;
}
