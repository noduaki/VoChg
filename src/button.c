#include "header.h"

// Button ********
void selButton1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (!da->status.open) {
        da->status.selNum = 1;
        da->status.open = 1;
        if (gSet.file == NULL)
            da->flag.soundMic = 1;
        else
            da->flag.soundFile = 1;
        soundThread(widget, data);
    } else
        statusprint("Please push stop button Button", data);
}

void selButton2(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    int res = 0;

    if (!da->status.open) {
        da->status.selNum = 2;
        da->status.open = 1;
        da->flag.soundFile = 1;

        dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"),
                                             GTK_RESPONSE_ACCEPT, NULL);
        res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT) {
            gchar* filename;

            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            snprintf(gSet.filename, 255, "%s", filename);
            gSet.file = g_file_new_for_path(filename);

            g_free(filename);
            soundThread(widget, data);
        } else{
            da->flag.soundFile = 0;
            da->status.open    = 0;
            da->status.selNum  = 0;
            printf("File cancel\n");
        }

        gtk_widget_destroy(dialog);
    } else
        statusprint("Please push stop button", data);
}

void selButton3(GtkWidget* widget, gpointer data) {
     VApp* da = (VApp*)data;
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    int res = 0;

    if (!da->status.open) {
        da->status.selNum = 3;
        da->status.open = 1;
        da->flag.soundFile = 1;

        dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"),
                                             GTK_RESPONSE_ACCEPT, NULL);
        res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT) {
            gchar* filename;

            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            snprintf(gSet.filename, 255, "%s", filename);
            gSet.file = g_file_new_for_path(filename);

            g_free(filename);
            mlDataThread(widget, data);
        } else{
            da->flag.soundFile = 0;
            da->status.open    = 0;
            da->status.selNum  = 0;
            printf("File cancel\n");
        }

        gtk_widget_destroy(dialog);
    } else
        statusprint("Please push stop button", data);
}

void stpButton(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    da->status.open = 0;
}

void sLowerButton1(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
    static int n = 0;

    if (n == 0) {
        n = 1;
        strcpy(gSet.deviceName, "hw:CARD=PCH,DEV=0");
        gtk_button_set_label(GTK_BUTTON(widget), "ALSA");
        statusprint("ALSA", data);

    } else if (n == 1) {
        n = 0;
        strcpy(gSet.deviceName, "default");
        gtk_button_set_label(GTK_BUTTON(widget), "Pulse");
        statusprint("Pulse Audio", data);

    } else {
        strcpy(gSet.deviceName, "\0");
        printf("b1 -> Device name is NULL\n");
    }
}

void sLowerButton2(GtkWidget* widget, gpointer data) {
    ;
}

void b1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    
    if ( da->draw1[0].on == 0) {
        da->draw1[0].on = 1;
    } else if ( da->draw1[0].on == 1) {
        da->draw1[0].on = 0;
    } else {
        printf("b1 Error\n");
        statusprint("Cepstrum Error", data);
    }
}

void b2(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
   
    if ( da->draw1[1].on == 0) {
        da->draw1[1].on = 1;
    } else if ( da->draw1[1].on == 1) {
        da->draw1[1].on = 0;
    } else {
        printf("b2 Error\n");
        statusprint("Cepstrum Error", data);
    }
}

void b3(GtkWidget* widget, gpointer data) {
     VApp* da = (VApp*)data;
   
    
    if (da->settings.filter == 0) {
       
        da->settings.filter = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR H");
    } else if (da->settings.filter == 1) {
       
        da->settings.filter = 2;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR -");
    }else if (da->settings.filter == 2) {
        
        da->settings.filter = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR L");
    } else {
        printf("b3 Error\n");
        statusprint("Switch Lo Hi Error", data);
    }
}

void b4(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b5(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b6(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b7(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b8(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b9(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    *(da->drawstatus1.x) = 4000.0;
    da->flag.drawResize = 1;
}

void b10(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    *(da->drawstatus1.x) = -4000.0;
    da->flag.drawResize = 1;
}

void b11(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    static int i = 0;
    int n;

    if (i == 0) {
        i = 1;
        for (n = 0; n < 5; n++) {
            da->draw2[n].log = 0;
        }
        gtk_button_set_label(GTK_BUTTON(widget), "Linear");
    } else if (i == 1) {
        i = 0;
        for (n = 0; n < 5; n++) {
            da->draw2[n].log = 1;
        }
        gtk_button_set_label(GTK_BUTTON(widget), "Log");
    } else {
        printf("b11 Error\n");
        statusprint("Log-Linear Error", data);
    }
}

void b12(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    
    if (da->draw2[0].on == 0) {
        da->draw2[0].on = 1;
    } else if (da->draw2[0].on == 1) {
        da->draw2[0].on = 0;
    } else {
        printf("b12 Error\n");
        statusprint("Spectrum Error", data);
    }
}

void b13(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    
    if (da->draw2[1].on == 0) {
        da->draw2[1].on = 1;
    } else if (da->draw2[1].on == 1) {
        da->draw2[1].on = 0;
    } else {
        printf("b13 Error\n");
        statusprint("Cepstrum Error", data);
    }
}

void b14(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b15(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b16(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b17(GtkWidget* widget, gpointer data) {
    VApp* tmp = (VApp*)data;
}

void b18(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
   
    if(da->flag.nextWave == 0){
        da->flag.nextWave = 1;
    }else if(da->flag.nextWave == 1){
        da->flag.nextWave = 0;
    } else{
        printf("b18 Error\n");
    }
}

void b19(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    
    if(da->flag.prevWave == 0){
        da->flag.prevWave = 1;
    }else if(da->flag.prevWave == 1){
        da->flag.prevWave = 0;
    } else{
        printf("b19 Error\n");
    }
}

void b20(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
   
    if(da->flag.pause == 0){
        da->flag.pause = 1;
    }else if(da->flag.pause == 1){
        da->flag.pause = 0;
    }else{
        printf("b20 Error\n");
    }
}