#ifndef _INIT_H_
#define _INIT_H_
#include "includes.h"

#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGTH 1080.0f

#define SHUTDOWN_X 825.0f
#define SHUTDOWN_Y 400.0f
#define SHUTDOWN_Z 0.0f
#define SHUTDOWN_WIDTH 58.0f
#define SHUTDOWN_HEIGTH 55.0f

#define DOWNLOAD_X 645.0f
#define DOWNLOAD_Y SHUTDOWN_Y
#define DOWNLOAD_Z 0.0f
#define DOWNLOAD_WIDTH 74.0f
#define DOWNLOAD_HEIGTH 55.0f

#define INTERNET_X 735.0f
#define INTERNET_Y SHUTDOWN_Y
#define INTERNET_Z 0.0f
#define INTERNET_WIDTH 52.0f
#define INTERNET_HEIGTH 52.0f

#define SETTINGS_X -785.0f
#define SETTINGS_Y SHUTDOWN_Y
#define SETTINGS_Z -3.0f
#define SETTINGS_WIDTH 74.0f
#define SETTINGS_HEIGTH 53.0f

#define GAME_X -895.0f
#define GAME_Y SHUTDOWN_Y
#define GAME_Z -3.0f
#define GAME_WIDTH 77.0f
#define GAME_HEIGTH 54.0f

#define BACKGROUND_X -960.0f
#define BACKGROUND_Y -540.0f
#define BACKGROUND_Z 0.0f
#define BACKGROUND_WIDTH 1920.0f
#define BACKGROUND_HEIGTH 1080.0f



#define SHIMMER_X -725.0f
#define SHIMMER_Y -150.0f
#define SHIMMER_Z -3.0f
#define SHIMMER_WIDTH 560.0f
#define SHIMMER_HEIGTH 400.0f

#define TV_X -125.0f
#define TV_Y -150.0f
#define TV_Z -3.0f
#define TV_WIDTH 400.0f
#define TV_HEIGTH 400.0f

#define STEAMOS_X 315.0f
#define STEAMOS_Y -150.0f
#define STEAMOS_Z -3.0f
#define STEAMOS_WIDTH 400.0f
#define STEAMOS_HEIGTH 400.0f

#define SETTINGS_X -785.0f
#define SETTINGS_Y SHUTDOWN_Y
#define SETTINGS_Z -3.0f
#define SETTINGS_WIDTH 74.0f
#define SETTINGS_HEIGTH 53.0f

#define GAME_X -895.0f
#define GAME_Y SHUTDOWN_Y
#define GAME_Z -3.0f
#define GAME_WIDTH 77.0f
#define GAME_HEIGTH 54.0f

#define AMPLIFY 20.0f

#define MAX_INCREASE 10


void BaseInit();
void *JoystickCreateThread(void *arg);
void TimerFun(int value);
void BaseTimerFun(int value);
void BaseRender();

void ResetCoord(void);

void Drawtext(char *string,double x,double y);
void TimeDisplay(void);
void help_note();
void BasePicPoad();
void MouseMove(int x, int y);

void TipNote();




#endif  //_INIT_H_










