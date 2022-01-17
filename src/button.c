#include "header.h"

// Left column Buttons ********
void selButton1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (!da->status.open) {
        da->status.selNum = 1;
        da->status.open = 1;
        if (gSet.readfile == NULL)
            da->flag.soundMic = 1;
        else
            da->flag.soundFile = 1;
        soundThread(widget, data);
    } else {
        strcpy(da->statusBuf, "Please push stop button Button");
        statusprint(data);
    }
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
            snprintf(gSet.readfilename, 255, "%s", filename);
            gSet.readfile = g_file_new_for_path(filename);

            g_free(filename);
            soundThread(widget, data);
        } else {
            da->flag.soundFile = 0;
            da->status.open = 0;
            da->status.selNum = 0;
            printf("File cancel\n");
        }

        gtk_widget_destroy(dialog);
    } else {
        strcpy(da->statusBuf, "Please push stop button Button");
        statusprint(data);
    }
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
            snprintf(gSet.readfilename, 255, "%s", filename);
            gSet.readfile = g_file_new_for_path(filename);

            g_free(filename);
            mlDataThread(widget, data);
        } else {
            da->flag.soundFile = 0;
            da->status.open = 0;
            da->status.selNum = 0;
            printf("File cancel\n");
        }

        gtk_widget_destroy(dialog);
    } else {
        strcpy(da->statusBuf, "Please push stop button Button");
        statusprint(data);
    }
}

void selButton4(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    printf("SelButton4\n");
    getWeight(widget, data);
}

void selButton5(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    int res = 0;

    if(da->W[0].weight == NULL){
        printf("Error in mlTest -> Weight was not found\n");
        strcpy(da->statusBuf, "Weight and bias was not found");
        statusprint(data);
        return;
    }
    if (!da->status.open) {
        da->status.selNum = 5;
        da->status.open = 1;
        da->flag.soundFile = 1;

        dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"),
                                             GTK_RESPONSE_ACCEPT, NULL);
        res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT) {
            gchar* filename;

            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            snprintf(gSet.readfilename, 255, "%s", filename);
            gSet.readfile = g_file_new_for_path(filename);

            g_free(filename);
            mlTestThread(widget, data);

        } else {
            da->flag.soundFile = 0;
            da->status.open = 0;
            da->status.selNum = 0;
            printf("File cancel\n");
        }

        gtk_widget_destroy(dialog);
    } else {
        strcpy(da->statusBuf, "Please push stop button Button");
        statusprint(data);
    }
}

void stpButton(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    da->status.open = 0;
}
//TMP Button
void sLowerButton0(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    testnn(widget, data);
    
}

void sLowerButton1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    static int n = 0;

    if (n == 0) {
        n = 1;
        strcpy(gSet.deviceName, "hw:CARD=PCH,DEV=0");
        gtk_button_set_label(GTK_BUTTON(widget), "ALSA");
        strcpy(da->statusBuf, "ALSA");
        statusprint(data);
    } else if (n == 1) {
        n = 0;
        strcpy(gSet.deviceName, "default");
        gtk_button_set_label(GTK_BUTTON(widget), "Pulse");
        strcpy(da->statusBuf, "Pulse");
        statusprint(data);
    } else {
        gSet.deviceName[0] = '\0';
        printf("b1 -> Device name is NULL\n");
        strcpy(da->statusBuf, "Device name is NULL");
        statusprint(data);
    }
}
//ML DATA Write
void sLowerButton2(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 3) {
        if (da->flag.writeFile == 0) {
            if (da->mlFlag.on) da->flag.writeFile = 1;
        }
    }
}

// Right column Buttons ********************************************************************************:

void b1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.filter == 0) {
        da->status.filter = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR H");
    } else if (da->status.filter == 1) {
        da->status.filter = 2;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR -");
    } else if (da->status.filter == 2) {
        da->status.filter = 0;
        gtk_button_set_label(GTK_BUTTON(widget), "IIR L");
    } else {
        printf("b1 Error\n");
        strcpy(da->statusBuf, "Switch Lo Hi Error");
        statusprint(data);
    }
}

void b2(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->draw1[0].on == 0) {
            da->draw1[0].on = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Row ON");
        } else if (da->draw1[0].on == 1) {
            da->draw1[0].on = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Row");
        } else {
            printf("b2 Error draw1 - 0.on\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->mlFlag.SRight == 0) {
            da->mlFlag.SRight = 1;
        } else {
            printf("Error b2 Sright\n");
        }
    }
}

void b3(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->draw1[1].on == 0) {
            da->draw1[1].on = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Procd ON");
        } else if (da->draw1[1].on == 1) {
            da->draw1[1].on = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Procd");
        } else {
            printf("b3 Error draw1-1 on\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->mlFlag.SLeft == 0) {
            da->mlFlag.SLeft = 1;
        } else {
            printf("Error b3 Sleft\n");
        }
    }
}

void b4(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->flag.octaveHi == 0 && !da->flag.octaveLo == 1) {
            da->flag.octaveHi = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Hi ON");

        } else if (da->flag.octaveHi == 1) {
            da->flag.octaveHi = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Oct Hi");
        } else {
            printf("b4 Error octave\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->mlFlag.ERight == 0) {
            da->mlFlag.ERight = 1;
        } else {
            printf("Error b4 Eright\n");
        }
    }
}

void b5(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->flag.octaveLo == 0 && !da->flag.octaveHi == 1) {
            da->flag.octaveLo = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Lo ON");
        } else if (da->flag.octaveLo == 1) {
            da->flag.octaveLo = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Oct Lo");
        } else {
            printf("b5 Error octave\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->mlFlag.ELeft == 0) {
            da->mlFlag.ELeft = 1;
        } else {
            printf("Error b5 Eleft\n");
        }
    }
}

void b6(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->flag.decomp == 0) {
            da->flag.decomp = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Dcp ON");
        } else if (da->flag.decomp == 1) {
            da->flag.decomp = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Decomp");
        } else {
            printf("b6 Error Decomp\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->mlFlag.enter0 == 0) {
            da->mlFlag.enter0 = 1;
        } else {
            printf("Error b6\n");
        }
    }
}

void b7(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 3) {
        if (da->mlFlag.enter1 == 0) {
            da->mlFlag.enter1 = 1;
        } else {
            printf("Error b7\n");
        }
    }
}

void b8(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 3) {
        if (da->mlFlag.on == 0) {
            da->mlFlag.on = 1;
            gtk_button_set_label(GTK_BUTTON(widget), "Auto +");

        } else if (da->mlFlag.on == 1) {
            da->mlFlag.on = 0;
            gtk_button_set_label(GTK_BUTTON(widget), "Auto -");
        } else {
            printf("Error b8\n");
        }
    }
}

void b9(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.open) {
        *(da->drawstatus1.x) = 4000.0;
        da->flag.drawResize = 1;
    }
}

void b10(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.open) {
        *(da->drawstatus1.x) = -4000.0;
        da->flag.drawResize = 1;
    }
}

void b11(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    static int i = 0;
    int n;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
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
        }
    } else if (da->status.selNum == 3) {
        da->mlFlag.dele1 = 1;
    }
}

void b12(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->draw2[0].on == 0) {
            da->draw2[0].on = 1;
        } else if (da->draw2[0].on == 1) {
            da->draw2[0].on = 0;
        } else {
            printf("b12 Error\n");
        }
    } else if (da->status.selNum == 3) {
        da->mlFlag.right1 = 1;
    }
}

void b13(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->draw2[1].on == 0) {
            da->draw2[1].on = 1;
        } else if (da->draw2[1].on == 1) {
            da->draw2[1].on = 0;
        } else {
            printf("b13 Error\n");
        }
    } else if (da->status.selNum == 3) {
        da->mlFlag.left1 = 1;
    }
}

void b14(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 3) {
        da->mlFlag.deleAll = 1;
    }
}

void b15(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
}

void b16(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
}

void b17(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
}

void b18(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
}

void b19(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->flag.nextWave == 0) {
        da->flag.nextWave = 1;
    } else if (da->flag.nextWave == 1) {
        da->flag.nextWave = 0;
    } else {
        printf("b18 Error\n");
    }
}

void b20(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
    if (da->status.selNum == 1 || da->status.selNum == 2) {
        if (da->flag.pause == 0) {
            da->flag.pause = 1;
        } else if (da->flag.pause == 1) {
            da->flag.pause = 0;
        } else {
            printf("b20 Error\n");
        }
    } else if (da->status.selNum == 3) {
        if (da->flag.prevWave == 0) {
            da->flag.prevWave = 1;
        } else if (da->flag.prevWave == 1) {
            da->flag.prevWave = 0;
        } else {
            printf("b20 Error\n");
        }
    } else {
        printf("Error b20 selNum\n");
    }
}