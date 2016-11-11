#include "rPodReceivedData.h"
#include "rI2CRX.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dataCache_t * dataCacheHead = NULL;

//New data has arrived, either update the data
//in the cache or create a new slot in the cache
void newData(struct rI2CRX_decParam decParam){
	dataCache_t * temp = dataCacheHead;
	
	if(temp == NULL){
		dataCacheHead = (dataCache_t*)malloc(sizeof(dataCache_t));
		dataCacheHead->next = NULL;
		memcpy(&dataCacheHead->decParam,&decParam, sizeof(rI2CRX_decParam_t));
		void *data = malloc(decParam.length);
		memcpy(data,decParam.val,decParam.length);
		dataCacheHead->decParam.val = data;
		return;
	}

	//Search for our index, or the end of the list
	while(temp->decParam.index != decParam.index && temp->next != NULL)
		temp = temp->next;

	//found the index?
	if(temp->decParam.index == decParam.index){

		//Uh-oh, lengths don't match
		if(temp->decParam.length != decParam.length)
		{
			printf("There is a glitch in the matrix, %d %d %d %d is probably declared twice somewhere.\n",decParam.index, temp->decParam.index, decParam.length, temp->decParam.length);
			return;
		}
		
		memcpy(temp->decParam.val,decParam.val,temp->decParam.length);
		return;
	}

	//Not in our cache, got to add it
	dataCache_t * temp2 = (dataCache_t*)malloc(sizeof(dataCache_t));
	temp2->next = NULL;
	temp2->decParam.val = malloc(decParam.length);
	memcpy(temp2->decParam.val,decParam.val,decParam.length);
	temp2->decParam.length = decParam.length;
	temp2->decParam.type = decParam.type;
	temp2->decParam.index = decParam.index;
	temp->next = temp2;
}
