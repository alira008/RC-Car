
#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

//========================= Shared Variables ===============================================================

extern unsigned char ServoMotorAngle;

//================= Servo Motor State Machine ==============================================================

enum ServoMotor_States {ServoMotorOutput};

int ServoMotorTick(int state) {
    // State Transitions
    switch(state) {
        case ServoMotorOutput:
            state = ServoMotorOutput;
            break;
        default:
            state = ServoMotorOutput;
            break;
    }

    // State Actions
    switch(state) {
        case ServoMotorOutput:
            //	Switch statement checking which angle was given as input
        	switch(ServoMotorAngle) {
        		//	Angle is -45
        		case 0x00:
        			OCR3A = 50;
        			break;

        		//	Angle is -30
        		case 0x01:
        			OCR3A = 72.5;
        			break;

        		//	Angle is -15
        		case 0x02:
        			OCR3A = 95;
        			break;

        		//	Angle is 0
        		case 0x03:
        			OCR3A = 117.5;
        			break;

        		//	Angle is +15
        		case 0x04:
        			OCR3A = 140;
        			break;

        		//	Angle is +30
        		case 0x05:
        			OCR3A = 162.5;
        			break;

        		//	Angle is +45
        		case 0x06:
        			OCR3A = 185;
        			break;

        		//	Default to angle 0
        		default:
        			OCR3A = 185;
        			break;
        	}
            break;
    }
    return state;
}

//==========================================================================================================

#endif