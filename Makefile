all: i2ccat setParameter

i2ccat:
	gcc rPodI2C.c i2ccat.c -o i2ccat
	
setParameter:
	g++ rI2CTX.cpp setParameter.cpp -o i2cSetParameter
	