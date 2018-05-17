/*
 * test.c
 *
 *  Created on: Oct 18, 2016
 *      Author: haoli
 */
#include<stdio.h>
#include<limits.h>
int main(){
	int a =INT_MAX;
	int b = INT_MIN;
	int c = 10;
	int d = 2;
	int e = 3;
	int x = a + c;
	printf("%d\n",a + c);
	printf("%d\n",b-c);
	printf("%d\n",a*d);
	printf("%d\n",a/d);
}
