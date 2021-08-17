
#include "MApp.h"


struct _MApp{
  GtkApplication parent;
};

G_DEFINE_TYPE(MApp, M_app, GTK_TYPE_APPLICATION);

mSettings gSet = {SOUNDBUFFERSIZE  , SOUNDPERIODSIZE , SND_PCM_FORMAT_S16_LE, 
                                    SOUNDCHANNELS, SOUNDRATE, SOUNDFRAMES, 
                                    SOUNDBUFFERSIZE * SOUNDCHANNELS * 4, "plughw:CARD=PCH,DEV=0", "0", NULL};

void
quit_activated (GSimpleAction *action,  GVariant *parameter, gpointer app){

  g_application_quit (G_APPLICATION (app));
}

static GActionEntry app_entries[] =
{ 
  { "openfile", activate_openfile, NULL, NULL, NULL},
  { "opencolor", activate_color, NULL, NULL, NULL},
  { "quit", quit_activated, NULL, NULL, NULL}
};

static void M_app_init (MApp *app){
}

static void M_app_startup (GApplication *app){ 
  GMenuModel *menubar; 
  GtkBuilder *builder; 
  GMenuModel *app_menu;  


  const gchar *quit_accels[2] = { "<Ctrl>q", NULL };
  const gchar *accel[2] = { "<Ctrl>n", NULL };

  G_APPLICATION_CLASS (M_app_parent_class)->startup (app);

  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);  

  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.openfile", accel);
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.quit", quit_accels);            

  builder = gtk_builder_new_from_resource ("/org/gtk/MLData/app-menu.ui");  
  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));     
  gtk_application_set_menubar (GTK_APPLICATION (app), app_menu);   
  g_object_unref (builder); 

  
}
static void M_app_activate (GApplication *app){
  MAppWindow *win;

  win = M_app_window_new (M_APP (app));
  gtk_window_present (GTK_WINDOW (win));
}

static void M_app_open (GApplication *app, GFile **files, int n_files, const char *hint){
  GList *windows;
  MAppWindow *win;
  int i;

  windows = gtk_application_get_windows (GTK_APPLICATION (app));
  if (windows)
    win = M_APP_WINDOW (windows->data);
  else
    win = M_app_window_new (M_APP (app));

  for (i = 0; i < n_files; i++)
    M_app_text_open (win, files[i]);

  gtk_window_present (GTK_WINDOW (win));
}

static void M_app_class_init (MAppClass *class){
  G_APPLICATION_CLASS (class)->startup = M_app_startup;
  G_APPLICATION_CLASS (class)->activate = M_app_activate;
  G_APPLICATION_CLASS (class)->open = M_app_open;
}

MApp * M_app_new (void){

  return g_object_new (M_APP_TYPE, "application-id", "org.gtk.MLData",
                             "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
