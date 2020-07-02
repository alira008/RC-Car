
#ifndef DC_MOTOR_H
#define DC_MOTOR_H

//================== Shared Variables =========================

extern unsigned char DCMotorSpeed;
extern unsigned char DCMotorDirection;

//================== DC Motor Direction State Machine ==============================

enum DCMotorDirection_States {DCMotorDirectionIdle, DCMotorDirectionForward, DCMotorDirectionReverse};

int DCMotorDirectionTick(int state) {
    // State Transitions
    switch(state) {
        case DCMotorDirectionIdle:
            if(DCMotorDirection == 0x00) {
                state = DCMotorDirectionForward;
            } else if(DCMotorDirection == 0x01) {
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

enum DCMotorSpeed_states {DCMotorSpeedIdle, DCMotorSpeedLow, DCMotorSpeedMedium, DCMotorSpeedHigh};

int DCMotorSpeedTick(int state) {
    //  State Transitions
    switch(state) {
        case DCMotorSpeedIdle:
        	if(DCMotorSpeed == 0x00) {
        		state = DCMotorSpeedIdle;
        	} else if(DCMotorSpeed == 0x01) {
        		state = DCMotorSpeedLow;
        	} else if(DCMotorSpeed == 0x02) {
        		state = DCMotorSpeedMedium;
        	} else if(DCMotorSpeed ==  0x03) {
        		state = DCMotorSpeedHigh;
        	}
        	break;

        case DCMotorSpeedLow:
        	if(DCMotorSpeed == 0x00) {
        		state = DCMotorSpeedIdle;
        	} else if(DCMotorSpeed == 0x01) {
        		state = DCMotorSpeedLow;
        	} else if(DCMotorSpeed == 0x02) {
        		state = DCMotorSpeedMedium;
        	} else if(DCMotorSpeed ==  0x03) {
        		state = DCMotorSpeedHigh;
        	}
        	break;

        case DCMotorSpeedMedium:
        	if(DCMotorSpeed == 0x00) {
        		state = DCMotorSpeedIdle;
        	} else if(DCMotorSpeed == 0x01) {
        		state = DCMotorSpeedLow;
        	} else if(DCMotorSpeed == 0x02) {
        		state = DCMotorSpeedMedium;
        	} else if(DCMotorSpeed ==  0x03) {
        		state = DCMotorSpeedHigh;
        	}
        	break;

        case DCMotorSpeedHigh:
        	if(DCMotorSpeed == 0x00) {
        		state = DCMotorSpeedIdle;
        	} else if(DCMotorSpeed == 0x01) {
        		state = DCMotorSpeedLow;
        	} else if(DCMotorSpeed == 0x02) {
        		state = DCMotorSpeedMedium;
        	} else if(DCMotorSpeed ==  0x03) {
        		state = DCMotorSpeedHigh;
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

        case DCMotorSpeedLow:
        	OCR0B = 170;
        	break;

        case DCMotorSpeedMedium:
        	OCR0B = 85;
        	break;

        case DCMotorSpeedHigh:
        	OCR0B = 0;
        	break;
    }

    return state;
}

//========================================================================

#endif