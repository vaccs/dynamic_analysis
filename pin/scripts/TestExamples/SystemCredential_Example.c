/*
 * SystemCredential.c
 *
 *  Created on: Nov 17, 2016
 *      Author: haoli
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>



int main()
{

	int f = 1;
	setgid(f);

	setregid(3,2);

	setresgid(5);

	setresuid(7);

	setreuid(9,2);

}
