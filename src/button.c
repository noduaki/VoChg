#include "header.h"

//Button ********
void selButton1(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  if(!da->status.open){
    da->status.selNum = 1;
    da->status.open   = 1;
    if(gSet.file == NULL)
      da->flag.soundFile = 0;
    else da->flag.soundFile = 1;
    soundThread(widget, data);
  }else statusprint("Please push stop button Button", data);

  
  
}

void selButton2(GtkWidget *widget, gpointer data){
  VApp* da = (VApp*)data;
  if(!da->status.open){
    da->status.selNum = 2;
    da->status.open   = 1;
    da->flag.soundFile = 1;
  }else statusprint("Please push stop button", data);

  mlDataThread(widget, data);
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