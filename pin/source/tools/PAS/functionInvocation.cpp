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
	DEBUGL(cout<<"after push stack size:"<<invocation_stack.size()<<endl);
	function_invocation_event.function_name = (char*)function_name;
	//ou_function_invocation<<dec<<function_invocation_event.id<<","<<function_invocation_event.function_name<<","<<hex<<EBP<<endl;
	//pas_output<<"pin_function_invocation~!~"<<dec<<function_invocation_event.id<<"|"<<function_invocation_event.function_name<<"|"<<hex<<EBP<<endl;
	DEBUGL(cout<<"Invocation ID:"<<current_invocation_id<<"function "<<name<<" was called. Frame pointer was "<<hex<<EBP<<endl);

	//current_EBP = EBP;

   // There is no call to main, so we have to detect when it is entered

   if (!strncmp((char *)function_name,"main",4)) {

      INT32 column,line;
      string fileName;

	   PIN_LockClient();
	   PIN_GetSourceLocation(ip,&column,&line,&fileName);
	   PIN_UnlockClient();


      vaccs_record_factory factory;
 
      DEBUGL(cout << "Call to function" << endl);
      DEBUGL(cout << '\t' << "Event num: " << dec << id << endl);
      DEBUGL(cout << '\t' << "Function name: " << (char *)function_name << endl);
      DEBUGL(cout << '\t' << "Func line: " << line << endl);
      DEBUGL(cout << '\t' << "Inv line: 0" << endl);
      DEBUGL(cout << '\t' << "Func file: " << fileName << endl);
      DEBUGL(cout << '\t' << "Inv file: " << NOFUNCNAME << endl);
      DEBUGL(cout << '\t' << "Callee address: 0x" << hex << ip << endl << endl);
      func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
      DEBUGL(cout << "frec = 0x" << frec << endl);
      frec = frec->add_event_num(timestamp++)
         ->add_func_name((char *)function_name)
         ->add_func_line_num(line)
         ->add_inv_line_num(0)
         ->add_c_func_file(fileName.c_str())
         ->add_c_inv_file(NOFUNCNAME)
         ->add_address(ip);
      DEBUGL(cout << "Getting frame pointer for main" << endl);
      ADDRINT dynamic_link = read_memory_as_address(EBP+OLD_FRAME_PTR_OFFSET);
      ADDRINT return_address = read_memory_as_address(EBP+RETURN_ADDRESS_OFFSET);
 
   DEBUGL(cout << "Return address" << endl);
   DEBUGL(cout << '\t' << "Function name: main" << endl);
   DEBUGL(cout << '\t' << "Dynamic link: 0x" << hex << dynamic_link << endl);
   DEBUGL(cout << '\t' << "Return address: 0x" << hex << return_address << endl);
       return_addr_record *rarec = ((return_addr_record*)factory
         .make_vaccs_record(VACCS_RETURN_ADDR))
         ->add_dynamic_link(dynamic_link)
         ->add_return_address(return_address)
         ->add_c_func_name("main");


      DEBUGL(cout << "Built frec" << endl);
      frec->write(vfp);
      rarec->write(vfp);
      DEBUGL(cout << "Wrote frec" << endl);
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
	//ou_function_invocation<<"quit "<<name<<endl;
	//ou_function_invocation<<"after pop stack size:"<<invocation_stack.size()<<endl;
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
