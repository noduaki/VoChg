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
    char status[20];
    int x;
    int y;

    if (widget == da->priv->draw1) {
        a = 1;
        if (surface1 == NULL) return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL) return FALSE;
    }
    if (a == 1) {
        if (event->button == GDK_BUTTON_PRIMARY) {
            if (a = 1) {
                da->drawstatus1.log = (int)(event->x);
                da->drawstatus1.on = (int)(event->y);
            }
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
        if (surface1 == NULL) return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL) return FALSE;
    }

    if (a == 1) {
        if (event->button == GDK_BUTTON_PRIMARY && da->status.open) {

            da->clickPos.x = (int)event->x;
            da->clickPos.y = (int)event->y;
            if (da->clickPos.y < da->drawstatus1.Height / 2 + 30 && da->clickPos.y > da->drawstatus1.Height / 2 - 30) {
                da->flag.selPos = 1;
            }
            sprintf(da->statusBuf, "x=%5.3f y=%5.3f", event->x, event->y);
            statusprint(data);
        }
    } else if (a == 2) {
        if (event->button == GDK_BUTTON_PRIMARY) {

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
    int i = 1;
    int x;
    int y;
    int xx;
    int yy;
    char status[100];

    // paranoia check, in case we haven't gotten a configure event
    if (widget == da->priv->draw1) {
        a = 1;
        if (surface1 == NULL) return FALSE;
    } else {
        a = 2;
        if (surface2 == NULL) return FALSE;
    }

    if (a == 1) {
        if (event->state & GDK_BUTTON1_MASK && da->status.open) {

            *(da->drawstatus1.x) = da->drawstatus1.log - (int)event->x;
            *(da->drawstatus1.y) = -(da->drawstatus1.on - (int)event->y);
            //  sprintf(status, "x=%5.3f y=%5.3f", *(da->drawstatus1.x), *(da->drawstatus1.y));
            //  statusprint(status, data);
            da->flag.drawResize = 1;
        }
    }

    return TRUE;
}

// DrawArea drawing ****************************************
// **                                                     **
// **                                                     **
// *********************************************************

GMutex mutex_drawArea1;
GMutex mutex_drawArea2;

gboolean update_drawArea1(gpointer data) {
    int i;
    int n;
    int tmp;
    int v;
    double sitmp = 0.0;
    double ttmp = 0.0;
    double height = 0.0;
    double width = 0.0;
    static int startPos = 0;
    static int endPos = 4096;
    static char startW[10] = "0";
    static char endW[10] = "0";
    cairo_t* cr;
    VApp* da = (VApp*)data;

    static double t = 0;
    static double si;

    g_mutex_lock(&mutex_drawArea1);

    if (da->flag.drawResize) {
        da->flag.drawResize = 0;
        if (da->flag.drawReSlider) {
            da->flag.drawReSlider = 0;
            if (startPos + (int)((double)da->settings.frames / da->scale.slider4) < da->settings.frames) {
                endPos = startPos + (int)((double)da->settings.frames / da->scale.slider4);
            } else {
                startPos = da->settings.frames - (int)((double)da->settings.frames / da->scale.slider4);
                endPos = da->settings.frames - 1;
            }

        } else {
            startPos += (int)*(da->drawstatus1.x) / 8;
            if (startPos < 0)
                startPos = 0;
            else if (startPos > da->settings.frames - (da->settings.frames / da->scale.slider4)) {
                startPos = da->settings.frames - (da->settings.frames / da->scale.slider4);
            }
            endPos = startPos + (int)((double)da->settings.frames / da->scale.slider4);
        }

        sprintf(startW, "%d", startPos);
        sprintf(endW, "%d", endPos);

        da->drawStartPos = startPos;
        da->drawEndPos = endPos;
    }

    cr = cairo_create(surface1);
    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0.3);
    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 13.0);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "WAVE");
    cairo_move_to(cr, 5, da->drawstatus1.Height - 10);
    cairo_show_text(cr, startW);
    cairo_move_to(cr, da->drawstatus1.Width - 40, da->drawstatus1.Height - 10);
    cairo_show_text(cr, endW);
    cairo_set_font_size(cr, 8.0);
    cairo_move_to(cr, 50.0, 10.0);
    cairo_show_text(cr, "Row");
    cairo_move_to(cr, 50.0, 19.0);
    cairo_show_text(cr, "Procd");
    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.2);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 20.0, 7.0);
    cairo_line_to(cr, 40.0, 7.0);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.2);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 20.0, 15.0);
    cairo_line_to(cr, 40.0, 15.0);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.4);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, 3.0, (double)(da->drawstatus1.Height / 2));
    cairo_line_to(cr, (double)da->drawstatus1.Width - 3.0, (double)(da->drawstatus1.Height / 2));
    cairo_stroke(cr);

    height = (double)da->drawstatus1.Height * da->scale.slider3;
    width = (double)da->drawstatus1.Width * da->scale.slider4;
    for (n = 0; n < 5; n++) {
        switch (n) {
        case 0:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
            cairo_set_line_width(cr, 2.0);
            break;
        case 1:
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.7);
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

                si = (double)(da->drawstatus1.Height / 2) -
                     (*(da->draw1[n].y + i) * height / (double)da->draw1[n].Height);
                t = (double)(i - startPos) * width / (double)(da->draw1[n].Width);

                if (si < 3.0)
                    si = 3.0;
                else if (si > (double)da->drawstatus1.Height - 3.0)
                    si = (double)da->drawstatus1.Height - 3.0;
                if (t < 0.0) t = 0.0;
                if (t > (double)da->drawstatus1.Width - 6.0) t = (double)da->drawstatus1.Width - 6.0;

                cairo_move_to(cr, (ttmp + 5.0), sitmp);
                cairo_line_to(cr, (t + 5.0), si);
                sitmp = si;
                ttmp = t;
            }
            cairo_stroke(cr);
        }
    }
    if (da->crossPoint.on) {
        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1.0);
        for (i = 0; i < da->crossPoint.Width; i++) {
            t = (double)(*(da->crossPoint.x + i) - startPos) * width / (double)(da->settings.pcm_buffer_size);
            cairo_arc(cr, t + 5.0, (double)(da->drawstatus1.Height / 2), 3.0, 0.0, 6.28318);
            cairo_fill(cr);
        }
    }
    da->flag.drawArea1 = 0; // DrawArea closing notification for end of thread sequence
    g_mutex_unlock(&mutex_drawArea1);

    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw1);

    return FALSE;
}

gboolean update_drawArea2(gpointer data) {
    int i;
    int n;
    int tmp;
    int v;
    double sitmp = 0.0;
    double ttmp = 0.0;
    double height = 0.0;
    double width = 0.0;
    cairo_t* cr;
    VApp* da = (VApp*)data;

    static double t = 0;
    static double si;
    g_mutex_lock(&mutex_drawArea2);
    cr = cairo_create(surface2);
    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***

    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 20.0, 7.0);
    cairo_line_to(cr, 35.0, 7.0);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.5);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 35.0, 7.0);
    cairo_line_to(cr, 50.0, 7.0);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, 0.5, 1.0, 0.0, 0.6);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 20.0, 15.0);
    cairo_line_to(cr, 35.0, 15.0);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.3);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, 35.0, 15.0);
    cairo_line_to(cr, 50.0, 15.0);
    cairo_stroke(cr);

    cairo_set_font_size(cr, 13);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "FFT");
    cairo_set_font_size(cr, 8);
    cairo_set_font_size(cr, 8.0);
    cairo_move_to(cr, 52.0, 10.0);
    cairo_show_text(cr, "Row");
    cairo_move_to(cr, 52.0, 19.0);
    cairo_show_text(cr, "Procd");
    if (da->draw2[0].log) {

        cairo_move_to(cr, ((log(608) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "7k");
        cairo_move_to(cr, ((log(304) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "3.5k");
        cairo_move_to(cr, ((log(152) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "1.7k");
        cairo_move_to(cr, ((log(76) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "880");
        cairo_move_to(cr, ((log(38) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "440");
        cairo_move_to(cr, ((log(18.5) - 0.693) * ((double)da->drawstatus2.Width / 6.5)),
                      (double)da->drawstatus2.Height);
        cairo_show_text(cr, "220");
        cairo_move_to(cr, ((log(9.3) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "110");
        cairo_move_to(cr, ((log(4.6) - 0.693) * ((double)da->drawstatus2.Width / 6.5)), (double)da->drawstatus2.Height);
        cairo_show_text(cr, "55");
        cairo_move_to(cr, 5.0, (double)da->drawstatus2.Height);
        cairo_show_text(cr, "20");
    } else {
        cairo_move_to(cr, 1024.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width),
                      (double)da->drawstatus2.Height);
        cairo_show_text(cr, "24k");
        cairo_move_to(cr, 512.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width) + 5.0,
                      (double)da->drawstatus2.Height);
        cairo_show_text(cr, "12k");
        cairo_move_to(cr, 256.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width) + 5,
                      (double)da->drawstatus2.Height);
        cairo_show_text(cr, "6k");
        cairo_move_to(cr, 128.0 * (double)da->drawstatus2.Width / (double)(da->draw2[0].Width),
                      (double)da->drawstatus2.Height);
        cairo_show_text(cr, "3k");
    }

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
                     (*(da->draw2[n].y + i) * (double)(da->drawstatus2.Height) / da->draw2[n].Height);
                if (da->draw2[n].log)
                    t = (log((double)i) - 0.693) * ((double)da->drawstatus2.Width / 6.5);
                else
                    t = (double)i * (double)da->drawstatus2.Width / (double)(da->draw2[n].Width);

                if (si < 10.0)
                    si = 10.0;
                else if (si > (double)da->drawstatus2.Height)
                    si = (double)da->drawstatus2.Height;
                if (t < 0.0) t = 0.0;
                if (t > (double)da->drawstatus2.Width - 6.0) t = (double)da->drawstatus2.Width - 6.0;

                if (da->draw2[n].bar) {
                    cairo_move_to(cr, (t + 5.0), (double)da->drawstatus2.Height - 10.0);
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
    da->flag.drawArea2 = 0; // DrawArea closing notification for end of thread sequence
    g_mutex_unlock(&mutex_drawArea2);
    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw2);

    return FALSE;
}

// 3***************************************************************3
gboolean update_drawArea3(gpointer data) {
    int i;
    int n;
    int tmp;
    int hz;
    int v;
    double sitmp = 0.0;
    double ttmp = 0.0;
    double height = 0.0;
    double width = 0.0;
    static int startPos = 0;
    static int endPos = 4096;
    static char startW[10] = "\0";
    static char endW[10] = "\0";
    char between[10] = "\0";
    char fq[20] = "\0";
    cairo_t* cr;
    VApp* da = (VApp*)data;

    static double t = 0;
    static double si;

    g_mutex_lock(&mutex_drawArea1);

    if (da->flag.drawResize) {
        da->flag.drawResize = 0;
        if (da->flag.drawReSlider) {
            da->flag.drawReSlider = 0;
            if (startPos + (int)((double)da->settings.frames / da->scale.slider4) < da->settings.frames) {
                endPos = startPos + (int)((double)da->settings.frames / da->scale.slider4);
            } else {
                startPos = da->settings.frames - (int)((double)da->settings.frames / da->scale.slider4);
                endPos = da->settings.frames - 1;
            }

        } else {
            startPos += (int)*(da->drawstatus1.x) / 8;
            if (startPos < 0)
                startPos = 0;
            else if (startPos > da->settings.frames - (int)((double)da->settings.frames / da->scale.slider4)) {
                startPos = da->settings.frames - (int)((double)da->settings.frames / da->scale.slider4);
            }
            endPos = startPos + (int)((double)da->settings.frames / da->scale.slider4);
        }

        sprintf(startW, "%d", startPos);
        sprintf(endW, "%d", endPos);

        da->drawStartPos = startPos;
        da->drawEndPos = endPos;
    }
    cr = cairo_create(surface1);

    tmp = da->selPointE.x - da->selPointS.x;
    if (tmp < 1) tmp = 1;
    hz = da->settings.rate / tmp;
    sprintf(fq, "%dpt-%dhz", tmp, hz);

    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0.3);
    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 13.0);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "WAVE");
    cairo_move_to(cr, 5.0, (double)da->drawstatus1.Height - 10.0);
    cairo_show_text(cr, startW);
    cairo_move_to(cr, (double)da->drawstatus1.Width - 40.0, (double)da->drawstatus1.Height - 10.0);
    cairo_show_text(cr, endW);

    cairo_move_to(cr, (double)(da->drawstatus1.Width / 2) - 30.0, (double)da->drawstatus1.Height - 10.0);
    cairo_show_text(cr, fq);
    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.4);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, 3.0, (double)(da->drawstatus1.Height / 2));
    cairo_line_to(cr, (double)da->drawstatus1.Width - 3.0, (double)(da->drawstatus1.Height / 2));
    cairo_stroke(cr);

    height = (double)da->drawstatus1.Height * da->scale.slider3;
    width = (double)da->drawstatus1.Width * da->scale.slider4;
    for (n = 0; n < 5; n++) {
        switch (n) {
        case 0:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
            cairo_set_line_width(cr, 2.0);
            break;
        case 1:
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.7);
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

                si = (double)(da->drawstatus1.Height / 2) -
                     (*(da->draw1[n].y + i) * height / (double)da->draw1[n].Height);
                t = (double)(i - startPos) * width / (double)da->draw1[n].Width;

                if (si < 3.0)
                    si = 3.0;
                else if (si > (double)da->drawstatus1.Height - 3.0)
                    si = (double)da->drawstatus1.Height - 3.0;
                if (t < 0.0) t = 0.0;
                if (t > (double)da->drawstatus1.Width) t = (double)da->drawstatus1.Width - 1.0;

                cairo_move_to(cr, ttmp, sitmp);
                cairo_line_to(cr, t, si);
                sitmp = si;
                ttmp = t;
            }
            cairo_stroke(cr);
        }
    }
    if (da->crossPoint.on) {
        cairo_set_source_rgba(cr, 0.505083, 0.114978, 0.663333, 1.0);
        for (i = 0; i < da->crossPoint.Width; i++) {
            t = (double)(*(da->crossPoint.pos + i) - startPos) * width / (double)da->settings.frames;
            cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 3.0, 0.0, 6.28318);
            cairo_fill(cr);
        }
    }
    //Previous select start point 
    if (da->selPointS.y > 0) {
        cairo_set_source_rgba(cr, 0.035933, 0.980000, 0.276702, 1.0);
        t = (double)(da->selPointS.y - startPos) * width / (double)da->settings.frames;
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 3.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    //Start point
    if (da->selPointS.x > 0) {
        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1.0);
        t = (double)(da->selPointS.x - startPos) * width / (double)(da->settings.frames);
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 5.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    //End point
    if (da->selPointE.x > 0) {
        cairo_set_source_rgba(cr, 0.0, 0.8, 0.3, 1.0);
        t = (double)(da->selPointE.x - startPos) * width / (double)(da->settings.frames);
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 5.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    //Next points
    if (da->nextPoint.x > 0) {
        cairo_set_source_rgba(cr, 0.0, 0.1, 0.1, 1.0);
        t = (double)(da->nextPoint.x - startPos) * width / (double)(da->settings.frames);
        cairo_move_to(cr, t, (double)(da->drawstatus1.Height / 2) + 40.0);
        cairo_line_to(cr, t, (double)(da->drawstatus1.Height / 2) + 3.0);
        cairo_stroke(cr);
    }
    if (da->nextPoint.y > 0) {
        cairo_set_source_rgba(cr, 0.0, 0.1, 0.9, 1.0);
        t = (double)(da->nextPoint.y - startPos) * width / (double)(da->settings.frames);
        cairo_move_to(cr, t, (double)(da->drawstatus1.Height / 2) + 40.0);
        cairo_line_to(cr, t, (double)(da->drawstatus1.Height / 2) + 3.0);
        cairo_stroke(cr);
    }

    da->flag.drawArea1 = 0; // DrawArea closing notification for end of thread sequence
    g_mutex_unlock(&mutex_drawArea1);

    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw1);

    return FALSE;
}

// 4***************************************************************4

gboolean update_drawArea4(gpointer data) {
    VApp* da = (VApp*)data;
    int i;
    int n;
    int tmp;
    int v;
    double sitmp = 0.0;
    double ttmp = 0.0;
    double height = 0.0;
    double width = 0.0;

    static char startW[10] = "\0";
    static char endW[10] = "\0";
    cairo_t* cr;
    static double t = 0;
    static double si;

    g_mutex_lock(&mutex_drawArea2);

    int startPos = da->drawStartPos;
    int endPos = da->drawEndPos;

    sprintf(startW, "%d", startPos);
    sprintf(endW, "%d", endPos);

    cr = cairo_create(surface2);
    // Clear surface
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Index part***
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0.3);
    cairo_select_font_face(cr, "Comic Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 13.0);
    cairo_move_to(cr, 20.0, 30.0);
    cairo_show_text(cr, "WAVE");
    cairo_move_to(cr, 5, da->drawstatus2.Height - 10);
    cairo_show_text(cr, startW);
    cairo_move_to(cr, da->drawstatus2.Width - 40, da->drawstatus2.Height - 10);
    cairo_show_text(cr, endW);
    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.8);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, 3.0, (double)(da->drawstatus2.Height / 2));
    cairo_line_to(cr, (double)da->drawstatus2.Width - 3.0, (double)(da->drawstatus2.Height / 2));

    // DrawArea1 Scope line
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, (double)(startPos * da->drawstatus2.Width / da->settings.frames), 5.0);
    cairo_line_to(cr, (double)(startPos * da->drawstatus2.Width / da->settings.frames),
                  (double)da->drawstatus2.Height - 5.0);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, (double)(endPos * da->drawstatus2.Width / da->settings.frames), 5.0);
    cairo_line_to(cr, (double)(endPos * da->drawstatus2.Width / da->settings.frames),
                  (double)da->drawstatus2.Height - 5.0);
    cairo_stroke(cr);

    height = (double)da->drawstatus2.Height * da->scale.slider3;
    width = (double)da->drawstatus2.Width;
    for (n = 0; n < 5; n++) {
        switch (n) {
        case 0:
            cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.3);
            cairo_set_line_width(cr, 2.0);
            break;
        case 1:
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.7);
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
        if (da->draw2[n].on) {
            for (i = 0; i < da->draw2[n].Width; i++) {

                si = (double)(da->drawstatus2.Height / 2) -
                     (*(da->draw2[n].y + i) * height / (double)da->draw2[n].Height);
                t = (double)i * width / (double)(da->draw2[n].Width);

                if (si < 3.0)
                    si = 3.0;
                else if (si > (double)da->drawstatus2.Height - 3.0)
                    si = (double)da->drawstatus2.Height - 3.0;
                if (t < 0.0) t = 0.0;
                if (t > (double)da->drawstatus2.Width) t = (double)da->drawstatus2.Width - 1.0;

                cairo_move_to(cr, ttmp, sitmp);
                cairo_line_to(cr, t, si);
                sitmp = si;
                ttmp = t;
            }
            cairo_stroke(cr);
        }
    }
    if (da->crossPoint.on) {
        cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.3);
        for (i = 0; i < da->crossPoint.Width; i++) {
            t = (double)*(da->crossPoint.pos + i) * width / (double)da->settings.frames;
            cairo_arc(cr, t, (double)(da->drawstatus2.Height / 2), 3.0, 0.0, 6.28318);
            cairo_fill(cr);
        }
    }
    if (da->selPointS.y > 0) {
        cairo_set_source_rgba(cr, 0.3, 1.0, 0.0, 1.0);
        t = (double)da->selPointS.y * width / (double)da->settings.frames;
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 3.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    if (da->selPointS.x > 0) {
        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1.0);
        t = (double)da->selPointS.x * width / (double)da->settings.frames;
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 5.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    if (da->selPointE.x > 0) {
        cairo_set_source_rgba(cr, 0.0, 0.8, 0.3, 1.0);
        t = (double)da->selPointE.x * width / (double)da->settings.frames;
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 5.0, 0.0, 6.28318);
        cairo_fill(cr);
    }
    if (da->nextPoint.x > 0) {
        cairo_set_source_rgba(cr, 0.3, 0.8, 0.8, 1.0);
        t = (double)da->nextPoint.x * width / (double)da->settings.frames;
        cairo_arc(cr, t, (double)(da->drawstatus1.Height / 2), 5.0, 0.0, 6.28318);
        cairo_fill(cr);
    }

    da->flag.drawArea2 = 0; // DrawArea closing notification for end of thread sequence
    g_mutex_unlock(&mutex_drawArea2);

    cairo_destroy(cr);
    gtk_widget_queue_draw(da->priv->draw2);

    return FALSE;
}
