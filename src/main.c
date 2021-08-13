#include <gtk/gtk.h>

#include "MApp.h"

int main(int argc, char *argv[])
{
   
   return g_application_run (G_APPLICATION (M_app_new ()), argc, argv);   
}
