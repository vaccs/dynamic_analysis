/*
 * testSensitiveData.c
 *
 *  Created on: Feb 20, 2017
 *      Author: haoli
 */
#include <sys/mman.h>

void vaccs_sensitve_data(void* ip,char* name,unsigned int size){

}

int main(){
	int x;
	printf("x's address=%p\n",&x);
	vaccs_sensitve_data(&x,"x",4);
	mlock(&x,4);
	int y = x+1;

	return 0;

}
