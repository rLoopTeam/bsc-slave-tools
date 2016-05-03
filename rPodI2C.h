#include <stdint.h>

#ifndef TeensyI2CReceiver_H
#define TeensyI2CReceiver_H

#define I2C_BUFFER_SIZE 2000 
#define I2C_CONTROL_CHAR 0xD5
#define I2C_FRAME_START 0xD0
#define I2C_PARAMETER_START 0xD3
#define I2C_FRAME_END 0xD8


extern uint8_t buffer2[I2C_BUFFER_SIZE];
extern uint16_t bufferBegin;
extern uint16_t bufferLength;

void initReceiver();
void receiveBytes(uint8_t* data, uint16_t length);
void processBuffer();
void processFrame(uint8_t *frameBuffer, uint16_t length);

#endif 
