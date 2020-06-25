//	Author: ariel
//#define F_CPU 8000000UL
#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"


//=================== ADC Functions ====================================

void ADC_init() {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void ServoPWM_init() {
    // set PWM output for servo 
    // Fast PWM mode
    DDRB |= (1 << PB6);     // Set OC3A to output
    //  Servo motor must receive constant 50 MHz
    //  target frequency (50 Mhz) = 1 Mhz / (prescaler * (1 + TOP))
    TCNT3 = 0;
    ICR3 = 2499;    // This is our TOP value
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << WGM32) | (1 << WGM33) | (1 << CS31);   // Here we set our prescaler to 64
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

//=================Servo Motor State Machine============================

enum ServoMotor_States {ServoMotorCheckInput};

int ServoMotorTick(int state) {
    unsigned short LRInput = ADC_read(0);
    // State Transitions
    switch(state) {
        case ServoMotorCheckInput:
            state = ServoMotorCheckInput;
            break;
        default:
            state = ServoMotorCheckInput;
            break;
    }

    // State Actions
    switch(state) {
        case ServoMotorCheckInput:
            OCR3A = 50 + (LRInput/4.0);
            break;
    }
    return state;
}

//======================================================================

int main(void) {

    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    
    ADC_init();
    ServoPWM_init();

    unsigned long int ServoMotorTick_calc = 1;

    // Calculate GCD
    unsigned long int tmpGCD = ServoMotorTick_calc;

    unsigned long int GCD = tmpGCD;

    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int ServoMotorTick_period = ServoMotorTick_calc/GCD;

    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    //  Task 1
    task1.state = -1;
    task1.period = ServoMotorTick_period;
    task1.elapsedTime = ServoMotorTick_period;
    task1.TickFct = &ServoMotorTick;

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
