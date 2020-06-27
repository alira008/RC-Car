//	Author: ariel

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "usart_ATmega1284.h"
#include "adc_ATmega1284.h"
#include "bluetooth_send.h"
#include "joystick_IO.h"

int main(void) {
    // DDRA = 0x00; PORTA = 0xFF;      //  Set all A pins as input

    // /* Insert your solution below */
    // unsigned long int TransmitDataTick_calc = 1;
    // unsigned long int FRJoystickDirectionTick_calc = 1;
    // unsigned long int FRJoystickSpeedTick_calc = 1;
    // unsigned long int LRJoystickTick_calc = 1;
    // unsigned long int CombineDataTick_calc = 1;

    // // Calculate GCD
    // unsigned long int tmpGCD = findGCD(TransmitDataTick_calc, FRJoystickDirectionTick_calc);
    // tmpGCD = findGCD(tmpGCD, FRJoystickSpeedTick_calc);
    // tmpGCD = findGCD(tmpGCD, LRJoystickTick_calc);
    // tmpGCD = findGCD(tmpGCD, CombineDataTick_calc);

    // unsigned long int GCD = tmpGCD;

    // //Greatest common divisor for all tasks or smallest time unit for tasks.
    // unsigned long int TransmitDataTick_period = TransmitDataTick_calc/GCD;
    // unsigned long int FRJoystickDirectionTick_period = FRJoystickDirectionTick_calc/GCD;
    // unsigned long int FRJoystickSpeedTick_period = FRJoystickSpeedTick_calc/GCD;
    // unsigned long int LRJoystickTick_period = LRJoystickTick_calc/GCD;
    // unsigned long int CombineDataTick_period = CombineDataTick_calc/GCD;


    // static task task1, task2, task3, task4, task5;
    // task *tasks[] = { &task1, &task2, &task3, &task4, &task5 };
    // const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // //  Task 1
    // task1.state = -1;
    // task1.period = TransmitDataTick_period;
    // task1.elapsedTime = TransmitDataTick_period;
    // task1.TickFct = &TransmitDataTick;

    // //  Task 2
    // task2.state = -1;
    // task2.period = FRJoystickDirectionTick_period;
    // task2.elapsedTime = FRJoystickDirectionTick_period;
    // task2.TickFct = &FRJoystickDirectionTick;

    // //  Task 3
    // task3.state = -1;
    // task3.period = FRJoystickSpeedTick_period;
    // task3.elapsedTime = FRJoystickSpeedTick_period;
    // task3.TickFct = &FRJoystickSpeedTick;

    // //  Task 4
    // task4.state = -1;
    // task4.period = LRJoystickTick_period;
    // task4.elapsedTime = LRJoystickTick_period;
    // task4.TickFct = &LRJoystickTick;

    // //  Task 5
    // task5.state = -1;
    // task5.period = CombineDataTick_period;
    // task5.elapsedTime = CombineDataTick_period;
    // task5.TickFct = &CombineDataTick;

    // //  Inits for ADC input
    // ADC_init();

    // //  Init for USART0
    // initUSART(0);

    // //  Inits for scheduler timer
    // TimerSet(GCD);
    // TimerOn();

    // unsigned short i;
    // while (1) {
    //     // Scheduler code
    //     for ( i = 0; i < numTasks; i++ ) {
    //         // Task is ready to tick
    //         if ( tasks[i]->elapsedTime == tasks[i]->period ) {
    //             // Setting next state for task
    //             tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
    //             // Reset the elapsed time for next tick.
    //             tasks[i]->elapsedTime = 0;
    //         }
    //         tasks[i]->elapsedTime += 1;
    //     }
    // while(!TimerFlag);
    // TimerFlag = 0;

    // }
    // return 1;
}
