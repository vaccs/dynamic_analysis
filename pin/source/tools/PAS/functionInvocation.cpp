/*
 * functionInvocation.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: haoli
 */
#include "functionInvocation.h"
#include "global.h"
#include "database_access_api.h"
#include<iostream>
#include <fstream>
#include <cstring>

#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/func_inv_record.h>
#include <io/return_record.h>
#include <vaccs_read/vaccs_dw_reader.h>

extern vaccs_dw_reader *vdr;
extern FILE *vfp;

function_invocation_transaction function_invocation_event;
VOID functionInvocationBefore(void* function_name,const CONTEXT* ctxt,
      ADDRINT ip){
	char* name = (char*) function_name;
	current_function_name = name;
	if(strcmp(name,".plt")==0) return;
	if(strcmp(name,"frame_dummy")==0) return;

	int id = timestamp++;
	current_invocation_id = id;
	get_registers(ctxt,id);
	ADDRINT EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
	function_invocation_event.frame_pointer = EBP;
	function_invocation_event.id = id;
	store_function_invocation_transaction(function_invocation_event);
	invocation_stack.push(function_invocation_event);
	std::cout<<"after push stack size:"<<invocation_stack.size()<<std::endl;
	function_invocation_event.function_name = (char*)function_name;
	ou_function_invocation<<dec<<function_invocation_event.id<<","<<function_invocation_event.function_name<<","<<hex<<EBP<<std::endl;
	pas_output<<"pin_function_invocation~!~"<<dec<<function_invocation_event.id<<"|"<<function_invocation_event.function_name<<"|"<<hex<<EBP<<std::endl;
	std::cout<<"Invocation ID:"<<current_invocation_id<<"function "<<name<<" was called. Frame pointer was "<<hex<<EBP<<std::endl;

	//current_EBP = EBP;

   // There is no call to main, so we have to detect when it is entered

   if (!strncmp((char *)function_name,"main",4)) {

      INT32 column,line;
      std::string fileName;

	   PIN_LockClient();
	   PIN_GetSourceLocation(ip,&column,&line,&fileName);
	   PIN_UnlockClient();


      vaccs_record_factory factory;
 
      cout << "Call to function" << endl;
      cout << '\t' << "Event num: " << dec << id << endl;
      cout << '\t' << "Function name: " << (char *)function_name << endl;
      cout << '\t' << "Func line: " << line << endl;
      cout << '\t' << "Inv line: 0" << endl;
      cout << '\t' << "Func file: " << fileName << endl;
      cout << '\t' << "Inv file: " << NOFUNCNAME << endl;
      cout << '\t' << "Callee address: 0x" << hex << ip << endl << endl;
      func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
      cout << "frec = 0x" << frec << endl;
      frec = frec->add_event_num(timestamp++)
         ->add_func_name((char *)function_name)
         ->add_func_line_num(line)
         ->add_inv_line_num(0)
         ->add_c_func_file(fileName.c_str())
         ->add_c_inv_file(NOFUNCNAME)
         ->add_address(ip);

      cout << "Built frec" << endl;
      frec->write(vfp);
      cout << "Wrote frec" << endl;
      delete frec;
   }
}
VOID functionInvocationAfter(void* function_name,const CONTEXT* ctxt){

	invocation_stack.pop();

   vaccs_record_factory factory;

   return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
   rrec = rrec->add_event_num(timestamp++);

   rrec->write(vfp);

   delete rrec;
 	//char* name = (char*) function_name;
	//ou_function_invocation<<"quit "<<name<<std::endl;
	//ou_function_invocation<<"after pop stack size:"<<invocation_stack.size()<<std::endl;
}
VOID FunctionInvocatioinImage(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find the malloc() function.
	if(Is_System_Image(img)) return;
	for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
				for(RTN rtn = SEC_RtnHead(sec);RTN_Valid(rtn);rtn = RTN_Next(rtn)){
					RTN_Open(rtn);

					RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)functionInvocationBefore,

							IARG_PTR, RTN_Name(rtn).c_str(), IARG_CONTEXT, IARG_INST_PTR,
					                       IARG_END);
					RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)functionInvocationAfter,

												IARG_PTR, RTN_Name(rtn).c_str(), IARG_CONTEXT,
										                       IARG_END);
					RTN_Close(rtn);
				}
			}

}
