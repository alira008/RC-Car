
#ifndef BLUETOOTH_RECEIVE_H
#define BLUETOOTH_RECEIVE_H

#include "usart_ATmega1284.h"

//========================= How we are receiving data ======================================================
/*
                | bit8 | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
                | tbd  | tbd  | tbd  | -- Servo Motor --- | ---- DC Motor ---- |
                                     | 8 different angles |   3 speeds  | direction |
    variables:                       |      LRAngles      |   FRSpeed   | FRDirection |
*/
//========================= Shared Variables ===============================================================

unsigned char receivedValue = 0x00;

//	DC Motor Variables
unsigned char DCMotorSpeed = 0x00;
unsigned char DCMotorDirection = 0x00;

//	Servo Motor Variables
unsigned char ServoMotorAngle = 0x00;

//==========================================================================================================

void start_usart(unsigned char num) {
    initUSART(num);
}

//================= Bluetooth Receiver State Machine =======================================================

enum BluetoothReceiver_states {BluetoothReceiverWait, BluetoothReceiverReadData};

int BluetoothReceiverTick(int state) {
    //  State Transitions
    switch(state) {
        case BluetoothReceiverWait:
            if(USART_HasReceived(0)) {
                state = BluetoothReceiverReadData;
            } else {
                state = BluetoothReceiverWait;
            }
            break;

        case BluetoothReceiverReadData:
            state = BluetoothReceiverWait;
            break;

        default:
            state = BluetoothReceiverWait;
            break;
    }

    //  State Actions
    switch(state) {
        case BluetoothReceiverWait:
            break;

        case BluetoothReceiverReadData:
            //  Retrieve value from the data register
            receivedValue = USART_Receive(0);
            //  Clear data register
            USART_Flush(0);
            break;
    }

    return state;
}

//=================== Parse Received Data State Machine ======================

enum ParseData_states {ParseDataGo};

int ParseDataTick(int state) {
    //  State Transitions
    switch(state) {
        case ParseDataGo:
            state = ParseDataGo;
            break;

        default:
            state = ParseDataGo;
            break;
    }

    //  State Actions
    switch(state) {
        case ParseDataGo:
        	//PORTC = receivedValue;
            DCMotorDirection = 0x01 & receivedValue;
            DCMotorSpeed = 0x02 & (receivedValue >> 1);
            ServoMotorAngle = 0x07 & (receivedValue >> 3);
            break;
    }

    return state;
}

//==========================================================================================================

#endif