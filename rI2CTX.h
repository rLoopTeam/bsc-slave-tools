#include <stdint.h>

#ifndef rI2CTX_H
#define rI2CTX_H

#define RPOD_I2C_BUFFER_SIZE 2000 
#define RPOD_I2C_CONTROL_CHAR 0xD5
#define RPOD_I2C_FRAME_START 0xD0
#define RPOD_I2C_PARAMETER_START 0xD3
#define RPOD_I2C_FRAME_END 0xD8

extern uint8_t rI2CTX_buffer[RPOD_I2C_BUFFER_SIZE];
extern uint16_t rI2CTX_bufferPos;
extern uint16_t rI2CTX_frameLength;

void rI2CTX_beginFrame();
void rI2CTX_CalculateChecksum(uint16_t lastByte);
uint16_t rI2CTX_endFrame();

enum rI2C_paramTypes
{
	rI2C_INT8 = 0x11,
	rI2C_UINT8 = 0x12,
	rI2C_INT16 = 0x21,
	rI2C_UINT16 = 0x22,
	rI2C_INT32 = 0x41,
	rI2C_UINT32 = 0x42,
	rI2C_INT64 = 0x81,
	rI2C_UINT64 = 0x82,
	rI2C_FLOAT = 0x43,
	rI2C_DOUBLE = 0x83
};

void rI2CTX_addParameter(uint8_t index, int8_t data);
void rI2CTX_addParameter(uint8_t index, uint8_t data);
void rI2CTX_addParameter(uint8_t index, int16_t data);
void rI2CTX_addParameter(uint8_t index, uint16_t data);
void rI2CTX_addParameter(uint8_t index, int64_t data);
void rI2CTX_addParameter(uint8_t index, uint64_t data);
void rI2CTX_addParameter(uint8_t index, int32_t data);
void rI2CTX_addParameter(uint8_t index, uint32_t data);
void rI2CTX_addParameter(uint8_t index, float data);
void rI2CTX_addParameter(uint8_t index, double data);

#endif