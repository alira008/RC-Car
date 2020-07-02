//	Author: ariel
#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "usart_ATmega1284.h"

//================= Shared Variables =========================================

unsigned char receivedValue = 0x00;

//  First two bit
unsigned char LEDmode = 0x00;

//  Output of which LEDs will be on
unsigned char fourLEDsOut = 0x00;

//================= Bluetooth Receiver State Machine =========================

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
            //  We will only look at first two bits for the LED modes
            //  0000 0011 = 0x03
            LEDmode = 0x03 & receivedValue;
            break;
    }

    return state;
}

//===================== LED Mode 1 State Machines ============================

enum LEDMode1_states {LEDMode1Wait, LEDMode1Go};

int LEDMode1Tick(int state) {
    //  State Transitions
    switch(state) {
        case LEDMode1Wait:
            if(LEDmode == 0x01) {
                state = LEDMode1Go;
            } else {
                state = LEDMode1Wait;
            }
            break;

        case LEDMode1Go:
            if(LEDmode == 0x01) {
                state = LEDMode1Go;
            } else {
                fourLEDsOut = 0x00;
                state = LEDMode1Wait;
            }
            break;

        default:
            state = LEDMode1Wait;
            break;
    }

    //  State Actions
    switch(state) {
        case LEDMode1Wait:
            break;

        case LEDMode1Go:
            if(fourLEDsOut == 0x00) {
                fourLEDsOut = 0x01;
            } else if(fourLEDsOut == 0x01) {
                fourLEDsOut = 0x02;
            } else if(fourLEDsOut == 0x02) {
                fourLEDsOut = 0x04;
            } else if(fourLEDsOut == 0x04) {
                fourLEDsOut = 0x08;
            } else {
                fourLEDsOut = 0x01;
            }
            break;
    }

    return state;
}

//===================== LED Mode 2 State Machines ============================

enum LEDMode2_states {LEDMode2Wait, LEDMode2Go};

int LEDMode2Tick(int state) {
    //  State Transitions
    switch(state) {
        case LEDMode2Wait:
            if(LEDmode == 0x02) {
                state = LEDMode2Go;
            } else {
                state = LEDMode2Wait;
            }
            break;

        case LEDMode2Go:
            if(LEDmode == 0x02) {
                state = LEDMode2Go;
            } else {
                fourLEDsOut = 0x00;
                state = LEDMode2Wait;
            }
            break;

        default:
            state = LEDMode2Wait;
            break;
    }

    //  State Actions
    switch(state) {
        case LEDMode2Wait:
            break;

        case LEDMode2Go:
            if(fourLEDsOut == 0x00) {
                fourLEDsOut = 0x08;
            } else if(fourLEDsOut == 0x08) {
                fourLEDsOut = 0x04;
            } else if(fourLEDsOut == 0x04) {
                fourLEDsOut = 0x02;
            } else if(fourLEDsOut == 0x02) {
                fourLEDsOut = 0x01;
            } else {
                fourLEDsOut = 0x08;
            }
            break;
    }

    return state;
}

//===================== Ouput LEDs State Machines ============================

enum OutputLEDs_states {OutputLEDsGo};

int OutputLEDsTick(int state) {
    //  State Transitions
    switch(state) {
        case OutputLEDsGo:
            state = OutputLEDsGo;
            break;

        default:
            state = OutputLEDsGo;
            break;
    }

    //  State Actions
    switch(state) {
        case OutputLEDsGo:
            // if(LEDmode == 0x00){
            //     PORTC = 0x00;
            // } else {
                PORTC = receivedValue;
            //}
            break;
    }

    return state;
}

//============================================================================ 

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;  //  All pins are output

    unsigned long int BluetoothReceiverTick_calc = 100;
    unsigned long int ParseDataTick_calc = 1;
    unsigned long int LEDMode1Tick_calc = 500;
    unsigned long int LEDMode2Tick_calc = 500;
    unsigned long int OutputLEDsTick_calc = 1;

    // Calculate GCD
    unsigned long int tmpGCD = findGCD(BluetoothReceiverTick_calc, ParseDataTick_calc);
    tmpGCD = findGCD(tmpGCD, LEDMode1Tick_calc);
    tmpGCD = findGCD(tmpGCD, LEDMode2Tick_calc);
    tmpGCD = findGCD(tmpGCD, OutputLEDsTick_calc);

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int BluetoothReceiverTick_period = BluetoothReceiverTick_calc/GCD;
    unsigned long int ParseDataTick_period = ParseDataTick_calc/GCD;
    unsigned long int LEDMode1Tick_period = LEDMode1Tick_calc/GCD;
    unsigned long int LEDMode2Tick_period = LEDMode2Tick_calc/GCD;
    unsigned long int OutputLEDsTick_period = OutputLEDsTick_calc/GCD;

    static task task1, task2, task3, task4, task5;
    task *tasks[] = { &task1, &task2, &task3, &task4, &task5 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    //  Task 1
    task1.state = -1;
    task1.period = BluetoothReceiverTick_period;
    task1.elapsedTime = BluetoothReceiverTick_period;
    task1.TickFct = &BluetoothReceiverTick;

    //  Task 2
    task2.state = -1;
    task2.period = ParseDataTick_period;
    task2.elapsedTime = ParseDataTick_period;
    task2.TickFct = ParseDataTick;

    //  Task 3
    task3.state = -1;
    task3.period = LEDMode1Tick_period;
    task3.elapsedTime = LEDMode1Tick_period;
    task3.TickFct = &LEDMode1Tick;

    //  Task 4
    task4.state = -1;
    task4.period = LEDMode2Tick_period;
    task4.elapsedTime = LEDMode2Tick_period;
    task4.TickFct = LEDMode2Tick;

    //  Task 5
    task5.state = -1;
    task5.period = OutputLEDsTick_period;
    task5.elapsedTime = OutputLEDsTick_period;
    task5.TickFct = OutputLEDsTick;

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
