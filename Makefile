CC=g++

all: i2ccat i2ccatChar i2ccatHex setParameter

i2ccat:
	$(CC) rPodI2C.c i2ccat.c -o i2ccat

i2ccatChar:
	$(CC) rPodI2C.c i2cChar.c -o i2ccatChar
	
i2ccatHex:
	$(CC) rPodI2C.c i2ccatHex.c -o i2ccatHex
		
setParameter:
	$(CC) rI2CTX.cpp setParameter.cpp -o i2cSetParameter
	