#ifndef __SOUND_H__
#define __SOUND_H__
#include "alsa_src/AlsaPlayCtrl_F.h"

void SoundInit();
void SndStop();
void SndPlay();

void SndResume();

void SndSet(int n);
void SndMove();

#endif //__SOUND_H__
