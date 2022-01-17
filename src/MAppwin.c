#include "header.h"

struct _MAppWindow {
    GtkApplicationWindow parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(MAppWindow, M_app_window, GTK_TYPE_APPLICATION_WINDOW);
// Non Thread func. ************************************************************************

// Thread launcher *************************************************************************

void soundThread(GtkWidget* window, gpointer data) {
    VApp* da = (VApp*)data;
    GTask* soundTask;
    int err = 0;

    if ((da->status.selNum == 1 || da->status.selNum == 2) && da->status.open == 1) {
        err = initVar(da);
        if (err) {
            delVar(da);
            printf("Error in soundThread() -> malloc\n");
            strcat(da->statusBuf, " Error");
            statusprint(data);
            return;
        }

        da->status.ref++;
        soundTask = g_task_new(NULL, NULL, NULL, NULL);
        g_task_set_task_data(soundTask, data, NULL);
        g_task_run_in_thread(soundTask, initSound);

    } else {
        printf("soundThread() selNum err status.open = %d, selnum = %d \n", da->status.open, da->status.selNum);
        strcat(da->statusBuf, "Please One more time");
        statusprint(data);
        return;
    }
}

void mlDataThread(GtkWidget* window, gpointer data) {
    VApp* da = (VApp*)data;
    GTask* mlDataTask;
    int err = 0;

    if (da->status.selNum == 3 && da->status.open == 1) {
        err = initVar(da);
        if (err) {
            delVar(da);
            printf("Error in mlDataThread() -> malloc");
            strcat(da->statusBuf, "Error");
            statusprint(data);
            return;
        }

        da->status.ref++;
        mlDataTask = g_task_new(NULL, NULL, NULL, NULL);
        g_task_set_task_data(mlDataTask, data, NULL);
        g_task_run_in_thread(mlDataTask, mlDataProcess);

    } else {
        printf("mlDataThread() selNum err\n");
        strcat(da->statusBuf, "Please One more time");
        statusprint(data);
        return;
    }
}

void mlTestThread(GtkWidget* window, gpointer data){
    VApp* da = (VApp*)data;
    GTask* mlTestTask;
    int err = 0;

    

    if (da->status.selNum == 5 && da->status.open == 1) {
        err = initVar(da);
        if (err) {
            delVar(da);
            printf("Error in mlTest() -> malloc");
            strcat(da->statusBuf, "Error");
            statusprint(data);
            return;
        }
        da->status.ref++;
        mlTestTask = g_task_new(NULL, NULL, NULL, NULL);
        g_task_set_task_data(mlTestTask, data, NULL);
        g_task_run_in_thread(mlTestTask, mlTest);
    } else {
        printf("mlTest() selNum err\n");
        strcat(da->statusBuf, "Please One more time");
        statusprint(data);
        return;
    }
}

// Init Windows *******************************************************************

static void M_app_window_init(MAppWindow* win) {
    int i;
    static VApp vApp;

    guint statusBar;
    GtkBuilder* builder;
    GMenuModel* menu;

    vApp.priv = M_app_window_get_instance_private(win);
    vApp.status.open = 0;
    vApp.status.selNum = 0;
    vApp.status.ref = 0;

    vApp.flag.soundFile = 0;
    vApp.flag.soundMic = 0;
    vApp.flag.drawArea1 = 0;
    vApp.flag.drawArea2 = 0;
    vApp.flag.drawResize = 0;
    vApp.flag.pause = 0;

    for (i = 0; i < 10; i++) {
        vApp.W[i].weight = NULL;
        vApp.W[i].bias = NULL;
    }

    vApp.statusBuf = (char*)malloc(200);
    *(vApp.statusBuf) = '\0';
    if (vApp.statusBuf == NULL) exit(1);

    gtk_widget_init_template(GTK_WIDGET(win));

    statusBar = gtk_statusbar_get_context_id(GTK_STATUSBAR(vApp.priv->entrytext), "settings");
    gtk_statusbar_push(GTK_STATUSBAR(vApp.priv->entrytext), 1, "Start");

    // How to add widget
    // Go to window.ui -> write widget xml with id-name then go to Terminal
    // Command "glib-compile-resources MLData.gresource.xml --target=resources.c --generate-source"
    // Go to MAppwin.h -> add widget id-name at struct->MAppWindowPrivate
    // Add widget id-name in M_app_window_class_init() it's next next func.
    // Add signal_connect in below
    // Write callback func.

    // Buttons**************************
    g_signal_connect(vApp.priv->selectButton1, "clicked", G_CALLBACK(selButton1), &vApp);
    g_signal_connect(vApp.priv->selectButton2, "clicked", G_CALLBACK(selButton2), &vApp);
    g_signal_connect(vApp.priv->selectButton3, "clicked", G_CALLBACK(selButton3), &vApp);
    g_signal_connect(vApp.priv->selectButton4, "clicked", G_CALLBACK(selButton4), &vApp);
    g_signal_connect(vApp.priv->selectButton5, "clicked", G_CALLBACK(selButton5), &vApp);
    g_signal_connect(vApp.priv->stopButton, "clicked", G_CALLBACK(stpButton), &vApp);
    g_signal_connect(vApp.priv->selLowerButton0, "clicked", G_CALLBACK(sLowerButton0), &vApp);
    g_signal_connect(vApp.priv->selLowerButton1, "clicked", G_CALLBACK(sLowerButton1), &vApp);
    g_signal_connect(vApp.priv->selLowerButton2, "clicked", G_CALLBACK(sLowerButton2), &vApp);
    g_signal_connect(vApp.priv->button1, "clicked", G_CALLBACK(b1), &vApp);
    g_signal_connect(vApp.priv->button2, "clicked", G_CALLBACK(b2), &vApp);
    g_signal_connect(vApp.priv->button3, "clicked", G_CALLBACK(b3), &vApp);
    g_signal_connect(vApp.priv->button4, "clicked", G_CALLBACK(b4), &vApp);
    g_signal_connect(vApp.priv->button5, "clicked", G_CALLBACK(b5), &vApp);
    g_signal_connect(vApp.priv->button6, "clicked", G_CALLBACK(b6), &vApp);
    g_signal_connect(vApp.priv->button7, "clicked", G_CALLBACK(b7), &vApp);
    g_signal_connect(vApp.priv->button8, "clicked", G_CALLBACK(b8), &vApp);
    g_signal_connect(vApp.priv->button9, "clicked", G_CALLBACK(b9), &vApp);
    g_signal_connect(vApp.priv->button10, "clicked", G_CALLBACK(b10), &vApp);
    g_signal_connect(vApp.priv->button11, "clicked", G_CALLBACK(b11), &vApp);
    g_signal_connect(vApp.priv->button12, "clicked", G_CALLBACK(b12), &vApp);
    g_signal_connect(vApp.priv->button13, "clicked", G_CALLBACK(b13), &vApp);
    g_signal_connect(vApp.priv->button14, "clicked", G_CALLBACK(b14), &vApp);
    g_signal_connect(vApp.priv->button15, "clicked", G_CALLBACK(b15), &vApp);
    g_signal_connect(vApp.priv->button16, "clicked", G_CALLBACK(b16), &vApp);
    g_signal_connect(vApp.priv->button17, "clicked", G_CALLBACK(b17), &vApp);
    g_signal_connect(vApp.priv->button18, "clicked", G_CALLBACK(b18), &vApp);
    g_signal_connect(vApp.priv->button19, "clicked", G_CALLBACK(b19), &vApp);
    g_signal_connect(vApp.priv->button20, "clicked", G_CALLBACK(b20), &vApp);

    // Scales*****************************
    // Initial value
    vApp.scale.slider1 = 1000.0;
    vApp.scale.slider2 = 1.0;
    vApp.scale.slider3 = 1.0;
    vApp.scale.slider4 = 1.0;
    vApp.scale.slider5 = 2.0;
    vApp.scale.slider6 = 7.0;
    vApp.scale.slider7 = 0.0;
    vApp.scale.slider8 = 0.0;
    vApp.scale.slider9 = 0.0;
    vApp.scale.slider10 = 0.0;
    vApp.scale.slider11 = 0.0;
    vApp.scale.slider12 = 0.0;
    vApp.scale.slider13 = 0.0;
    vApp.scale.slider14 = 0.0;
    vApp.scale.slider15 = 0.0;

    // Slider set value
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale1), 1000.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale2), 1.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale3), 1.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale4), 1.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale5), 2.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale6), 7.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale7), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale8), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale9), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale10), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale11), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale12), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale13), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale14), 0.0);
    gtk_range_set_value(GTK_RANGE(vApp.priv->scale15), 0.0);
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale1), 200.0, GTK_POS_TOP, "IIR cutoff");
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale2), 0.01, GTK_POS_TOP, "IIR Q");
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale3), 1.0, GTK_POS_TOP, "Height");
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale4), 1.0, GTK_POS_TOP, "Width");
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale5), 0.0, GTK_POS_TOP, "D-Gain");
    gtk_scale_add_mark(GTK_SCALE(vApp.priv->scale6), 0.0, GTK_POS_TOP, "D-OutV");
    g_signal_connect(vApp.priv->scale1, "value-changed", G_CALLBACK(fscale1), &vApp);
    g_signal_connect(vApp.priv->scale2, "value-changed", G_CALLBACK(fscale2), &vApp);
    g_signal_connect(vApp.priv->scale3, "value-changed", G_CALLBACK(fscale3), &vApp);
    g_signal_connect(vApp.priv->scale4, "value-changed", G_CALLBACK(fscale4), &vApp);
    g_signal_connect(vApp.priv->scale5, "value-changed", G_CALLBACK(fscale5), &vApp);
    g_signal_connect(vApp.priv->scale6, "value-changed", G_CALLBACK(fscale6), &vApp);
    g_signal_connect(vApp.priv->scale7, "value-changed", G_CALLBACK(fscale7), &vApp);
    g_signal_connect(vApp.priv->scale8, "value-changed", G_CALLBACK(fscale8), &vApp);
    g_signal_connect(vApp.priv->scale9, "value-changed", G_CALLBACK(fscale9), &vApp);
    g_signal_connect(vApp.priv->scale10, "value-changed", G_CALLBACK(fscale10), &vApp);
    g_signal_connect(vApp.priv->scale11, "value-changed", G_CALLBACK(fscale11), &vApp);
    g_signal_connect(vApp.priv->scale12, "value-changed", G_CALLBACK(fscale12), &vApp);
    g_signal_connect(vApp.priv->scale13, "value-changed", G_CALLBACK(fscale13), &vApp);
    g_signal_connect(vApp.priv->scale14, "value-changed", G_CALLBACK(fscale14), &vApp);
    g_signal_connect(vApp.priv->scale15, "value-changed", G_CALLBACK(fscale15), &vApp);

    g_signal_connect(vApp.priv->entry1, "activate", G_CALLBACK(fentry1), &vApp);

    // Draw area1************

    g_signal_connect(vApp.priv->draw1, "draw", G_CALLBACK(draw_cb), &vApp);
    g_signal_connect(vApp.priv->draw1, "configure-event", G_CALLBACK(configure_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw1, "motion-notify-event", G_CALLBACK(motion_notify_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw1, "button-press-event", G_CALLBACK(button_press_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw1, "button-release-event", G_CALLBACK(button_release_event_cb), &vApp);

    gtk_widget_set_events(vApp.priv->draw1, gtk_widget_get_events(vApp.priv->draw1) | GDK_BUTTON_PRESS_MASK |
                                                GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    // Drawarea2*************

    g_signal_connect(vApp.priv->draw2, "draw", G_CALLBACK(draw_cb), &vApp);
    g_signal_connect(vApp.priv->draw2, "configure-event", G_CALLBACK(configure_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw2, "motion-notify-event", G_CALLBACK(motion_notify_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw2, "button-press-event", G_CALLBACK(button_press_event_cb), &vApp);
    g_signal_connect(vApp.priv->draw2, "button-release-event", G_CALLBACK(button_release_event_cb), &vApp);
    gtk_widget_set_events(vApp.priv->draw2, gtk_widget_get_events(vApp.priv->draw2) | GDK_BUTTON_PRESS_MASK |
                                                GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    // How to use signal-emit-Func->
    // g_signal_emit_by_name(GTK_WIDGET(da->priv->), "key_word", 0); <-this need the following code
    // Put here this code
    // g_signal_connect(GTK_WIDGET(vApp.priv->), "key_word", G_CALLBACK( -CallBackFunc- ), &vApp);
    // The follow code is put in classInit()
    // g_signal_new("key_word", G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void M_app_window_dispose(GObject* object) {
    MAppWindow* win;
    MAppWindowPrivate* priv;

    win = M_APP_WINDOW(object);
    priv = M_app_window_get_instance_private(win);

    g_clear_object(&priv->settings);

    G_OBJECT_CLASS(M_app_window_parent_class)->dispose(object);
}

static void M_app_window_class_init(MAppWindowClass* class) {

    G_OBJECT_CLASS(class)->dispose = M_app_window_dispose;
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/MLData/window.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, content_boxIn);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, content_box);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, draw1);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, draw2);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, textview);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selectButton1);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selectButton2);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selectButton3);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selectButton4);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selectButton5);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, stopButton);
     gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selLowerButton0);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selLowerButton1);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, selLowerButton2);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button1);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button2);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button3);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button4);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button5);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button6);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button7);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button8);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button9);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button10);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button11);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button12);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button13);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button14);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button15);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button16);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button17);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button18);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button19);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, button20);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale1);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale2);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale3);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale4);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale5);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale6);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale7);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale8);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale9);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale10);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale11);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale12);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale13);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale14);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, scale15);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, entrytext);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), MAppWindow, entry1);
}

MAppWindow* M_app_window_new(MApp* app) {
    return g_object_new(M_APP_WINDOW_TYPE, "application", app, NULL);
}

void M_app_text_open(MAppWindow* win, GFile* file) {
    MAppWindowPrivate* priv;
    gchar* basename;
    gchar* contents;
    gsize length;
    GtkTextBuffer* buffer;

    priv = M_app_window_get_instance_private(win);
    basename = g_file_get_basename(file);

    if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) {
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->textview));
        gtk_text_buffer_set_text(buffer, contents, length);

        g_free(contents);
    }
    g_free(basename);
}
