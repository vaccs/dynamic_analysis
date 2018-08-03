/*
 * memory_access.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "monitor_registers.h"
#include <iostream>
#include "global.h"
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/register_record.h>

extern vaccs_dw_reader *vdr;
extern FILE *vfp;
using namespace std;

VOID AfterRegMod(ADDRINT ip, CONTEXT *ctxt, REG reg) {
	INT32	column;
	INT32	line;
	std::string 	fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip,&column,&line,&fileName);
	PIN_UnlockClient();

   if (line == 0)
      fileName = NOCSOURCE;

   vaccs_record_factory factory;
   std::string reg_name = REG_StringShort(reg);
   PIN_REGISTER value;
   PIN_GetContextRegval(ctxt,reg,(UINT8*)&value);
   Generic regval;

   // No vector registers handled nor floating point registers
   switch (REG_Size(reg)) {
      case 1:
         regval = (Generic)value.byte[0];
         break;
      case 2:
         regval = (Generic)value.word[0];
         break;
      case 4:
         regval = (Generic)value.dword[0];
         break;
      default:
         regval = (Generic)value.qword[0];
         break;
   }

   register_record *rrec = (register_record*)factory.make_vaccs_record(VACCS_REGISTER);
   cout << "Register Record" << endl;
   cout << '\t' << "Event num: " << dec << timestamp << endl;
   cout << '\t' << "Line #: " << line << endl;
   cout << '\t' << "File: " << fileName << endl;
   cout << '\t' << "Register: " << reg_name << endl;
   cout << '\t' << "Value: 0x" << hex << regval << dec << endl;
   rrec = rrec->add_event_num(timestamp++)
      ->add_c_line_num(line)
      ->add_c_file_name(fileName.c_str())
      ->add_register_name(reg_name.c_str())
      ->add_value(regval);

   rrec->write(vfp);
}

// Is called for every instruction and instruments all of them that have 
// corresponding C code and modify a register
VOID MonitorRegisterInstruction(INS ins, VOID *v)
{

	if (INS_MaxNumWRegs(ins) > 0) {
   	string assembly_code = INS_Disassemble(ins);
	   IPOINT where = IPOINT_AFTER;
	   if (!INS_HasFallThrough(ins))
	      where = IPOINT_TAKEN_BRANCH;
      UINT32 num_wregs = INS_MaxNumWRegs(ins);
      for (UINT32 i = 0; i < num_wregs; i++) {
         REG reg = INS_RegW(ins,i);
	      INS_InsertPredicatedCall(ins, where, (AFUNPTR)AfterRegMod,
				                     IARG_INST_PTR,
                                 IARG_CONTEXT,
                                 IARG_UINT32, reg,
				                     IARG_END);
      }
   }
}
