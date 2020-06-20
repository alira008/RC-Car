//	Author: ariel

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"


//=================== ADC Functions ====================================

void ADC_init(){
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

unsigned short ADC_read(unsigned char channel){
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    channel = channel & 0x07;
    ADMUX = (ADMUX & 0xF8) | channel;

    // start single convertion
    // write ’1′ to ADSC
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    return ADC;
}

//================== Shared Variables =========================

unsigned char moveForward, moveReverse;

//================== Foward/Reverse Joystick State Machine =========================

enum FRJoystick_states {FRJoystickIdle, FRJoystickForward, FRJoystickReverse};

int FRJoystickTick(int state) {
    unsigned short input = ADC_read(1);
    // State Transitions
    switch(state) {
        case FRJoystickIdle:
            if(input > 550) {
                state = FRJoystickForward;
            } else if(input < 530) {
                state = FRJoystickReverse;
            }
            else {
                state = FRJoystickIdle;
            }
            break;

        case FRJoystickForward:
            state = FRJoystickIdle;
            break;

        case FRJoystickReverse:
            state = FRJoystickIdle;
            break;

        default:
            state = FRJoystickIdle;
            break;
    }

    // State Actions
    switch(state) {
        case FRJoystickIdle:
            moveForward = 0x00;
            moveReverse = 0x00;
            break;

        case FRJoystickForward:
            moveForward = 0xFF;
            moveReverse = 0x00;
            break;

        case FRJoystickReverse:
            moveForward = 0x00;
            moveReverse = 0xFF;
            break;
    }
    return state;
}

//================== DC Motor State Machine ==============================

enum DCMotor_States {DCMotorIdle, DCMotorForward, DCMotorReverse};

int DCMotorTick(int state) {
    // State Transitions
    switch(state) {
        case DCMotorIdle:
            if(moveForward == 0xFF && moveReverse == 0x00) {
                state = DCMotorForward;
            } else if(moveForward == 0x00 && moveReverse == 0xFF) {
                state = DCMotorReverse;
            } else {
                state = DCMotorIdle;
            }
            break;

        case DCMotorForward:
            state = DCMotorIdle;
            break;

        case DCMotorReverse:
            state = DCMotorIdle;
            break;

        default:
            state = DCMotorIdle;
            break;
    }

    // State Actions
    switch(state) {
        case DCMotorIdle:
            PORTD = 0x00;
            break;

        case DCMotorForward:
            PORTD = 0x08 | 0x20;
            break;

        case DCMotorReverse:
            PORTD = 0x04 | 0x10;
            break;
    }
    return state;
}

//========================================================================
int main(void) {
    DDRA = 0xF0; PORTA = 0x0F;  //  First four pins are input, last four pins are output    (Joysticks will be connected here)
    DDRD = 0xFF; PORTD = 0x00;  //  All 8 pins are output                                   (Motors will be connected here)

    ADC_init();

    unsigned long int FRJoystickTick_calc= 1;
    unsigned long int DCMotorTick_calc = 1;

    // Calculate GCD
    unsigned long int tmpGCD = findGCD(FRJoystickTick_calc, DCMotorTick_calc);

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int FRJoystickTick_period = FRJoystickTick_calc/GCD;
    unsigned long int DCMotorTick_period = DCMotorTick_calc/GCD;

    static task task1, task2;
    task *tasks[] = { &task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    //  Task 1
    task1.state = -1;
    task1.period = FRJoystickTick_period;
    task1.elapsedTime = FRJoystickTick_period;
    task1.TickFct = &FRJoystickTick;

    //  Task 2
    task2.state = -1;
    task2.period = DCMotorTick_period;
    task2.elapsedTime = DCMotorTick_period;
    task2.TickFct = &DCMotorTick;

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
