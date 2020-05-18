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

static list<bool> func_is_user_code;
list<ADDRINT> call_inst_ip;

VOID
before_function_call(ADDRINT ip, ADDRINT addr)
{

  INT32 column;
  INT32 line;
  string fileName;

  PIN_LockClient();
  PIN_GetSourceLocation(ip, &column, &line, &fileName);
  PIN_UnlockClient();

  call_inst_ip.push_front(ip);
  if (line != 0) {
    PIN_LockClient();
    PIN_GetSourceLocation(addr, &column, &line, &fileName);
    PIN_UnlockClient();
    DEBUGL(LOG("Function call to address: " + hexstr(addr) + " ip = " + hexstr(ip) + "\n"));


    if (line != 0) {
      func_is_user_code.push_front(true);
      DEBUGL(LOG("Function call to user code: line = " + decstr(line) + "\n"));
    } else
      func_is_user_code.push_front(false);
  } else
    func_is_user_code.push_front(false);
}

VOID
after_function_call(VOID * function_name, const CONTEXT * ctxt, ADDRINT ip)
{
  string fstr((char*)function_name);

  DEBUGL(LOG("In after_function_call in " + fstr + ", ip = " + hexstr(ip) + "\n"));
  INT32 column, line;
  string fileName;

  PIN_LockClient();
  PIN_GetSourceLocation(ip, &column, &line, &fileName);
  PIN_UnlockClient();

  if (line != 0 && !func_is_user_code.front()) {

    // after a function call to a non local function, check for updated variables

    DEBUGL(LOG("Looking for updates after a library function call\n"));
    cu_table * cutab = vdr->get_cutab();

    list<var_upd_record *> * variables = stack_model->get_all_updated_variables(cutab);
    variables->splice(variables->end(), *stack_model->get_all_updated_points_to(cutab));

    for (list<var_upd_record *>::iterator it = variables->begin(); it != variables->end(); it++) {
      var_upd_record * vurec = *it;
      DEBUGL(LOG("Found variable updated after call in " + fstr + ":" + vurec->get_variable_name()));
      vurec->write(vaccs_fd, fileName, line, cutab, timestamp);
    }

    list<return_addr_record *> * ralist = stack_model->get_updated_links(fileName,line);
    for (list<return_addr_record *>::iterator it = ralist->begin(); it != ralist->end(); it++) {
      return_addr_record * rarec = *it;
      DEBUGL(LOG("Found link updated after call in " + fstr + ", timestamp = " + decstr(timestamp) + "\n"));
      rarec->write(vaccs_fd);
    }
    timestamp++;
  }
  func_is_user_code.pop_front();
  call_inst_ip.pop_front();
}

// Is called for every instruction and instruments all of them that have
// corresponding C code and modify a register
VOID
monitor_function_calls(INS ins, VOID * fn, VOID * v)
{
  if (INS_IsDirectCall(ins)) {
    OPCODE opc = INS_Opcode(ins);
    string op = OPCODE_StringShort(opc);
    INS next_ins = INS_Next(ins);
    if (op.compare("CALL_NEAR") == 0 && INS_Valid(next_ins)) {
      // Only instrument calls that have a place to return (an exit() at the end of a function does not)
      DEBUGL(LOG("Direct call: " + INS_Disassemble(ins) + "\n"));
      ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
      INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)before_function_call,
                     IARG_INST_PTR, IARG_ADDRINT, target, IARG_END);

      INS_InsertCall(next_ins, IPOINT_BEFORE, (AFUNPTR)after_function_call,
                     IARG_PTR, fn, IARG_CONST_CONTEXT, IARG_INST_PTR, IARG_END);
    }
  }
}
