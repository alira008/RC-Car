//	Author: ariel

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "usart_ATmega1284.h"

//========================= Macro Variables ==============================

// Button 1 will be connected to pin 0
// If button is high, the button is pressed
#define BUTTON1 ~PINA & 0x01

// Button 2 will be connected to pin 1
// If button is high, the button is pressed
#define BUTTON2 ~PINA & 0x02

//========================= Shared Variables =============================

// Three modes
// Mode 0: All LEDs off
// Mode 1: Tell LEDs to turn on left to right
// Mode 2: Tell LEDs to turn on right to left
unsigned char LEDBlinkMode = 0x00;

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

//========================= Button State Machine ===========================

enum ButtonSelector_states {ButtonSelectorWait, ButtonSelectorReset, ButtonSelector1Pressed, ButtonSelector2Pressed, ButtonSelectorDepress};

int ButtonSelectorTick(int state) {
    //  State Transitions
    switch(state){
        case ButtonSelectorWait:
            if(BUTTON1 && BUTTON2) {                   //   If button 1 and button 2 are pressed reset LEDs     
                state = ButtonSelectorReset;
            } else if(BUTTON1) {                       //   If button 1 is pressed set mode to 1
                state = ButtonSelector1Pressed;
            } else if(BUTTON2) {                       //   If button 2 is pressed set mode to 2
                state = ButtonSelector2Pressed;
            } else {
                state = ButtonSelectorWait;
            }
            break;

        case ButtonSelectorReset:
            state = ButtonSelectorDepress;
            break;

        case ButtonSelector1Pressed:
            state = ButtonSelectorDepress;
            break;

        case ButtonSelector2Pressed:
            state = ButtonSelectorDepress;
            break;

        case ButtonSelectorDepress:
            if(!(BUTTON1) && !(BUTTON2)) {
                state = ButtonSelectorWait;
            } else {
                state = ButtonSelectorDepress;
            }
            break;

        default:
            state = ButtonSelectorWait;
            break;
    }

    //  State Actions
    switch(state){
        case ButtonSelectorWait:
            break;

        case ButtonSelectorReset:
            LEDBlinkMode = 0x00;
            break;

        case ButtonSelector1Pressed:
            LEDBlinkMode = 0x01;
            break;

        case ButtonSelector2Pressed:
            LEDBlinkMode = 0x02;
            break;

        case ButtonSelectorDepress:
            break;

    }

    return state;
}

//====================== Combine Data to Send State Machine ==============

enum CombineData_states {CombineDataGo};

int CombineDataTick(int state) {
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
            transmitValue = LEDBlinkMode;
            break;
    }

    return state;
}

//========================================================================

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;      // Set all pins as input

    unsigned long int TransmitDataTick_calc = 1;
    unsigned long int ButtonSelectorTick_calc = 1;
    unsigned long int CombineDataTick_calc = 1;

    // Calculate GCD
    unsigned long int tmpGCD = findGCD(TransmitDataTick_calc, ButtonSelectorTick_calc);
    tmpGCD = findGCD(tmpGCD, CombineDataTick_calc);

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int TransmitDataTick_period = TransmitDataTick_calc/GCD;
    unsigned long int ButtonSelectorTick_period = ButtonSelectorTick_calc/GCD;
    unsigned long int CombineDataTick_period = CombineDataTick_calc/GCD;

    static task task1, task2, task3;
    task *tasks[] = { &task1, &task2, &task3 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Task 1
    task1.state = -1;
    task1.period = TransmitDataTick_period;
    task1.elapsedTime = TransmitDataTick_period;
    task1.TickFct = &TransmitDataTick;

    // Task 2
    task2.state = -1;
    task2.period = ButtonSelectorTick_period;
    task2.elapsedTime = ButtonSelectorTick_period;
    task2.TickFct = &ButtonSelectorTick;

    // Task 3
    task3.state = -1;
    task3.period = CombineDataTick_period;
    task3.elapsedTime = CombineDataTick_period;
    task3.TickFct = &CombineDataTick;

    initUSART(0);
    TimerSet(GCD);
    TimerOn();

    unsigned short i;
    while (1) {
        // Scheduler code
        for ( i = 0; i < numTasks; i++ ) {
            // Task is ready to tick
            if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
    while(!TimerFlag);
    TimerFlag = 0;

    }
    return 1;
}
