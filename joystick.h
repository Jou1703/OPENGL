#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_



void JoystickInit(void);
void JoystickResponse();
void KeyBoard ( unsigned char key, int x, int y);
void processSpecialKeys(int key, int x, int y);

#endif
