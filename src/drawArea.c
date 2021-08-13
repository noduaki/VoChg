#include "drawArea.h"

cairo_surface_t *surface1 = NULL;
cairo_surface_t *surface2 = NULL;

static void clear_surface (int i){
    cairo_t *cr;
    if(i == 1){
        cr = cairo_create (surface1);
        cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.1);
    }else{ 
        cr = cairo_create (surface2); 
        cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.8);
    }
    cairo_paint (cr);
    cairo_destroy (cr);
}

gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer  data){
    int i;
    VApp* da = (VApp*)data;
    da->draw2.drawAreaWidth = gtk_widget_get_allocated_width (da->priv->draw2);
    da->draw2.drawAreaHeight = gtk_widget_get_allocated_height (da->priv->draw2);
    da->draw1.drawAreaWidth = gtk_widget_get_allocated_width (da->priv->draw1);
    da->draw1.drawAreaHeight = gtk_widget_get_allocated_height (da->priv->draw1);
    
    if (widget == da->priv->draw1){
        i = 1;
        cairo_surface_destroy (surface1);
        surface1 = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                CAIRO_CONTENT_COLOR_ALPHA, gtk_widget_get_allocated_width (widget), 
                                            gtk_widget_get_allocated_height (widget));
                                                
                                                
    }else{
        i = 2;
        cairo_surface_destroy(surface2); 
        surface2 = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                CAIRO_CONTENT_COLOR_ALPHA, gtk_widget_get_allocated_width (widget), 
                                            gtk_widget_get_allocated_height (widget));
    }
    
    /* Initialize the surface to white */
    clear_surface (i);
    
    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}
gboolean draw_cb (GtkWidget *widget, cairo_t   *cr, gpointer data){
    VApp* da = (VApp*)data;
    
    if(widget == da->priv->draw1)
        cairo_set_source_surface (cr, surface1, 0, 0); 
    else   cairo_set_source_surface (cr, surface2, 0, 0);    

    cairo_paint (cr);

    return FALSE;
    }

gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data){ 
    int a = 0;
    VApp* da = (VApp*)data;

    if(widget == da->priv->draw1){
        a = 1;
        if (surface1 == NULL)
        return FALSE;
    
    }else{
        a = 2;
        if (surface2 == NULL)
        return FALSE;
    }
    if(a == 1){
        if (event->button == GDK_BUTTON_PRIMARY){ 
            statusprint("Text Draw1 Press", da);
        }else if (event->button == GDK_BUTTON_SECONDARY){
           
          
        }
    }
    if(a == 2){
        if (event->button == GDK_BUTTON_PRIMARY){   
            
        }else if (event->button == GDK_BUTTON_SECONDARY){

        }
    }
  /* We've handled the event, stop processing */
    return TRUE;
}

gboolean button_release_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data){
    int a = 0;
    VApp* da = (VApp*)data;
    /* paranoia check, in case we haven't gotten a configure event */
    if(widget == da->priv->draw1){
        a = 1;
        if (surface1 == NULL)
        return FALSE;
    }else{
        a = 2;
        if (surface2 == NULL)
        return FALSE;
    }
    if(a == 2){
        if(event->button == GDK_BUTTON_PRIMARY){
            statusprint("Text Draw Release", da);
        }else if (event->button == GDK_BUTTON_SECONDARY){
            
        }
    }

    /* We've handled the event, stop processing */
    return TRUE;
}

gboolean motion_notify_event_cb (GtkWidget *widget, GdkEventMotion *event, gpointer data){
  
    VApp* da = (VApp*)data;
    static double start = 0.0;
    static double end   = 0.0;
    int a = 0;
    int x;
    double xx;
    double y;
    
    //paranoia check, in case we haven't gotten a configure event 
    if(widget == da->priv->draw1){
        a = 1;
        if (surface1 == NULL)
        return FALSE;
    }else{
        a = 2;
        if (surface2 == NULL)
        return FALSE;
    }
    return TRUE;
}
