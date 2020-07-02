#ifndef ADC_ATMEGA1284_H
#define ADC_ATMEGA1284_H

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

void ServoPWM_init() {
    // set PWM output for servo 
    // Fast PWM mode
    DDRB |= (1 << PB6);     // Set OC3A to output
    //  Servo motor must receive constant 50 MHz
    //  target frequency (50 Mhz) = 1 Mhz / (prescaler * (1 + TOP))
    TCNT3 = 0;
    ICR3 = 2499;    // This is our TOP value
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << WGM32) | (1 << WGM33) | (1 << CS31) | (1 << CS30);   // Here we set our prescaler to 64
}

#endif