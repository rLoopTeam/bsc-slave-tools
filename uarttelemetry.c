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

//ZMQ stuff
void *context;
void *pubTelemetry;
int rc;

char *nodeName = "";
int nodeNameLength = 0;
uint8_t buffer[5000];

void recvParam(struct rI2CRX_decParam decParam);

//Called for every parameter found by rI2CRX
void recvParam(struct rI2CRX_decParam decParam)
{
	//Add the latest parameter data to the local
	//cache in RAM
	newData(decParam);
}

//Called when rI2CRX finds a valid packet before calling
//recvParam for each value in the packet
{}


//Called when rI2CRX is done decoding a valid packet
//and processed all the parameters included in it
void endFrame()
{
	//Start the beginning of the linked list
	//that has all the latest parameter data
	dataCache_t * temp = dataCacheHead;

	//Initialze a new data frame
	//that will be sent off over ZMQ
	rI2CTX_beginFrame();

	//Iterate through the linked list
	//adding all the latest data values
	//we need to rebroadcast over ZMQ
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

		//See what type of data is in the parameter and add it to the new data packet
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
		
		//Next link in the list
		temp = temp->next;
	}
	
	//Writes all the final data to the new data packet
	//such as checksum and total length
	rI2CTX_endFrame();

	//Pack the new data packet into the ZMQ transmit buffer
	//along with some metadata at the start and send it out
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

	//Grab the local node name from the file system if it exists
	//Looks a bit long winded, may be able to clean this up a bit
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

	//Open the serial port
	int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf ("error %d opening %s: %s", errno, portname, strerror (errno));
		return;
	}

	//Set the serial port data rate
	set_interface_attribs (fd, B921600);  // set speed to 921,600 bps, 8n1 (no parity)
	
	//The serial port to be required to flush everything when the interface settings are changed
	//otherwise we don't get any data when the Pi is first booted and this is run
	//as a daemon
	close(fd);
	fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	uint8_t buff;

	//Initialize the data receive process
	rI2CRX_begin();
	
	//Link the local receiving functions to the
	//appropriate callbacks
	rI2CRX_recvDecParamCB = &recvParam;
	rI2CRX_frameRXBeginCB = &gotAFrame;
	rI2CRX_frameRXEndCB = &endFrame;

	//Get the ZMQ Pub socket all setup
	context = zmq_ctx_new();
	pubTelemetry = zmq_socket (context, ZMQ_PUB);
	rc = zmq_bind (pubTelemetry, "tcp://*:3000");
	assert (rc == 0);

	//Read data from the serial port forever
	while(1){
		
		//Grab some new data
		if((length = read(fd, buffer, 20)) == -1){
			printf("unable to read!\n");
		} else if (length < 0) {
            		printf("Error from read: %d: %s\n", length, strerror(errno));
        	}

		//Pass the new data onto the RX data protocol processor
		for(count = 0; count < length; count++){
			buff = buffer[count];
			fflush(stdout);
			rI2CRX_receiveBytes(&buff,1);
		}

	}
	
	//Don't ever get here to clean up nicely
	//but it's the thought that counts
	zmq_close(pubTelemetry);
	zmq_ctx_destroy(context);
	close(fd);
	return 0;
}
