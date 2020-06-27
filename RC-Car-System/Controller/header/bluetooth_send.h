#ifndef BLUETOOTH_SEND_H
#define BLUETOOTH_SEND_H

unsigned char transmitValue;

//========================= USART State Machine =============================

int TransmitDataTick(int state);

//======================= Combine Data State Machine =========================

int CombineDataTick(int state);

//============================================================================

#endif