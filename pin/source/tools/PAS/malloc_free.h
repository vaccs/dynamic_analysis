/*
 * malloc_free.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef MALLOC_FREE_H_
#define MALLOC_FREE_H_

#include "pin.H"
#include <tables/heap.h>

extern HeapMap *heapMap;

VOID freeBefore(CHAR * name, ADDRINT size);
VOID mallocBefore(CHAR * name, ADDRINT size);
VOID malloc_after(ADDRINT ret);
VOID MallocAndFreeImage(IMG img, VOID *v);

#endif /* MALLOC_FREE_H_ */
