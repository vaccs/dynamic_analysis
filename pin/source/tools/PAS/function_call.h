/*
 * memory_access.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "pin.H"


VOID before_function_direct_call(ADDRINT ip, ADDRINT callee_address);
VOID before_function_indirect_call(ADDRINT ip, ADDRINT callee_address, BOOL taken);
VOID after_function_call(void);
VOID monitor_function_calls(INS ins, VOID *v);

#endif /* FUNCTION_CALL_H */
