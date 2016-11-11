#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>
#include "rI2CRX.h"
#include "rI2CTX.h"
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <netinet/in.h>
#include <endian.h>
#include <inttypes.h>
#include <stdint.h>
#include "rPodSerialPort.h"
#include "rPodReceivedData.h"
#include <zmq.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "teensyByteOrder.h"

//ZMQ stuff
void *context;
void *pubTelemetry;
int rc;

char *nodeName = "";
int nodeNameLength = 0;
uint8_t buffer[5000];

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

	rI2CTX_beginFrame();

	while(temp != NULL)
	{
/*
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
		}*/

		switch(temp->decParam.type)
		{
			case rI2C_INT8: rI2CTX_addParameter_int8(temp->decParam.index, *(int8_t*)(temp->decParam.val));break;
			case rI2C_UINT8: rI2CTX_addParameter_uint8(temp->decParam.index,*(uint8_t*)(temp->decParam.val));break;
			case rI2C_INT16: rI2CTX_addParameter_int16(temp->decParam.index,*(int16_t*)(temp->decParam.val));break;
			case rI2C_UINT16: rI2CTX_addParameter_uint16(temp->decParam.index,*(uint16_t*)(temp->decParam.val));break;
			case rI2C_INT32: rI2CTX_addParameter_int32(temp->decParam.index,*(int32_t*)(temp->decParam.val));break;
			case rI2C_UINT32: rI2CTX_addParameter_uint32(temp->decParam.index,*(uint32_t*)(temp->decParam.val));break;
			case rI2C_INT64: rI2CTX_addParameter_int64(temp->decParam.index,*(int64_t*)(temp->decParam.val));break;
			case rI2C_UINT64: rI2CTX_addParameter_uint64(temp->decParam.index,*(uint64_t*)(temp->decParam.val));break;
			case rI2C_FLOAT: rI2CTX_addParameter_float(temp->decParam.index,*(float*)(temp->decParam.val));break;
			case rI2C_DOUBLE: rI2CTX_addParameter_double(temp->decParam.index,*(double*)(temp->decParam.val));break;
		}
		temp = temp->next;
	}
	rI2CTX_endFrame();

	if((rI2CTX_bufferPos + strlen(nodeName) + strlen(nodeName)) < sizeof(buffer)){
		char * t = "telemetry ";
		memcpy(&buffer,t,strlen(t));
		memcpy(buffer + strlen(t),nodeName,strlen(nodeName));
		memcpy(buffer + strlen(t) + strlen(nodeName), &rI2CTX_buffer,rI2CTX_bufferPos + strlen(t) + strlen(nodeName));
		zmq_send(pubTelemetry, buffer, rI2CTX_bufferPos+strlen(t)+strlen(nodeName), 0);
	}else{
		printf("ZMQ TX buffer too small.\n");
	}


}

int main(void){

	char buffer[20];

	//Check if there's a config file specifying this node's name

	if(access("/mnt/data/config/nodename",F_OK) != -1){
		int input_fd = open("/mnt/data/config/nodename", O_RDONLY);
		char nameBuffer[50];
		char inputBuffer;
		int nameLength = 0;
		while(read(input_fd, &inputBuffer, 1)>0 && inputBuffer >= 32 && inputBuffer <= 126)
			nameBuffer[nameLength++] = inputBuffer;
		nodeNameLength = nameLength-1;
		nodeName = nameBuffer;
	}

	int length, tx_length = 2;
	int count, value_count, transfered;
	char *pointer;
	int output_end = 0;

	char *portname = "/dev/ttyAMA0";
	
	memcpy(&buffer, nodeName, sizeof(nodeName));

	int fd = open (portname, O_RDONLY | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf ("error %d opening %s: %s", errno, portname, strerror (errno));
		return -20;
	}

	set_interface_attribs (fd, B57600);  // set speed to 921,600 bps, 8n1 (no parity)
	
	//Seems to be required to flush everything when the interface settings are changed
	//otherwise we don't get any data when the Pi is first booted and this is run
	//as a daemon
	close(fd);
	sleep(2);
	fd = open (portname, O_RDONLY | O_NOCTTY | O_SYNC);
	tcflush(fd,TCIFLUSH);

	uint8_t buff;

	rI2CRX_begin();

	rI2CRX_recvDecParamCB = &recvParam;
	rI2CRX_frameRXBeginCB = &gotAFrame;
	rI2CRX_frameRXEndCB = &endFrame;

	context = zmq_ctx_new();
	pubTelemetry = zmq_socket (context, ZMQ_PUB);
	rc = zmq_bind (pubTelemetry, "tcp://*:3000");
	assert (rc == 0);

	while(1){
		if((length = read(fd, buffer, 20)) == -1){
			printf("unable to read!\n");
		} else if (length < 0) {
            		printf("Error from read: %d: %s\n", length, strerror(errno));
        	}

		for(count = 0; count < length; count++){
			buff = buffer[count];
			fflush(stdout);
			rI2CRX_receiveBytes(&buff,1);
		}

	}

	zmq_close(pubTelemetry);
	zmq_ctx_destroy(context);
	close(fd);
	return 0;
}
