/*
 * functionInvocation.h
 *
 *  Created on: Aug 29, 2016
 *      Author: haoli
 */
#include "pin.H"


#ifndef FUNCTIONINVOCATION_H_
#define FUNCTIONINVOCATION_H_
VOID functionInvocationBefore(void* function_name,const CONTEXT* contex,ADDRINT ip);
VOID functionInvocationAfter(void* function_name,const CONTEXT* ctxt, ADDRINT ip);
VOID FunctionInvocatioinImage(IMG img, VOID *v);

#endif /* FUNCTIONINVOCATION_H_ */
