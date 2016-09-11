#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>
#include "rI2CRX.h"
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <netinet/in.h>
#include <endian.h>
#include <inttypes.h>
#include <stdint.h>
#include "rPodSerialPort.h"
#include "rPodReceivedData.h"

void recvParam(struct rI2CRX_decParam decParam);

void recvParam(struct rI2CRX_decParam decParam)
{
	newData(decParam);
}

void gotAFrame()
{}

void endFrame()
{
	dataCache_t * temp = dataCacheHead;

	while(temp != NULL)
	{
		printf("%d ",temp->decParam.index);
		switch(temp->decParam.type)
		{
			case rI2C_INT8: printf("\tINT8 %d\n",*(int8_t*)(temp->decParam.val));break;
			case rI2C_UINT8: printf("\tUINT8 %d\n",*(uint8_t*)(temp->decParam.val));break;
			case rI2C_INT16: printf("\tINT16 %d\n",*(int16_t*)(temp->decParam.val));break;
			case rI2C_UINT16: printf("\tUINT16 %d\n",*(uint16_t*)(temp->decParam.val));break;
			case rI2C_INT32: printf("\tINT32 %d\n",*(int32_t*)(temp->decParam.val));break;
			case rI2C_UINT32: printf("\tUINT32 %d\n",*(uint32_t*)(temp->decParam.val));break;
			case rI2C_INT64: printf("\tINT64 %"PRId64"\n",*(int64_t*)(temp->decParam.val));break;
			case rI2C_UINT64: printf("\tUINT64 %"PRId64"\n",*(uint64_t*)(temp->decParam.val));break;
			case rI2C_FLOAT: printf("\tFLOAT %f\n",*(float*)(temp->decParam.val));break;
			case rI2C_DOUBLE: printf("\tDOUBLE %f\n",*(double*)(temp->decParam.val));break;
		}
		temp = temp->next;
	}
}

int main(void){

	char buffer[20];
	int length, tx_length = 2;
	int count, value_count, transfered;
	char *pointer;
	int output_end = 0;

	char *portname = "/dev/ttyAMA0";

	int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf ("error %d opening %s: %s", errno, portname, strerror (errno));
		return;
	}

	set_interface_attribs (fd, B921600, 0);  // set speed to 921,600 bps, 8n1 (no parity)
	set_blocking (fd, 0);                // set no blocking

	uint8_t buff;

	rI2CRX_begin();

	rI2CRX_recvDecParamCB = &recvParam;
	rI2CRX_frameRXBeginCB = &gotAFrame;
	rI2CRX_frameRXEndCB = &endFrame;

	while(1){
		if((length = read(fd, buffer, 20)) == -1){
			printf("unable to read!\n");
		}

		for(count = 0; count < length; count++){
			buff = buffer[count];
			rI2CRX_receiveBytes(&buff,1);
		}

	}

	close(fd);
	return 0;
}
