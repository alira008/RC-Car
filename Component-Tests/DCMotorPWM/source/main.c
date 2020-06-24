//  Author: ariel

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"


//=================== ADC Functions ====================================

void ADC_init(){
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void DCPWM_init() {
    DDRB |= (1 << PB4);     // Set OC0B to output
    // Set timer counter to 0
    TCNT0 = 0;
    // We generate a phase correct PWM mode with WGM00
    // We also enable inverting mode
    TCCR0A = (1 << COM0B1) | (1 << COM0B0) | (1 << WGM00);
    // Here we set our prescaler to 1024
    TCCR0B = (1 << CS02) | (1 << CS00);
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
unsigned short FRJoystickInput;

//================== Foward/Reverse Joystick State Machine =========================

enum FRJoystick_states {FRJoystickIdle, FRJoystickForward, FRJoystickReverse};

int FRJoystickTick(int state) {
    FRJoystickInput = ADC_read(1);
    // State Transitions
    switch(state) {
        case FRJoystickIdle:
            if(FRJoystickInput > 550) {
                state = FRJoystickForward;
            } else if(FRJoystickInput < 470) {
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

//================== DC Motor Direction State Machine ==============================

enum DCMotorDirection_States {DCMotorDirectionIdle, DCMotorDirectionForward, DCMotorDirectionReverse};

int DCMotorDirectionTick(int state) {
    // State Transitions
    switch(state) {
        case DCMotorDirectionIdle:
            if(moveForward == 0xFF && moveReverse == 0x00) {
                state = DCMotorDirectionForward;
            } else if(moveForward == 0x00 && moveReverse == 0xFF) {
                state = DCMotorDirectionReverse;
            } else {
                state = DCMotorDirectionIdle;
            }
            break;

        case DCMotorDirectionForward:
            state = DCMotorDirectionIdle;
            break;

        case DCMotorDirectionReverse:
            state = DCMotorDirectionIdle;
            break;

        default:
            state = DCMotorDirectionIdle;
            break;
    }

    // State Actions
    switch(state) {
        case DCMotorDirectionIdle:
            PORTB = 0x00;
            break;

        case DCMotorDirectionForward:
            PORTB = 0x02 | 0x08;
            break;

        case DCMotorDirectionReverse:
            PORTB = 0x01 | 0x04;
            break;
    }
    return state;
}

//=================DC Motor Speed=========================================

enum DCMotorSpeed_states {DCMotorSpeedIdle, DCMotorSpeedGo};

int DCMotorSpeedTick(int state) {
    //  State Transitions
    switch(state) {
        case DCMotorSpeedIdle:
            if(FRJoystickInput > 550 || FRJoystickInput < 470) {
                state = DCMotorSpeedGo;
            }
            else {
                state = DCMotorSpeedIdle;
            }
            break;

        case DCMotorSpeedGo:
            if(FRJoystickInput > 550 || FRJoystickInput < 470) {
                state = DCMotorSpeedGo;
            }
            else {
                state = DCMotorSpeedIdle;
            }
            break;

        default:
            state = DCMotorSpeedIdle;
            break;
    }
    //  State Actions
    switch(state) {
        case DCMotorSpeedIdle:
            OCR0B = 255;
            break;

        case DCMotorSpeedGo:
            if((FRJoystickInput > 0 && FRJoystickInput <= 130) || (FRJoystickInput > 950)){
                OCR0B = 0;
            }
            else if((FRJoystickInput > 130 && FRJoystickInput <= 230) || (FRJoystickInput > 850 && FRJoystickInput <= 950)){
                OCR0B = 51;
            }
            else if((FRJoystickInput > 230 && FRJoystickInput <= 330) || (FRJoystickInput > 750 && FRJoystickInput <= 850)){
                OCR0B = 102;
            }
            else if((FRJoystickInput > 330 && FRJoystickInput <= 430) || (FRJoystickInput > 650 && FRJoystickInput <= 750)){
                OCR0B = 153;
            }
            else if((FRJoystickInput > 430) || (FRJoystickInput > 550 && FRJoystickInput <= 650)){
                OCR0B = 204;
            }
            break;
    }

    return state;
}

//========================================================================
int main(void) {
    DDRA = 0xF0; PORTA = 0x0F;  //  First four pins are input, last four pins are output    (Joysticks will be connected here)
    DDRB = 0xFF; PORTB = 0x00;  //  All 8 pins are output                                   (Motors will be connected here)

    ADC_init();
    DCPWM_init();

    unsigned long int FRJoystickTick_calc= 1;
    unsigned long int DCMotorDirectionTick_calc = 1;
    unsigned long int DCMotorSpeedTick_calc = 1;

    // Calculate GCD
    unsigned long int tmpGCD = findGCD(FRJoystickTick_calc, DCMotorDirectionTick_calc);
    tmpGCD = findGCD(tmpGCD, DCMotorSpeedTick_calc);

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int FRJoystickTick_period = FRJoystickTick_calc/GCD;
    unsigned long int DCMotorDirectionTick_period = DCMotorDirectionTick_calc/GCD;
    unsigned long int DCMotorSpeedTick_period = DCMotorSpeedTick_calc/GCD;

    static task task1, task2, task3;
    task *tasks[] = { &task1, &task2, &task3 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    //  Task 1
    task1.state = -1;
    task1.period = FRJoystickTick_period;
    task1.elapsedTime = FRJoystickTick_period;
    task1.TickFct = &FRJoystickTick;

    //  Task 2
    task2.state = -1;
    task2.period = DCMotorDirectionTick_period;
    task2.elapsedTime = DCMotorDirectionTick_period;
    task2.TickFct = &DCMotorDirectionTick;

    //  Task 3
    task3.state = -1;
    task3.period = DCMotorSpeedTick_period;
    task3.elapsedTime = DCMotorSpeedTick_period;
    task3.TickFct = &DCMotorSpeedTick;

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
