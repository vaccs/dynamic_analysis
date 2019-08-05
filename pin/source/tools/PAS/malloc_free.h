/*
 * malloc_free.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "pin.H"
#ifndef MALLOC_FREE_H_
#define MALLOC_FREE_H_
VOID freeBefore(CHAR * name, ADDRINT size);
VOID mallocBefore(CHAR * name, ADDRINT size);
VOID malloc_after(ADDRINT ret);
VOID MallocAndFreeImage(IMG img, VOID *v);

#endif /* MALLOC_FREE_H_ */
