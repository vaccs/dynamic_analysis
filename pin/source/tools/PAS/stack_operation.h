/*
 * stack_operation.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "../PAS/vaccs_analysis.h"
#include "pin.H"
#ifndef STACK_OPERATION_H_
#define STACK_OPERATION_H_
VOID beforePush(VOID *assemly,const CONTEXT * ctxt);
VOID afterPush(const CONTEXT * ctxt);
VOID StackOperationInstruction(INS ins, VOID *v);
VOID beforePop(VOID *assemly,const CONTEXT * ctxt);
VOID afterPop(const CONTEXT * ctxt);
VOID popInstruction(INS ins, VOID *v);



#endif /* STACK_OPERATION_H_ */
