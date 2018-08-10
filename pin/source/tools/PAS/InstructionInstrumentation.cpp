/*
 * InstructionInstrumentation.cpp
 *
 *  Created on: Aug 15, 2016
 *      Author: haoli
 */
#include "InstructionInstrumentation.h"
#include "memory_access.h"
#include "monitor_registers.h"
#include "function_call.h"
#include "stack_operation.h"
#include "global.h"
#include <iostream>
#include <string>
#include <util/general.h>
#include <io/vaccs_record_factory.h>
#include <io/asm_record.h>

/* ===================================================================== */
/* Write a vaccs asm record for each line of assembly                    */
/* ===================================================================== */
VOID EmitAssembly(INS ins, VOID *v){
   string assembly = INS_Disassemble(ins);
   ADDRINT ip = INS_Address(ins);

   INT32	column;
	INT32	line;
	string 	fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip,&column,&line,&fileName);
	PIN_UnlockClient();

   string asmFileName;
   if (line == 0) {  // there is no corresponding source
      fileName = NOCSOURCE;
      asmFileName = NOASMSOURCE;
   }
   else {
      asmFileName = fileName;
      string key = ".c";
      string rpl = ".s";
      size_t pos = asmFileName.rfind(key);
      if (pos != string::npos)
         asmFileName.replace(pos,key.length(),rpl);
   }
   vaccs_record_factory factory;
   
   DEBUGL(LOG("ASM Record\n"));
   DEBUGL(LOG("\tASM line #: " + decstr(ip) + "\n"));
   DEBUGL(LOG("\tC line #: " + decstr(line) + "\n"));
   DEBUGL(LOG("\tASM file: " + asmFileName + "\n"));
   DEBUGL(LOG("\tC file: " + fileName + "\n"));
   DEBUGL(LOG("\tInstruction: " + assembly + "\n\n")); 
  
   asm_record *arec = (asm_record*)factory.make_vaccs_record(VACCS_ASM);
   arec->add_asm_line_num(ip)
      ->add_c_line_num(line)
      ->add_asm_file_name(asmFileName.c_str())
      ->add_c_file_name(fileName.c_str())
      ->add_asm_inst(assembly.c_str());

   arec->write(vaccs_fd);
   delete arec;
}


//extern bool Is_System_Image(IMG img);;
VOID InstructionInstrumentation(IMG img, VOID *v){
	if(Is_System_Image(img)) return;
		for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
			for(RTN rtn = SEC_RtnHead(sec);RTN_Valid(rtn);rtn = RTN_Next(rtn)){
				RTN_Open(rtn);
				for(INS ins = RTN_InsHead(rtn);INS_Valid(ins);ins = INS_Next(ins)){
               EmitAssembly(ins,0);
               MonitorRegisterInstruction(ins,0);
					MemoryAccessInstruction(ins, 0);
               monitor_function_calls(ins,0);
					StackOperationInstruction(ins, 0);
				}
				RTN_Close(rtn);
			}
		}

}
