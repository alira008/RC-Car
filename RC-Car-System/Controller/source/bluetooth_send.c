#include <avr/io.h>
#include "usart_ATmega1284.h"
#include "bluetooth_send.h"
#include "joystick_IO.h"

//========================= How we are sending data =======================
/*
                | bit8 | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
                | tbd  | tbd  | tbd  | -- Servo Motor --- | ---- DC Motor ---- |
                                     | 8 different angles |   3 speeds  | direction |
    variables:                       |      LRAngles      |   FRSpeed   | FRDirection |
*/
//========================= Shared Variables ==============================

// Value we want to send
unsigned char transmitValue = 0x00;

//========================= USART State Machine ===========================

enum TransmitData_states {TransmitDataWait, TransmitDataWrite};

int TransmitDataTick(int state) {
    //  State Transitions
    switch(state) {
        case TransmitDataWait:
            if(USART_IsSendReady(0)) {
                state = TransmitDataWrite;
            } else {
                state = TransmitDataWait;
            }
            break;

        case TransmitDataWrite:
            if(USART_HasTransmitted(0)) {
                state = TransmitDataWait;
                USART_Flush(0);
            } else {
                state = TransmitDataWrite;
            }
            break;

        default:
            state = TransmitDataWait;
            break;
    }

    //  State Actions
    switch(state) {
        case TransmitDataWait:
            break;

        case TransmitDataWrite:
            USART_Send(transmitValue, 0);
            break;
    }

    return state;
}

//======================= Combine Data State Machine =========================

enum CombineData_states {CombineDataGo};

int CombineDataTick(int state) {
    unsigned char FRDirection, FRSpeed, LRAngles;
    //  State Transitions
    switch(state) {
        case CombineDataGo:
            state = CombineDataGo;
            break;

        default:
            state = CombineDataGo;
            break;
    }

    //  State Actions
    switch(state) {
        case CombineDataGo:
            transmitValue = LRAngles << 3 | FRSpeed << 1 | FRDirection;
            break;
    }

    return state;
}

//============================================================================