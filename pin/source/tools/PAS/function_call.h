/*
 * memory_access.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "pin.H"


VOID before_function_call(ADDRINT ip, ADDRINT addr);
VOID after_function_call(VOID * function_name, const CONTEXT * ctxt, ADDRINT ip);
VOID monitor_function_calls(INS ins, VOID *fn, VOID *v);

#endif /* FUNCTION_CALL_H */
