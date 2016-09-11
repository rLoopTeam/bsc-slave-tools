CC=g++

all: uarttelemetry

uarttelemetry:
	$(CC) rI2CRX.c rI2CTX.c rPodSerialPort.c rPodReceivedData.c uarttelemetry.c -lzmq -o uarttelemetry
	
setParameter:
	$(CC) rI2CTX.cpp setParameter.cpp -o i2cSetParameter
	
