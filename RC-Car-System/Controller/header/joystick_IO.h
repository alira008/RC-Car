
#ifndef JOYSTICK_IO_H
#define JOYSTICK_IO_H


//========================= Shared Variables =======================================

//  ADC input of left joystick controlling DC motor
unsigned short FRJoystickInput;

//  FR joystick directions: 0 == forward, 1 == reverse
extern unsigned char FRDirection;

//  FR joystick speeds: 0, 1, 2, 3 (zero speed, low speed, medium speed, fast speed)
extern unsigned char FRSpeed;

//  LR joystick angles: 0, 1, 2, 3, 4, 5, 6, 7 ()
extern unsigned char LRAngles;

//================== Foward/Reverse Joystick State Machine ================================

int FRJoystickDirectionTick(int state);

//================= FR Joystick Motor Speed State Machine =================================

int FRJoystickSpeedTick(int state);

//=================== Servo Motor Angle State Machine =====================================

int LRJoystickTick(int state);

//=========================================================================================

#endif