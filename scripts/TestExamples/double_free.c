/*
 * double_free.c
 *
 *  Created on: Oct 11, 2016
 *      Author: haoli
 */
 #include <stdlib.h>
int main(){
	char * p = (char*) malloc(100000);
	char* q;
	free(p);
	free(q);
	free(p);
//	char * q = (char*) malloc(100);
//	free(p);
//	free(q);
}
