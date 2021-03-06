/*
 * memory_access.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef MONITOR_REGISTERS_H
#define MONITOR_REGISTERS_H

#include "pin.H"
#include <string>

using namespace std;

VOID AfterRegMod(ADDRINT ip, CONTEXT * ctxt, REG reg, UINT32 opcode);
VOID MonitorRegisterInstruction(INS ins, VOID *v);

#endif /* MONITOR_REGISTERS_H */
