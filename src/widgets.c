#include "header.h"

// Entry *********
void fentry1(GtkWidget* widget, gpointer data) {
    VApp* da = (VApp*)data;
   
    da->entry1 = atof(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(da->statusBuf, "%f", da->entry1);
    statusprint(data);
}

gboolean statusprint(gpointer data) {
    VApp* da = (VApp*)data;
    gtk_statusbar_pop(GTK_STATUSBAR(da->priv->entrytext), 1);
    gtk_statusbar_push(GTK_STATUSBAR(da->priv->entrytext), 1, da->statusBuf);
    *(da->statusBuf) = '\0';
    return FALSE;
}

void popupMenu() {
}
