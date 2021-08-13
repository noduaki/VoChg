#ifndef __MAPPWIN_H
#define __MAPPWIN_H

#include <gtk/gtk.h>
#include "MApp.h"




typedef struct _MAppWindowPrivate
{ 
  GSettings *settings;         //settings
  GtkWidget *content_box;      //UI file Widget Registrasion  
  GtkWidget *content_boxIn;  
  GtkWidget *textview;
  
  GtkWidget *entrytext;
  GtkWidget *entry1;
  GtkWidget *draw1;
  GtkWidget *draw2;
 
  GtkWidget *selectButton1;
  GtkWidget *selectButton2;
  GtkWidget *selectButton3;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *button4;
  GtkWidget *button5;
  GtkWidget *button6;
  GtkWidget *button7;  
  GtkWidget *button8;
  GtkWidget *button9;
  GtkWidget *button10;
  GtkWidget *button11;
  GtkWidget *button12;
  GtkWidget *button13;
  GtkWidget *button14;
  GtkWidget *button15;
  GtkWidget *button16;
  GtkWidget *button17;
  GtkWidget *button18;
  GtkWidget *button19;
  GtkWidget *button20;
  
  GtkWidget *scale1;
  GtkWidget *scale2;
  GtkWidget *scale3;
  GtkWidget *scale4;
  GtkWidget *scale5;
  GtkWidget *scale6;
  GtkWidget *scale7;
  GtkWidget *scale8;
  GtkWidget *scale9;
  GtkWidget *scale10;
  GtkWidget *scale11;
  GtkWidget *scale12;
  GtkWidget *scale13;
  GtkWidget *scale14;
  GtkWidget *scale15;

  GtkWidget *popuplist1;
  GtkWidget *popuplist2;
  GtkWidget *popuplist3;
  GtkWidget *popuplist4;
  GtkWidget *popuplist5;

  GtkWidget *popupMenu;


}MAppWindowPrivate;

typedef struct _mInt{
    int data1;
    int data2;
    int data3;
    int* pData;
}mInt;

typedef struct _mFloat{
    float data1;
    float data2;
    float data3;
    float* pData;
}mFloat;

typedef struct _mDouble{
    double data1;
    double data2;
    double data3;
    double* pData;
}mDouble;

typedef struct _mFlag{
    int flag1;
    int flag2;
    int flag3;
}mFlag;

typedef struct _mScale{
    double slider1;
    double slider2;
    double slider3;
    double slider4;
    double slider5;
    double slider6;
    double slider7;
    double slider8;
    double slider9;
    double slider10;
    double slider11;
    double slider12;
    double slider13;
    double slider14;
    double slider15;

}mScale;

typedef struct _mDraw{
    int     drawAreaWidth;
    int     drawAreaHeight;
    double*  x;
    double*  y; 
}mDraw;

typedef struct _mStatus{
    int open;
    int selNum;
}mStatus;

typedef struct _VApp{
  MAppWindowPrivate *priv;
  mStatus   status;
  mInt      dataInt;
  mFloat    dataFloat;
  mDouble   dataDouble;
  mFlag     flag;
  mScale    scale;
  mDraw     draw1;
  mDraw     draw2;
  double    entry1;
}VApp;

void soundThread(GtkWidget* window, gpointer data);
void mlDataThread(GtkWidget* window, gpointer data);
void SB3Thread(GtkWidget* window, gpointer data);
MAppWindow  *M_app_window_new(MApp *app);
void M_app_window_open(MAppWindow *win, GFile  *file);




#endif