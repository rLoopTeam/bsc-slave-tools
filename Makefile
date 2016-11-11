CC=gcc

all: uarttelemetry uartSetParameter

uarttelemetry:
	$(CC) rI2CRX.c rI2CTX.c rPodSerialPort.c rPodReceivedData.c uarttelemetry.c -lzmq -o uarttelemetry
	
uartSetParameter:
	$(CC) rI2CRX.c rI2CTX.c rPodSerialPort.c uartSetParameter.c -lzmq -o uartSetParameter
	
