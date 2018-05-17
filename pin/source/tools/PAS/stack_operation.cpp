/*
 * stack_operation.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "stack_operation.h"
#include "database_access_api.h"
#include <iostream>
#include <fstream>
#include "global.h"


/* ===================================================================== */
/* instrumentation for push and pop instruction                          */
/* ===================================================================== */

VOID beforePush(VOID *assembly,const CONTEXT * ctxt){
	char* assembly_code = (char*) assembly;
	ADDRINT BeforeSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);
	std::cout << "Before push: SP = " << hex << BeforeSP << dec << endl;
	push_event.frame_pointer = current_EBP;
	push_event.id_function_invocation_happened_in = current_invocation_id;
	push_event.id = timestamp++;
	get_registers(ctxt,push_event.id);
	push_event.sp_before = BeforeSP;
	ou_push <<dec<<push_event.id<<","<<assembly_code<<","<< hex<<push_event.sp_before<<",";


}
VOID afterPush(const CONTEXT * ctxt){
	ADDRINT AfterSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);
	ADDRINT EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
	std::cout << "After push: SP = " << hex << AfterSP << dec << endl;
	push_event.sp_after = AfterSP;
	store_push_transaction(push_event);
	ou_push << hex <<push_event.sp_after<<","<<dec<<push_event.id_function_invocation_happened_in<<","<<hex<<EBP<<std::endl;
}
/* ===================================================================== */
/* Analysis routine for push instruction                                 */
/* ===================================================================== */
VOID StackOperationInstruction(INS ins, VOID *v){
	OPCODE opc = INS_Opcode	(ins);
	std::string op = OPCODE_StringShort(opc);
	std::string push("PUSH");
	std::string pop("POP");
	std::string function_name = RTN_Name(INS_Rtn(ins));
	char* assembly_code = (char*) malloc(strlen(INS_Disassemble(ins).c_str())+1);
	strcpy(assembly_code,INS_Disassemble(ins).c_str());
	std::cout<<assembly_code<<std::endl;
	if(op.compare(push)==0){
		INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)beforePush,
									IARG_PTR,assembly_code,
		                            IARG_CONTEXT, IARG_END);
		INS_InsertCall( ins, IPOINT_AFTER, (AFUNPTR)afterPush,
				                            IARG_CONTEXT,
				                         IARG_END);
	}
	if(op.compare(pop)==0){
		INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)beforePop,
											IARG_PTR,assembly_code,
				                            IARG_CONTEXT, IARG_END);
				INS_InsertCall( ins, IPOINT_AFTER, (AFUNPTR)afterPop,
						                            IARG_CONTEXT, IARG_END);
	}



}
/* ===================================================================== */
/* instrumentation for pop instruction                                   */
/* ===================================================================== */
VOID beforePop(VOID *assembly,const CONTEXT * ctxt){
	char* assembly_code = (char*) assembly;
	ADDRINT BeforeSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);
	std::cout << "Before pop: SP = " << hex << BeforeSP << dec << endl;
	pop_event.frame_pointer = current_EBP;
	pop_event.id = timestamp++;
	get_registers(ctxt,pop_event.id);
	pop_event.id_function_invocation_happened_in = current_invocation_id;
	pop_event.sp_before = BeforeSP;
	ou_pop<<dec<<pop_event.id<<","<<assembly_code<<","<<hex<<pop_event.sp_before<<",";
}
VOID afterPop(const CONTEXT * ctxt){
	ADDRINT AfterSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);
   std::cout << "After pop: SP = " << hex << AfterSP << dec << endl;
   pop_event.sp_after = AfterSP;
   store_pop_transaction(pop_event);
   ou_pop<<hex<<pop_event.sp_after<<","<<dec<<pop_event.id_function_invocation_happened_in<<std::endl;

}

