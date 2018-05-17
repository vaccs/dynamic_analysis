/*
 * InstructionInstrumentation.cpp
 *
 *  Created on: Aug 15, 2016
 *      Author: haoli
 */
#include "InstructionInstrumentation.h"
#include "memory_access.h"
#include "stack_operation.h"
#include "global.h"
#include <iostream>
//extern bool Is_System_Image(IMG img);;
VOID InstructionInstrumentation(IMG img, VOID *v){
	if(Is_System_Image(img)) return;
		for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
			for(RTN rtn = SEC_RtnHead(sec);RTN_Valid(rtn);rtn = RTN_Next(rtn)){
				RTN_Open(rtn);
				for(INS ins = RTN_InsHead(rtn);INS_Valid(ins);ins = INS_Next(ins)){
					MemoryAccessInstruction(ins, 0);
					StackOperationInstruction(ins, 0);
				}
				RTN_Close(rtn);
			}
		}

}
