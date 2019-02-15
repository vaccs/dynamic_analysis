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
#include <iostream>
#include <fstream>
#include <cstring>

#include <util/general.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/func_inv_record.h>
#include <io/return_record.h>
#include <io/return_addr_record.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/output_record.h>
#include <tables/frame.h>

extern NATIVE_FD vaccs_stdout;
extern runtime_stack *stack_model;
Generic stack_base_address = 0;

function_invocation_transaction function_invocation_event;
VOID functionInvocationBefore(void* function_name,const CONTEXT* ctxt,
      ADDRINT ip){

    char* name = (char*) function_name;
    current_function_name = name;

    DEBUGL(string sname(name));
    DEBUGL(LOG( "Function Call to " + sname +"\n"));
    //if(strcmp(name,".plt")==0) return;
    if(strcmp(name,"frame_dummy")==0) return;

    int id = timestamp++;
    current_invocation_id = id;
    get_registers(ctxt,id);
    ADDRINT EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
    ADDRINT ESP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);

    if (!strncmp(name,"_start",6)) {
	DEBUGL(LOG("Base SP = "+hexstr(ESP)+"\n"));
	stack_base_address = (Generic)ESP;
    }

    function_invocation_event.frame_pointer = EBP;
    function_invocation_event.id = id;
    store_function_invocation_transaction(function_invocation_event);
    invocation_stack.push(function_invocation_event);
    DEBUGL(LOG("after push stack size:"+decstr(invocation_stack.size())+"\n"));
    function_invocation_event.function_name = (char*)function_name;
    DEBUGL(LOG("Invocation ID:"+decstr(current_invocation_id)+"function "+name+" was called.\n Frame pointer was "+
           hexstr(EBP)+"\n"));


    INT32 column,line;
    string fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip,&column,&line,&fileName);
    PIN_UnlockClient();


    vaccs_record_factory factory;

    string sfname(name);
    DEBUGL(LOG( "Getting frame pointer for " + sfname + "\n"));
    ADDRINT dynamic_link = 0;
    ADDRINT return_address = 0;
    INT32 inv_column = 0, inv_line = 0;
    string inv_fileName = NOFUNCNAME;

    if (EBP >= ESP && EBP < stack_base_address) {
	dynamic_link = read_memory_as_address(EBP+OLD_FRAME_PTR_OFFSET);
	return_address = read_memory_as_address(EBP+RETURN_ADDRESS_OFFSET);
	PIN_LockClient();
	PIN_GetSourceLocation(return_address-sizeof(Generic),&inv_column,&inv_line,&inv_fileName);
	PIN_UnlockClient();
    }

    DEBUGL(LOG( "Call to function\n"));
    DEBUGL(LOG( "\tEvent num: " + decstr(id) +"\n"));
    DEBUGL(LOG( "\tFunction name: " + sfname +"\n"));
    DEBUGL(LOG( "\tFunc line: " + decstr(line) +"\n"));
    DEBUGL(LOG( "\tInv line: " + decstr(inv_line) + "\n"));
    DEBUGL(LOG( "\tFunc file: " + fileName +"\n"));
    DEBUGL(LOG( "\tInv file: " + inv_fileName +"\n"));
    DEBUGL(LOG( "\tCallee stack address: " + hexstr(ESP) + "\n\n"));
    func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
    frec = frec->add_event_num(timestamp++)
        	->add_func_name(name)
        	->add_func_line_num(line)
        	->add_inv_line_num(inv_line)
        	->add_c_func_file(fileName.c_str())
        	->add_c_inv_file(inv_fileName.c_str())
        	->add_address((Generic)ESP);
    frec->write(vaccs_fd);
    delete frec;

    DEBUGL(LOG( "Return address\n"));
    DEBUGL(LOG( "\tFunction name: main\n"));
    DEBUGL(LOG( "\tDynamic link: " + hexstr(dynamic_link) +"\n"));
    DEBUGL(LOG( "\tReturn address: " + hexstr(return_address) +"\n"));
    return_addr_record *rarec = ((return_addr_record*)factory
      	.make_vaccs_record(VACCS_RETURN_ADDR))
      	->add_dynamic_link(dynamic_link)
      	->add_return_address(return_address)
      	->add_c_func_name(name);
    rarec->write(vaccs_fd);
    delete rarec;

    DEBUGL(LOG("Build stack frame\n"));

    if (line != 0)
	stack_model->push(sfname,(Generic)ip, ctxt);
}

VOID functionInvocationAfter(void* function_name,const CONTEXT* ctxt,ADDRINT ip){

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

    vaccs_record_factory factory;

    DEBUGL(LOG("Enter functionInvocationAfter\n"));

    char stdout_buff[VACCS_MAX_OUTPUT_LENGTH+1];

    USIZE num_bytes = VACCS_MAX_OUTPUT_LENGTH;

    OS_ReadFD(vaccs_stdout,&num_bytes,stdout_buff);

    if (num_bytes != 0) {
	stdout_buff[num_bytes] = '\0';
	string bufs(stdout_buff);
	DEBUGL(LOG("output has been found: " + bufs + "\n"));
	output_record *orec = (output_record*)factory.make_vaccs_record(VACCS_OUTPUT);
	orec = orec->add_event_num(timestamp++)
		->add_output(stdout_buff);
	orec->write(vaccs_fd);
	delete orec;
    }
    else {
	DEBUGL(LOG("No output found\n"));
    }

    return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
    rrec = rrec->add_event_num(timestamp++);
    rrec->write(vaccs_fd);
    delete rrec;

    INT32 column,line;
    string fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip,&column,&line,&fileName);
    PIN_UnlockClient();


    if (line != 0) {
	     stack_model->pop();
       stack_model->update_variables();
    }
    DEBUGL(LOG("function return\n"));
    DEBUGL(LOG("Exit functionInvocationAfter\n"));
}

VOID FunctionInvocatioinImage(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find the malloc() function.
	if(Is_System_Image(img)) return;
	for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
	    if (SEC_Name(sec).find(".plt") == string::npos) // ignore dynamically linked libc routines
		for(RTN rtn = SEC_RtnHead(sec);RTN_Valid(rtn);rtn = RTN_Next(rtn)){
		    RTN_Open(rtn);

		    cerr << "Instrumenting call to routine " << RTN_Name(rtn) << endl;
		    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)functionInvocationBefore,
				    IARG_PTR, RTN_Name(rtn).c_str(), IARG_CONTEXT, IARG_INST_PTR,
				    IARG_END);
		    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)functionInvocationAfter,
				    IARG_PTR, RTN_Name(rtn).c_str(), IARG_CONTEXT, IARG_INST_PTR,
				    IARG_END);
		    RTN_Close(rtn);
		}
	}

}
