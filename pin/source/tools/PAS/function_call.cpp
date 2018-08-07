/*
 * function_call.cpp
 *
 */
#include "function_call.h"
#include <cstdio>
#include <iostream>
#include "global.h"
#include <util/general.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/func_inv_record.h>
#include <io/return_record.h>
#include <io/return_addr_record.h>
#include <vaccs_read/vaccs_dw_reader.h>

extern vaccs_dw_reader *vdr;
extern FILE *vfp;
using namespace std;

VOID before_function_direct_call(ADDRINT ip, CONTEXT *ctxt, ADDRINT callee_address) {
   DEBUGL(cout << "Enter before_function_direct_call" << endl);
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
 
   DEBUGL(cout << "Call to function" << endl);
   DEBUGL(cout << '\t' << "Event num: " << dec << timestamp << endl);
   DEBUGL(cout << '\t' << "Function name: " << callee_name << endl);
   DEBUGL(cout << '\t' << "Func line: " << callee_line << endl);
   DEBUGL(cout << '\t' << "Inv line: " << line << endl);
   DEBUGL(cout << '\t' << "Func file: " << callee_file_name << endl);
   DEBUGL(cout << '\t' << "Inv file: " << fileName << endl);
   DEBUGL(cout << '\t' << "Callee address: 0x" << hex << callee_address << endl << endl);
   func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
   DEBUGL(cout << "frec = 0x" << frec << endl);
   frec = frec->add_event_num(timestamp++)
      ->add_func_name(callee_name.c_str())
      ->add_func_line_num(callee_line)
      ->add_inv_line_num(line)
      ->add_c_func_file(callee_file_name.c_str())
      ->add_c_inv_file(fileName.c_str())
      ->add_address(callee_address);

   DEBUGL(cout << "Built frec" << endl);
   frec->write(vfp);
   delete frec;
   DEBUGL(cout << "Wrote frec" << endl);


   if (callee_line != 0) { // if there is no C line, then we can't access memory
      DEBUGL(cout << "Getting frame pointer for " << callee_name << endl);
      ADDRINT frame_ptr = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
      ADDRINT dynamic_link = read_memory_as_address(frame_ptr+OLD_FRAME_PTR_OFFSET);
      ADDRINT return_address = read_memory_as_address(frame_ptr+RETURN_ADDRESS_OFFSET);
 
      DEBUGL(cout << "Return address" << endl);
      DEBUGL(cout << '\t' << "Function name: " << callee_name << endl);
      DEBUGL(cout << '\t' << "Dynamic link: 0x" << hex << dynamic_link << endl);
      DEBUGL(cout << '\t' << "Return address: 0x" << hex << return_address << endl);
      return_addr_record *rarec = ((return_addr_record*)factory
         .make_vaccs_record(VACCS_RETURN_ADDR))
         ->add_dynamic_link(dynamic_link)
         ->add_return_address(return_address)
         ->add_c_func_name(callee_name.c_str());
   
      rarec->write(vfp);
      delete rarec;
   }

   DEBUGL(cout << "Exit before_function_direct_call" << endl);
}

VOID before_function_indirect_call(ADDRINT ip, CONTEXT *ctxt, ADDRINT callee_address,BOOL taken) {
   DEBUGL(cout << "Enter before_function_indirect_call" << endl);
   if (taken)
      before_function_direct_call(ip,ctxt,callee_address);

   DEBUGL(cout << "Exit before_function_indirect_call" << endl);
}

VOID after_function_call(void) {
   DEBUGL(cout << "Enter after_function_call" << endl);
   DEBUGL(cout << "function return" << endl);
   vaccs_record_factory factory;
   return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
   rrec = rrec->add_event_num(timestamp++);
   rrec->write(vfp);
   delete rrec;
   DEBUGL(cout << "Exit after_function_call" << endl);
}

// Is called for every instruction and instruments all of them that have 
// corresponding C code and modify a register
VOID monitor_function_calls(INS ins, VOID *v)
{

   if (INS_IsCall(ins)) {
      if (INS_IsDirectBranchOrCall(ins)) {
         DEBUGL(cout << "Direct call: " << INS_Disassemble(ins) << endl);
         ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
         INS_InsertPredicatedCall(ins,IPOINT_BEFORE,AFUNPTR(before_function_direct_call),
                                 IARG_INST_PTR,
                                 IARG_CONTEXT,
                                 IARG_ADDRINT, target,
                                 IARG_END);
      } else {
         DEBUGL(cout << "Indirect call: " << INS_Disassemble(ins) << endl);
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
