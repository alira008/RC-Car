//	Author: ariel

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "adc_ATmega1284.h"
#include "dc_motor.h"
#include "servo_motor.h"
#include "bluetooth_receive.h"

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;

    unsigned long int BluetoothReceiverTick_calc = 100;
    unsigned long int ParseDataTick_calc = 100;
    unsigned long int DCMotorDirectionTick_calc = 100;
    unsigned long int DCMotorSpeedTick_calc = 100;
    unsigned long int ServoMotorTick_calc = 100;

    // Calculate GCD
    unsigned long int tmpGCD = findGCD(BluetoothReceiverTick_calc, ParseDataTick_calc);
    tmpGCD = findGCD(tmpGCD, DCMotorDirectionTick_calc);
    tmpGCD = findGCD(tmpGCD, DCMotorSpeedTick_calc);
    tmpGCD = findGCD(tmpGCD, ServoMotorTick_calc);

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int BluetoothReceiverTick_period = BluetoothReceiverTick_calc/GCD;
    unsigned long int ParseDataTick_period = ParseDataTick_calc/GCD;
    unsigned long int DCMotorDirectionTick_peiod = DCMotorDirectionTick_calc/GCD;
    unsigned long int DCMotorSpeedTick_period = DCMotorSpeedTick_calc/GCD;
    unsigned long int ServoMotorTick_period = ServoMotorTick_calc/GCD;

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
    task2.TickFct = &ParseDataTick;

    //  Task 3
    task3.state = -1;
    task3.period = DCMotorDirectionTick_peiod;
    task3.elapsedTime = DCMotorDirectionTick_peiod;
    task3.TickFct = &DCMotorDirectionTick;

    //  Task 4
    task4.state = -1;
    task4.period = DCMotorSpeedTick_period;
    task4.elapsedTime = DCMotorSpeedTick_period;
    task4.TickFct = &DCMotorSpeedTick;

    //  Task 5
    task5.state = -1;
    task5.period = ServoMotorTick_period;
    task5.elapsedTime = ServoMotorTick_period;
    task5.TickFct = &ServoMotorTick;

    //  Initiate USART 0
    start_usart(0);

    //  Initiate PWM for DC/Servo Motors
    DCPWM_init();
    ServoPWM_init();

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
