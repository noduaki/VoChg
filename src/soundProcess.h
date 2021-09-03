#ifndef __SOUNDPROCESS__H
#define __SOUNDPROCESS__H
#include "header.h"

extern GMutex mutex_sound;
void soundProcessPoll(VApp* da);
#endif