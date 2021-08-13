#include "header.h"

//Entry *********
void fentry1(GtkWidget *widget, gpointer data){
  VApp* tmp = (VApp*)data;
  
  tmp->entry1 =  atof(gtk_entry_get_text (GTK_ENTRY(widget))); 
  statusprint(gtk_entry_get_text (GTK_ENTRY(widget)), data);
}

void statusprint(const char* text, gpointer data){
  VApp* da = (VApp*)data;
  gtk_statusbar_pop (GTK_STATUSBAR(da->priv->entrytext), 1);
  gtk_statusbar_push (GTK_STATUSBAR(da->priv->entrytext), 1, text);
}

void popupMenu(){}