#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "bsc-slave.h"
#include "rI2CTX.h"

#define SLV_ADDR  	 0x33

int main(int argc, char *argv[]){
	char tx_buffer[200];
	int fd;
	int tx_length = 2;
	int count, transfered;
	char *pointer;

	if ((argc - 1) % 3 != 0)
	{
		printf("Format: type index value (repeat as needed)\nType:\n");
		printf("1 = int8_t\n");
		printf("2 = uint8_t\n");
		printf("3 = int16_t\n");
		printf("4 = uint16_t\n");
		printf("5 = int64_t\n");
		printf("6 = uint64_t\n");
		printf("7 = float\n");
		printf("8 = double\n");
		return -1;
	}
	else{} //Number of parameters is correct carry on

	if ((fd = open("/dev/i2c-slave", O_RDWR)) == -1){
		printf("could not open i2c-slave\n");
	}

	if ((ioctl(fd, I2C_SLAVE, SLV_ADDR) < 0)){
		printf("failed setting slave adress!\n");
		return -1;
	}

	//Basic testing code
	/*
	rI2CTX_beginFrame();
	rI2CTX_addParameter(0, (int8_t)-25);
	rI2CTX_addParameter(1, (uint8_t)25);
	rI2CTX_addParameter(2, (int16_t)-1000);
	rI2CTX_addParameter(3, (uint16_t)3000);
	rI2CTX_addParameter(4, (int64_t)-1000);
	rI2CTX_addParameter(5, (uint64_t)1000);
	rI2CTX_addParameter(6, (float)-250.252525);
	rI2CTX_addParameter(7, (double)-250.252525);
	rI2CTX_endFrame();*/

	rI2CTX_beginFrame();

	uint8_t type;
	uint8_t index;

	int i;
	for (i = 0; i < (int)((argc - 1) / 3); i++)
	{

		type = atoi(argv[0 + 3 * i + 1]);
		index = atoi(argv[1 + 3 * i + 1]);
		switch (type)
		{
			case 1 :{
				int8_t data1;
				data1 = atoi(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data1);
				break;}
			case 2:{
				uint8_t data2;
				data2 = atoi(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data2);
				break; }
			case 3:{
				int16_t data3;
				data3 = atoi(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data3);
				break;}
			case 4:{
				uint16_t data4;
				data4 = atoi(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data4);
				break;}
			case 5:{
				int64_t data5;
				data5 = atoll(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data5);
				break;}
			case 6:{
				uint64_t data6;
				data6 = atoll(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data6);
				break;}
			case 7:{
				float data7;
				data7 = atof(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data7);
				break;}
			case 8:{
				double data8;
				data8 = atof(argv[2 + 3 * i + 1]);
				rI2CTX_addParameter(index, data8);
				break;}
			default: printf("Invalid data type.\n"); break;
		}

	}

	rI2CTX_endFrame();

	for (count = 0; count < rI2CTX_bufferPos; count++){
		tx_buffer[count] = rI2CTX_buffer[count];
	}

	tx_length = rI2CTX_bufferPos;
	pointer = tx_buffer;
	while (tx_length > 0){             //send values
		transfered = write(fd, pointer, tx_length);
		for (count = 0; count < transfered; count++){
			(int)*pointer++;
			tx_length--;
		}
	}

	printf("Sent\n");

	fsync(fd);
	close(fd);
	return 0;
}
