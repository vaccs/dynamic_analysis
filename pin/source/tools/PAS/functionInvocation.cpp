/*
 * functionInvocation.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: haoli
 */
#include "functionInvocation.h"
#include "global.h"
#include "database_access_api.h"
#include <unistd.h>
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

function_invocation_transaction function_invocation_event;
VOID functionInvocationBefore(void* function_name,const CONTEXT* ctxt,
      ADDRINT ip){
	char* name = (char*) function_name;
	current_function_name = name;
   DEBUGL(string sname(name));
   DEBUGL(LOG( "Function Call to " + sname +"\n"));
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
	DEBUGL(LOG("after push stack size:"+decstr(invocation_stack.size())+"\n"));
	function_invocation_event.function_name = (char*)function_name;
	//ou_function_invocation<<dec<<function_invocation_event.id<<","<<function_invocation_event.function_name<<","<<hex<<EBP<<endl;
	//pas_output<<"pin_function_invocation~!~"<<dec<<function_invocation_event.id<<"|"<<function_invocation_event.function_name<<"|"<<hex<<EBP<<endl;
	DEBUGL(LOG("Invocation ID:"+decstr(current_invocation_id)+"function "+name+" was called. Frame pointer was "+
            hexstr(EBP)+"\n"));

	//current_EBP = EBP;

   // There is no call to main, so we have to detect when it is entered

   if (!strncmp((char *)function_name,"main",4)) {

      INT32 column,line;
      string fileName;

	   PIN_LockClient();
	   PIN_GetSourceLocation(ip,&column,&line,&fileName);
	   PIN_UnlockClient();


      vaccs_record_factory factory;
 
      DEBUGL(LOG( "Call to function\n"));
      DEBUGL(LOG( "\tEvent num: " + decstr(id) +"\n"));
      DEBUGL(string sfname((char *)function_name));
      DEBUGL(LOG( "\tFunction name: " + sfname +"\n"));
      DEBUGL(LOG( "\tFunc line: " + decstr(line) +"\n"));
      DEBUGL(LOG( "\tInv line: 0\n"));
      DEBUGL(LOG( "\tFunc file: " + fileName +"\n"));
      DEBUGL(sfname = NOFUNCNAME);
      DEBUGL(LOG( "\tInv file: " + sfname +"\n"));
      DEBUGL(LOG( "\tCallee address: 0x" + hexstr(ip) + "\n\n"));
      func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
      DEBUGL(LOG( "frec = 0x" + hexstr(frec) +"\n"));
      frec = frec->add_event_num(timestamp++)
         ->add_func_name((char *)function_name)
         ->add_func_line_num(line)
         ->add_inv_line_num(0)
         ->add_c_func_file(fileName.c_str())
         ->add_c_inv_file(NOFUNCNAME)
         ->add_address(ip);
      DEBUGL(LOG( "Getting frame pointer for main\n"));
      ADDRINT dynamic_link = read_memory_as_address(EBP+OLD_FRAME_PTR_OFFSET);
      ADDRINT return_address = read_memory_as_address(EBP+RETURN_ADDRESS_OFFSET);
 
      DEBUGL(LOG( "Return address\n"));
      DEBUGL(LOG( "\tFunction name: main\n"));
      DEBUGL(LOG( "\tDynamic link: 0x" + hexstr(dynamic_link) +"\n"));
      DEBUGL(LOG( "\tReturn address: 0x" + hexstr(return_address) +"\n"));
      return_addr_record *rarec = ((return_addr_record*)factory
         .make_vaccs_record(VACCS_RETURN_ADDR))
         ->add_dynamic_link(dynamic_link)
         ->add_return_address(return_address)
         ->add_c_func_name("main");


      DEBUGL(LOG( "Built frec\n"));
      frec->write(vaccs_fd);
      rarec->write(vaccs_fd);
      DEBUGL(LOG( "Wrote frec\n"));
      delete frec;
   }
}
VOID functionInvocationAfter(void* function_name,const CONTEXT* ctxt){

   // This may have been a library call to print to stdout
   // Restore the original stdout
   // Check to see if there is anything in the pipe that is connected to stdout
   // If so, read it, print to stdout
   // Then reset stdout to the pipe
   //
  string fstr((char *)function_name);

 
	invocation_stack.pop();

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
