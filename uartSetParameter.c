#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "rI2CRX.h"
#include "rI2CTX.h"
#include <errno.h>
#include <string.h>
#include <termios.h>
#include "rPodSerialPort.h"
#include <zmq.h>


#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <endian.h>


void recvParam(struct rI2CRX_decParam decParam);

void recvParam(struct rI2CRX_decParam decParam)
{
/*
	printf("%d ",decParam.index);
	switch(decParam.type)
	{
		case rI2C_INT8: printf("\tINT8 %d\n",*(int8_t*)(decParam.val));break;
		case rI2C_UINT8: printf("\tUINT8 %d\n",*(uint8_t*)(decParam.val));break;
		case rI2C_INT16: printf("\tINT16 %d\n",*(int16_t*)(decParam.val));break;
		case rI2C_UINT16: printf("\tUINT16 %d\n",*(uint16_t*)(decParam.val));break;
		case rI2C_INT32: printf("\tINT32 %d\n",*(int32_t*)(decParam.val));break;
		case rI2C_UINT32: printf("\tUINT32 %d\n",*(uint32_t*)(decParam.val));break;
		case rI2C_INT64: printf("\tINT64 %"PRId64"\n",*(int64_t*)(decParam.val));break;
		case rI2C_UINT64: printf("\tUINT64 %"PRId64"\n",*(uint64_t*)(decParam.val));break;
		case rI2C_FLOAT: printf("\tFLOAT %f\n",*(float*)(decParam.val));break;
		case rI2C_DOUBLE: printf("\tDOUBLE %f\n",*(double*)(decParam.val));break;
	}*/
	switch(decParam.type)
	{
		case rI2C_INT8: rI2CTX_addParameter_int8(decParam.index, *(int8_t*)(decParam.val));break;
		case rI2C_UINT8: rI2CTX_addParameter_uint8(decParam.index,*(uint8_t*)(decParam.val));break;
		case rI2C_INT16: rI2CTX_addParameter_int16(decParam.index,*(int16_t*)(decParam.val));break;
		case rI2C_UINT16: rI2CTX_addParameter_uint16(decParam.index,*(uint16_t*)(decParam.val));break;
		case rI2C_INT32: rI2CTX_addParameter_int32(decParam.index,*(int32_t*)(decParam.val));break;
		case rI2C_UINT32: rI2CTX_addParameter_uint32(decParam.index,*(uint32_t*)(decParam.val));break;
		case rI2C_INT64: rI2CTX_addParameter_int64(decParam.index,*(int64_t*)(decParam.val));break;
		case rI2C_UINT64: rI2CTX_addParameter_uint64(decParam.index,*(uint64_t*)(decParam.val));break;
		case rI2C_FLOAT: rI2CTX_addParameter_float(decParam.index,*(float*)(decParam.val));break;
		case rI2C_DOUBLE: rI2CTX_addParameter_double(decParam.index,*(double*)(decParam.val));break;
	}
}

void gotAFrame()
{}

void endFrame()
{
	


}

int main(void){
	char *portname = "/dev/ttyAMA0";

	int fd = open (portname, O_WRONLY | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf ("error %d opening %s: %s", errno, portname, strerror (errno));
		return;
	}

	set_interface_attribs (fd, B57600);  // set speed to 921,600 bps, 8n1 (no parity)
	
	//Seems to be required to flush everything when the interface settings are changed
	//otherwise we don't get any data when the Pi is first booted and this is run
	//as a daemon
	close(fd);
	sleep(2);
	fd = open (portname, O_WRONLY | O_NOCTTY | O_SYNC);
	tcflush(fd,TCOFLUSH);

	rI2CRX_begin();

	rI2CRX_recvDecParamCB = &recvParam;
	rI2CRX_frameRXBeginCB = &gotAFrame;
	rI2CRX_frameRXEndCB = &endFrame;

    	//  Socket to receive messages on
    	void *context = zmq_ctx_new ();
    	void *receiver = zmq_socket (context, ZMQ_REP);
    	zmq_bind (receiver, "tcp://*:6789");

	uint8_t buffer2[5000];
	int recvCount; 

        //  Process tasks forever
    	while (1) {
		rI2CTX_beginFrame();
		recvCount = zmq_recv (receiver, buffer2, 5000, 0);        	
		int i;
		for(i = 0;i<recvCount;i++)
		{
			//printf("%x ",buffer2[i]);
			rI2CRX_receiveBytes(&buffer2[i],1);
		}
		//printf("\n");
		rI2CTX_endFrame();

		int wlen = write(fd,&rI2CTX_buffer,rI2CTX_bufferPos);
		if(wlen != rI2CTX_bufferPos)
			printf("Error writing to serial port %d %d.\n",wlen, errno);

		zmq_send(receiver, "Got It",6, 0);
        	fflush (stdout);
    	}

	zmq_close(receiver);
	zmq_ctx_destroy(context);
	close(fd);
	return 0;
}
