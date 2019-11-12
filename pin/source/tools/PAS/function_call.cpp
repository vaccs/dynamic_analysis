/*
 * function_call.cpp
 *
 */
#include "function_call.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <list>
#include <iostream>
#include <unistd.h>
#include <iostream>
#include "global.h"
#include <util/general.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/func_inv_record.h>
#include <io/return_record.h>
#include <io/return_addr_record.h>
#include <io/output_record.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <tables/frame.h>

extern NATIVE_FD vaccs_stdout;
extern runtime_stack * stack_model;

bool func_is_user_code;

VOID
before_function_call(ADDRINT ip, ADDRINT addr)
{

	INT32 column;
	INT32 line;
	string fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip, &column, &line, &fileName);
	PIN_UnlockClient();

	func_is_user_code = false;
	if (line != 0) {
		PIN_LockClient();
		PIN_GetSourceLocation(addr, &column, &line, &fileName);
		PIN_UnlockClient();
		DEBUGL(LOG("Function call to address: " + hexstr(addr) + " ip = " + hexstr(ip) + "\n"));


		if (line != 0) {
			func_is_user_code = true;
			DEBUGL(LOG("Function call to user code: line = " + decstr(line) + "\n"));
		}
	}
}

VOID
after_function_call(VOID * function_name, const CONTEXT * ctxt, ADDRINT ip)
{
	DEBUGL(LOG("In after_function_call\n"));
	string fstr((char*)function_name);
	INT32 column, line;
	string fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip, &column, &line, &fileName);
	PIN_UnlockClient();

	if (line != 0 && !func_is_user_code) {

		// after a function call to a non local function, check for updated variables

    DEBUGL(LOG("Looking for updates after a library function call\n"));
		cu_table * cutab = vdr->get_cutab();

		list<var_upd_record *> * variables = stack_model->get_all_updated_variables(cutab);
		variables->splice(variables->end(), *stack_model->get_all_updated_points_to(cutab));

		for (list<var_upd_record *>::iterator it = variables->begin(); it != variables->end(); it++) {
			var_upd_record * vurec = *it;
			DEBUGL(LOG("Found variable updated after call in " + fstr + ":" + vurec->get_variable_name()));
			vurec->write(vaccs_fd, fstr, line, cutab, timestamp);
		}

		list<return_addr_record *> * ralist = stack_model->get_updated_links();
		for (list<return_addr_record *>::iterator it = ralist->begin(); it != ralist->end(); it++) {
			return_addr_record * rarec = *it;
			DEBUGL(LOG("Found link updated after call in " + fstr));
			rarec->write(vaccs_fd);
		}
	}
}

// Is called for every instruction and instruments all of them that have
// corresponding C code and modify a register
VOID
monitor_function_calls(INS ins, VOID * fn, VOID * v)
{
	if (INS_IsDirectCall(ins)) {
		OPCODE opc = INS_Opcode(ins);
		string op = OPCODE_StringShort(opc);
		if (op.compare("CALL_NEAR") == 0) {
			DEBUGL(LOG("Direct call: " + INS_Disassemble(ins) + "\n"));
			ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)before_function_call,
			               IARG_INST_PTR, IARG_ADDRINT, target, IARG_END);

			IPOINT where = IPOINT_AFTER;
			if (!INS_HasFallThrough(ins))
				where = IPOINT_TAKEN_BRANCH;
			INS_InsertPredicatedCall(ins, where, (AFUNPTR)after_function_call,
			                         IARG_PTR, fn, IARG_CONTEXT, IARG_INST_PTR, IARG_END);
		}
	}
}
