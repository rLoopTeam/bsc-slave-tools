CC=g++

all: i2ccat setParameter

i2ccat:
	$(CC) rPodI2C.c i2ccat.c -o i2ccat
	
setParameter:
	$(CC) rI2CTX.cpp setParameter.cpp -o i2cSetParameter
	