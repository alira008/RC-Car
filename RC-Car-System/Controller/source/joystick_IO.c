#include <avr/io.h>
#include "adc_ATmega1284.h"
#include "joystick_IO.h"


//========================= Shared Variables =======================================

//  ADC input of left joystick controlling DC motor
unsigned short FRJoystickInput = 0x00;

//  FR joystick directions: 0 == forward, 1 == reverse
unsigned char FRDirection = 0x00;

//  FR joystick speeds: 0, 1, 2, 3 (zero speed, low speed, medium speed, fast speed)
unsigned char FRSpeed = 0x00;

//  LR joystick angles: 0, 1, 2, 3, 4, 5, 6, 7 ()
unsigned char LRAngles = 0x00;

//==================================================================================

void start_adc() {
    ADC_init();
}

//================== Foward/Reverse Joystick State Machine =========================

enum FRJoystickDirection_states {FRJoystickDirectionIdle, FRJoystickDirectionForward, FRJoystickDirectionReverse};

int FRJoystickDirectionTick(int state) {
    FRJoystickInput = ADC_read(1);
    // State Transitions
    switch(state) {
        case FRJoystickDirectionIdle:
            if(FRJoystickInput > 550) {
                state = FRJoystickDirectionForward;
            } else if(FRJoystickInput < 470) {
                state = FRJoystickDirectionReverse;
            }
            else {
                state = FRJoystickDirectionIdle;
            }
            break;

        case FRJoystickDirectionForward:
            state = FRJoystickDirectionIdle;
            break;

        case FRJoystickDirectionReverse:
            state = FRJoystickDirectionIdle;
            break;

        default:
            state = FRJoystickDirectionIdle;
            break;
    }

    // State Actions
    switch(state) {
        case FRJoystickDirectionIdle:
            FRDirection = 0;
            break;

        case FRJoystickDirectionForward:
            FRDirection = 0;
            break;

        case FRJoystickDirectionReverse:
            FRDirection = 1;
            break;
    }
    return state;
}

//================= FR Joystick Motor Speed State Machine ================================

enum FRJoystickSpeed_states {FRJoystickSpeedIdle, FRJoystickSpeedGo};

int FRJoystickSpeedTick(int state) {
    //  State Transitions
    switch(state) {
        case FRJoystickSpeedIdle:
            if(FRJoystickInput > 550 || FRJoystickInput < 470) {
                state = FRJoystickSpeedGo;
            }
            else {
                state = FRJoystickSpeedIdle;
            }
            break;

        case FRJoystickSpeedGo:
            if(FRJoystickInput > 550 || FRJoystickInput < 470) {
                state = FRJoystickSpeedGo;
            }
            else {
                state = FRJoystickSpeedIdle;
            }
            break;

        default:
            state = FRJoystickSpeedIdle;
            break;
    }
    //  State Actions
    switch(state) {
        case FRJoystickSpeedIdle:
            FRSpeed = 0x00;
            break;

        case FRJoystickSpeedGo:
            if((FRJoystickInput > 0 && FRJoystickInput <= 156) || (FRJoystickInput > 862)){
                FRSpeed = 0x03;
            }
            else if((FRJoystickInput > 156 && FRJoystickInput <= 312) || (FRJoystickInput > 706 && FRJoystickInput <= 862)){
                FRSpeed = 0x02;
            }
            else if((FRJoystickInput > 312 && FRJoystickInput <= 468) || (FRJoystickInput > 550 && FRJoystickInput <= 706)){
                FRSpeed = 0x01;
            }
            break;
    }

    return state;
}

//=================== Servo Motor Angle State Machine =====================================

enum LRJoystick_states {LRJoystickIdle, LRJoystickTurnLeft, LRJoystickTurnRight};

int LRJoystickTick(int state) {
    unsigned short LRJoystickInput = ADC_read(0);
    // State Transitions
    switch(state) {
        case LRJoystickIdle:
            if(LRJoystickInput > 550) {
                state = LRJoystickTurnRight;
            } else if(LRJoystickInput < 470) {
                state = LRJoystickTurnLeft;
            }
            else {
                state = LRJoystickIdle;
            }
            break;

        case LRJoystickTurnLeft:
            if(LRJoystickInput > 550) {
                state = LRJoystickTurnRight;
            } else if(LRJoystickInput < 470) {
                state = LRJoystickTurnLeft;
            }
            else {
                state = LRJoystickIdle;
            }
            break;

        case LRJoystickTurnRight:
            if(LRJoystickInput > 550) {
                state = LRJoystickTurnRight;
            } else if(LRJoystickInput < 470) {
                state = LRJoystickTurnLeft;
            }
            else {
                state = LRJoystickIdle;
            }
            break;

        default:
            state = LRJoystickIdle;
            break;
    }

    // State Actions
    switch(state) {
        case LRJoystickIdle:
            LRAngles = 0x03;
            break;

        case LRJoystickTurnLeft:
            if(LRJoystickInput > 0 && LRJoystickInput <= 156){
                LRAngles = 0x00;
            } else if(LRJoystickInput > 156 && LRJoystickInput <= 312){
                LRAngles = 0x01;
            } else if(LRJoystickInput > 312 && LRJoystickInput <= 470){
                LRAngles = 0x02;
            }
            break;

        case LRJoystickTurnRight:
            if(LRJoystickInput > 550 && LRJoystickInput <= 706){
                LRAngles = 0x04;
            } else if(LRJoystickInput > 706 && LRJoystickInput <= 862){
                LRAngles = 0x05;
            } else if(LRJoystickInput > 862){
                LRAngles = 0x06;
            }
            break;
    }
    return state;
}

//===========================================================================================