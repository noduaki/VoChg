#include "header.h"


void activate_openfile (GSimpleAction *simple, GVariant  *parameter, gpointer app){    
  GList *windows;
  MAppWindow *win;
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;
  
  
  GApplication* a = (GApplication*)app;
  
  windows = gtk_application_get_windows (GTK_APPLICATION (app));//windows get app currently focused window data
  
  if (windows)
      win = M_APP_WINDOW (windows->data);
  else
    win = M_app_window_new (M_APP (app)); 

  dialog = gtk_file_chooser_dialog_new ("Open File", NULL, action, ("_Cancel"), 
                          GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT) {  

      gchar *filename;      
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      snprintf(gSet.filename, 255, "%s", filename);
      gSet.file =  g_file_new_for_path(filename);      

      g_free (filename);
    } 
  
  
  gtk_widget_destroy (dialog); 
  
 
}

void activate_color (GSimpleAction *simple,  GVariant *parameter,  gpointer app){
  GList *windows;
  MAppWindow *win;
  GtkWidget *dialog;
  gint res;
  GdkRGBA *mycolor;
  mycolor = (GdkRGBA*)malloc(sizeof(GdkRGBA));
  windows = gtk_application_get_windows (GTK_APPLICATION (app));//windows get app currently focused window data
  win = M_APP_WINDOW (windows->data);

  dialog = gtk_color_chooser_dialog_new (NULL, NULL);
  
  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_OK)
  {   
    gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (dialog), mycolor);
   
    printf("%f,,%f,,%f,,%f,,\n", mycolor->alpha, mycolor->blue, mycolor->green, mycolor->red);
  }
  printf("%d///\n", res);
  gtk_widget_destroy (dialog); 
}

