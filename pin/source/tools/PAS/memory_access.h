/*
 * memory_access.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "pin.H"
#ifndef MEMORY_ACCESS_H_
#define MEMORY_ACCESS_H_
VOID BeforeMemRead(VOID* assembly,ADDRINT ip, VOID * addr,const CONTEXT * ctxt,UINT32 size);
VOID BeforeMemWrite(VOID* assembly,ADDRINT  ip, VOID * addr,const CONTEXT * ctxt, UINT32 size);
VOID MemoryAccessInstruction(INS ins, VOID *v);

#endif /* MEMORY_ACCESS_H_ */
