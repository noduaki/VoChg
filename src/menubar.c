#include "header.h"

void activate_openfile(GSimpleAction* simple, GVariant* parameter, gpointer app) {
    GList* windows;
    MAppWindow* win;
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    GApplication* a = (GApplication*)app;

    windows = gtk_application_get_windows(GTK_APPLICATION(app)); // windows get app currently focused window data

    if (windows)
        win = M_APP_WINDOW(windows->data);
    else
        win = M_app_window_new(M_APP(app));

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"),
                                         GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {

        gchar* filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        snprintf(gSet.readfilename, 255, "%s", filename);
        gSet.readfile = g_file_new_for_path(filename);

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void activate_color(GSimpleAction* simple, GVariant* parameter, gpointer app) {
    GList* windows;
    MAppWindow* win;
    GtkWidget* dialog;
    gint res;
    GdkRGBA* myColor;
    myColor = (GdkRGBA*)malloc(sizeof(GdkRGBA));
    windows = gtk_application_get_windows(GTK_APPLICATION(app)); // windows get app currently focused window data
    win = M_APP_WINDOW(windows->data);

    dialog = gtk_color_chooser_dialog_new(NULL, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_OK) {
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dialog), myColor);

        printf(" %f, %f, %f, %f \n", myColor->red, myColor->green, myColor->blue, myColor->alpha);
    }
   
    gtk_widget_destroy(dialog);
}

// Menubar func ***********

void init_list(GtkWidget* list) {

    GtkCellRenderer* renderer;
    GtkTreeViewColumn* column;
    GtkListStore* store;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    store = gtk_list_store_new(1, G_TYPE_STRING);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    g_object_unref(store);
}

void on_changed(GtkWidget* widget, gpointer label) {

    GtkTreeIter iter;
    GtkTreeModel* model;
    gchar* value;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {

        gtk_tree_model_get(model, &iter, 0, &value, -1);
        strcpy(gSet.deviceName, value);

        g_free(value);
    }
}

void on_ratechanged(GtkWidget* widget, gpointer label) {

    GtkTreeIter iter;
    GtkTreeModel* model;
    gchar* value;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {

        gtk_tree_model_get(model, &iter, 0, &value, -1);

        if (!(strcmp(value, "44100")))
            gSet.rate = 44100;
        else if (!(strcmp(value, "48000")))
            gSet.rate = 48000;
        else if (!(strcmp(value, "88200")))
            gSet.rate = 88200;
        else if (!(strcmp(value, "96000")))
            gSet.rate = 96000;
        else if (!(strcmp(value, "192000")))
            gSet.rate = 192000;
        g_free(value);
    }
}

void on_frameschanged(GtkWidget* widget, gpointer label) {

    GtkTreeIter iter;
    GtkTreeModel* model;
    gchar* value;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {

        gtk_tree_model_get(model, &iter, 0, &value, -1);

        if (!(strcmp(value, "512"))) {
            gSet.frames = 512;
            gSet.pcm_buffer_size = 512;
            gSet.period_size = 128;

        } else if (!(strcmp(value, "1024"))) {
            gSet.frames = 1024;
            gSet.pcm_buffer_size = 1024;
            gSet.period_size = 256;

        } else if (!(strcmp(value, "2048"))) {
            gSet.frames = 2048;
            gSet.pcm_buffer_size = 2048;
            gSet.period_size = 512;

        } else if (!(strcmp(value, "4096"))) {
            gSet.frames = 4096;
            gSet.pcm_buffer_size = 4096;
            gSet.period_size = 1024;

        } else if (!(strcmp(value, "8192"))) {
            gSet.frames = 8192;
            gSet.pcm_buffer_size = 8192;
            gSet.period_size = 2048;

        } else if (!(strcmp(value, "16384"))) {
            gSet.frames = 16384;
            gSet.pcm_buffer_size = 16384;
            gSet.period_size = 4096;
        }
        g_free(value);
    }
}

void add_to_list(GtkWidget* list, const gchar* str) {

    GtkListStore* store;
    GtkTreeIter iter;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, str, -1);
}

void choosesquit(GtkWidget* widget, gpointer window) {

    gtk_widget_destroy(GTK_WIDGET(window));
}

void cardnamesubmit(GtkWidget* widget, gpointer window) {

    gtk_widget_destroy(GTK_WIDGET(window));
}

void choosesrate(GSimpleAction* simple, GVariant* parameter, gpointer app) {

    GtkTreeSelection* selection;
    GtkWidget* rate;
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget* list = gtk_tree_view_new();
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget* add = gtk_button_new_with_label("Select");
    gtk_window_set_title(GTK_WINDOW(window), "Frames list");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, 1, 1, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), list);
    gtk_box_pack_end(GTK_BOX(box), add, FALSE, 0, 0);

    init_list(list);

    add_to_list(list, "44100");
    add_to_list(list, "48000");
    add_to_list(list, "88200");
    add_to_list(list, "96000");
    add_to_list(list, "192000");

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

    g_signal_connect(add, "clicked", G_CALLBACK(cardnamesubmit), window);
    g_signal_connect(selection, "changed", G_CALLBACK(on_ratechanged), rate);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(choosesquit), window);
    gtk_widget_show_all(window);
    return;
}

void choosesframes(GSimpleAction* simple, GVariant* parameter, gpointer app) {

    GtkTreeSelection* selection;
    GtkWidget* frames;
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget* list = gtk_tree_view_new();
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget* add = gtk_button_new_with_label("Select");
    gtk_window_set_title(GTK_WINDOW(window), "Frames list");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, 1, 1, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), list);
    gtk_box_pack_end(GTK_BOX(box), add, FALSE, 0, 0);

    init_list(list);

    add_to_list(list, "512");
    add_to_list(list, "1024");
    add_to_list(list, "2048");
    add_to_list(list, "4096");
    add_to_list(list, "8192");
    add_to_list(list, "16384");

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

    g_signal_connect(add, "clicked", G_CALLBACK(cardnamesubmit), window);
    g_signal_connect(selection, "changed", G_CALLBACK(on_frameschanged), frames);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(choosesquit), window);
    // g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK(choosesquit),
    // window);
    gtk_widget_show_all(window);
    return;
}

void choosescard(GSimpleAction* simple, GVariant* parameter, gpointer app) {

    void **hints, **n;
    int i;
    char *name, *descr, *descr1, *io;
    const char* filter;

    GtkTreeSelection* selection;
    GtkWidget* label;
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget* list = gtk_tree_view_new();
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget* add = gtk_button_new_with_label("Select");
    gtk_window_set_title(GTK_WINDOW(window), "Sound Card list");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, 1, 1, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), list);
    gtk_box_pack_end(GTK_BOX(box), add, FALSE, 0, 0);

    init_list(list);
    if (snd_device_name_hint(-1, "pcm", &hints) < 0) return;
    n = hints;
    filter = "Input";
    while (*n != NULL) {
        name = snd_device_name_get_hint(*n, "NAME");

        descr = snd_device_name_get_hint(*n, "DESC");
        io = snd_device_name_get_hint(*n, "IOID");
        if (io != NULL && strcmp(io, filter) == 0) {
            goto __free;
        }
        add_to_list(list, name);
    __free:

        if (name != NULL) free(name);
        if (descr != NULL) free(descr);
        if (io != NULL) free(io);
        n++;
    }
    snd_device_name_free_hint(hints);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

    g_signal_connect(add, "clicked", G_CALLBACK(cardnamesubmit), window);

    g_signal_connect(selection, "changed", G_CALLBACK(on_changed), label);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(choosesquit), window);
    gtk_widget_show_all(window);
    return;
}
