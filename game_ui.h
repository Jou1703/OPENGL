#ifndef _GAME_H_
#define _GAME_H_
#include "option.h"

#define GAMELIB_WIDTH 400.0f
#define GAMELIB_HEIGTH 300.0f

#define GAMELIB_Y 0.0f
#define GAMELIB_Z -2.0f

#define GAMELIB_MINPOS 1
#define GAMELIB_MAXPOS 5

void GamePicLoad(void);
void GameRender(void);
void *MountDisk(void *arg);
void *DelMoveGame(void *arg);
void *CpLocal2Move(void *arg);
void *CpMove2Local(void *arg);
int GetgameSize();
int GetSgameSize();
void DrawCpGame();


#endif  //_GAME_H_
