#include "header.h"

//Button ********
void selButton1(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  if(!da->status.open){
    da->status.selNum = 1;
    da->status.open   = 1;
    if(gSet.file == NULL)
      da->flag.soundMic = 1;
    else da->flag.soundFile = 1;
    soundThread(widget, data);
  }else statusprint("Please push stop button Button", data);

  
  
}

void selButton2(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN; 
  int res = 0;
 
  if(!da->status.open){
    da->status.selNum = 2;
    da->status.open   = 1;
    da->flag.soundFile = 1;
    

    dialog = gtk_file_chooser_dialog_new ("Open File", NULL, action, ("_Cancel"), 
                              GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
    res = gtk_dialog_run (GTK_DIALOG (dialog)); 
    if (res == GTK_RESPONSE_ACCEPT){ 
      gchar *filename; 
 
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      snprintf(gSet.filename, 255, "%s", filename);
      gSet.file =  g_file_new_for_path(filename);      

      g_free (filename);
      soundThread(widget, data);
    }else  da->flag.soundFile = 0;   
    gtk_widget_destroy (dialog);
  }else statusprint("Please push stop button", data);

 
}

void selButton3(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  if(!da->status.open){
    da->status.selNum = 3;
    da->status.open   = 1;
    SB3Thread(widget, data);
  }else statusprint("please push stop button", data);

  
}

void stpButton(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  da->status.open   = 0;
}

void b1(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  statusprint("Text Button", data);
}

void b2(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b3(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b4(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b5(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b6(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b7(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b8(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b9(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b10(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b11(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b12(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b13(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b14(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b15(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b16(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b17(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b18(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b19(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}

void b20(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
}