/*
 * function_call.cpp
 *
 */
#include "function_call.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
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

VOID before_function_direct_call(ADDRINT ip, CONTEXT *ctxt, ADDRINT callee_address) {
   DEBUGL(LOG("Enter before_function_direct_call\n"));
	INT32	column, callee_column;
	INT32	line, callee_line;
	std::string 	fileName, callee_file_name, callee_name;

	PIN_LockClient();
	PIN_GetSourceLocation(ip,&column,&line,&fileName);
	PIN_GetSourceLocation(callee_address,&callee_column,&callee_line,&callee_file_name);
	PIN_UnlockClient();

   if (line == 0)
      fileName = NOCSOURCE;
   if (callee_line == 0) {
      callee_file_name = NOCSOURCE;
      callee_name = NOFUNCNAME;
   }
   else 
      callee_name = vdr->get_cutab()->translate_address_to_function(callee_address).first;

   vaccs_record_factory factory;
 
   DEBUGL(LOG("Call to function\n"));
   DEBUGL(LOG("\tEvent num: " + decstr(timestamp) + "\n"));
   DEBUGL(LOG("\tFunction name: " + callee_name + "\n"));
   DEBUGL(LOG("\tFunc line: " + decstr(callee_line) + "\n"));
   DEBUGL(LOG("\tInv line: " + decstr(line) + "\n"));
   DEBUGL(LOG("\tFunc file: " + callee_file_name + "\n"));
   DEBUGL(LOG("\tInv file: " + fileName + "\n"));
   DEBUGL(LOG("\tCallee address: 0x" + hexstr(callee_address) + "\n\n"));
   func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
   DEBUGL(LOG("frec = 0x" + hexstr(frec) + "\n"));
   frec = frec->add_event_num(timestamp++)
      ->add_func_name(callee_name.c_str())
      ->add_func_line_num(callee_line)
      ->add_inv_line_num(line)
      ->add_c_func_file(callee_file_name.c_str())
      ->add_c_inv_file(fileName.c_str())
      ->add_address(callee_address);

   DEBUGL(LOG("Built frec\n"));
   frec->write(vaccs_fd);
   delete frec;
   DEBUGL(LOG("Wrote frec\n"));


   if (callee_line != 0) { // if there is no C line, then we can't access memory
      DEBUGL(LOG("Getting frame pointer for " + callee_name + "\n"));
      ADDRINT frame_ptr = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
      ADDRINT dynamic_link = read_memory_as_address(frame_ptr+OLD_FRAME_PTR_OFFSET);
      ADDRINT return_address = read_memory_as_address(frame_ptr+RETURN_ADDRESS_OFFSET);
 
      DEBUGL(LOG("Return address\n"));
      DEBUGL(LOG("\tFunction name: " + callee_name + "\n"));
      DEBUGL(LOG("\tDynamic link: 0x" + hexstr(dynamic_link) + "\n"));
      DEBUGL(LOG("\tReturn address: 0x" + hexstr(return_address) + "\n"));
      return_addr_record *rarec = ((return_addr_record*)factory
         .make_vaccs_record(VACCS_RETURN_ADDR))
         ->add_dynamic_link(dynamic_link)
         ->add_return_address(return_address)
         ->add_c_func_name(callee_name.c_str());
   
      rarec->write(vaccs_fd);
      delete rarec;
   }

   DEBUGL(LOG("Exit before_function_direct_call\n"));
}

VOID before_function_indirect_call(ADDRINT ip, CONTEXT *ctxt, ADDRINT callee_address,BOOL taken) {
   DEBUGL(LOG("Enter before_function_indirect_call\n"));
   if (taken)
      before_function_direct_call(ip,ctxt,callee_address);

   DEBUGL(LOG("Exit before_function_indirect_call\n"));
}

VOID after_function_call(void) {
   DEBUGL(LOG("Enter after_function_call\n"));

   vaccs_record_factory factory;
   return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
   rrec = rrec->add_event_num(timestamp++);
   rrec->write(vaccs_fd);
   delete rrec;

   DEBUGL(LOG("function return\n"));
   DEBUGL(LOG("Exit after_function_call\n"));
}

// Is called for every instruction and instruments all of them that have 
// corresponding C code and modify a register
VOID monitor_function_calls(INS ins, VOID *v)
{

   if (INS_IsCall(ins)) {
      if (INS_IsDirectBranchOrCall(ins)) {
         DEBUGL(LOG("Direct call: " + INS_Disassemble(ins) + "\n"));
         ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
         INS_InsertPredicatedCall(ins,IPOINT_BEFORE,AFUNPTR(before_function_direct_call),
                                 IARG_INST_PTR,
                                 IARG_CONTEXT,
                                 IARG_ADDRINT, target,
                                 IARG_END);
      } else {
         DEBUGL(LOG("Indirect call: " + INS_Disassemble(ins) + "\n"));
         INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(before_function_indirect_call),
                        IARG_INST_PTR,
                        IARG_CONTEXT,
                        IARG_BRANCH_TARGET_ADDR,
                        IARG_END);
      }
	   IPOINT where = IPOINT_AFTER;
	   if (!INS_HasFallThrough(ins))
	      where = IPOINT_TAKEN_BRANCH;
	   INS_InsertPredicatedCall(ins, where, (AFUNPTR)after_function_call,
		                        IARG_END);
   }
}
