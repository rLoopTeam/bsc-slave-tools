#include "rI2CRX.h"
#include <stdio.h>

#ifndef rPodReceivedData_H
#define rPodReceivedData_H

void newData(struct rI2CRX_decParam decParam);

typedef struct dataCache {
	struct rI2CRX_decParam decParam;
	struct dataCache * next;
} dataCache_t;

extern dataCache_t * dataCacheHead;

#endif 
