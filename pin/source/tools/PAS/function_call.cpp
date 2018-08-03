/*
 * memory_access.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "function_call.h"
#include <cstdio>
#include <iostream>
#include "global.h"
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/func_inv_record.h>
#include <io/return_record.h>
#include <vaccs_read/vaccs_dw_reader.h>

extern vaccs_dw_reader *vdr;
extern FILE *vfp;
using namespace std;

VOID before_function_direct_call(ADDRINT ip, ADDRINT callee_address) {
   cout << "Enter before_function_direct_call" << endl;
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
 
   cout << "Call to function" << endl;
   cout << '\t' << "Event num: " << dec << timestamp << endl;
   cout << '\t' << "Function name: " << callee_name << endl;
   cout << '\t' << "Func line: " << callee_line << endl;
   cout << '\t' << "Inv line: " << line << endl;
   cout << '\t' << "Func file: " << callee_file_name << endl;
   cout << '\t' << "Inv file: " << fileName << endl;
   cout << '\t' << "Callee address: 0x" << hex << callee_address << endl << endl;
   func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
   cout << "frec = 0x" << frec << endl;
   frec = frec->add_event_num(timestamp++)
      ->add_func_name(callee_name.c_str())
      ->add_func_line_num(callee_line)
      ->add_inv_line_num(line)
      ->add_c_func_file(callee_file_name.c_str())
      ->add_c_inv_file(fileName.c_str())
      ->add_address(callee_address);

   cout << "Built frec" << endl;
   frec->write(vfp);
   cout << "Wrote frec" << endl;
   delete frec;
   cout << "Exit before_function_direct_call" << endl;
}

VOID before_function_indirect_call(ADDRINT ip, ADDRINT callee_address,BOOL taken) {
   cout << "Enter before_function_indirect_call" << endl;
   if (taken)
      before_function_direct_call(ip,callee_address);

   cout << "Exit before_function_indirect_call" << endl;
}

VOID after_function_call(void) {
   cout << "Enter after_function_call" << endl;
   cout << "function return" << endl;
   vaccs_record_factory factory;
   return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
   rrec = rrec->add_event_num(timestamp++);
   rrec->write(vfp);
   delete rrec;
   cout << "Exit after_function_call" << endl;
}

// Is called for every instruction and instruments all of them that have 
// corresponding C code and modify a register
VOID monitor_function_calls(INS ins, VOID *v)
{

   if (INS_IsCall(ins)) {
      if (INS_IsDirectBranchOrCall(ins)) {
         cout << "Direct call: " << INS_Disassemble(ins) << endl;
         ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
         INS_InsertPredicatedCall(ins,IPOINT_BEFORE,AFUNPTR(before_function_direct_call),
                                 IARG_INST_PTR,
                                 IARG_ADDRINT, target,
                                 IARG_END);
      } else {
         cout << "Indirect call: " << INS_Disassemble(ins) << endl;
         INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(before_function_indirect_call),
                        IARG_INST_PTR,
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
