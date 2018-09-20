/*
 * return.cpp
 *
 *  Created on: Jul 19, 2016
 *      Author: haoli
 */
#include "return.h"
#include <iostream>
#include <fstream>
#include "global.h"
#include "database_access_api.h"
#include <util/general.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/return_record.h>


using namespace std;

VOID AfterReturn( const CONTEXT * ctxt)
{
    ADDRINT AfterSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR );
    DEBUGL(LOG("After return : SP = " + hexstr(AfterSP) + "\n"));
    return_event.sp_after = AfterSP;
    store_return_transaction(return_event);
    //ou_return<<","<< hex<< return_event.sp_after<< "," <<dec << return_event.id_function_invocation_happened_in<<endl;
    //pas_output<<"|"<< hex<< return_event.sp_after<< "|" <<dec << return_event.id_function_invocation_happened_in<<endl;
}

VOID BeforeReturn(CONTEXT * ctxt)
{
    ADDRINT BeforeSP = (ADDRINT)PIN_GetContextReg( ctxt, REG_STACK_PTR);
    DEBUGL(LOG("Before return: SP = " + hexstr(BeforeSP) + "\n"));
    return_event.id = timestamp++;
    return_event.id_function_invocation_happened_in = invocation_stack.top().id;
    return_event.sp_before = BeforeSP;
    get_registers(ctxt,return_event.id);
    //ou_return << dec << return_event.id  <<","<<hex << return_event.sp_before;
    //pas_output<<"pin_return~!~" << dec << return_event.id  <<"|"<<hex << return_event.sp_before;
}

VOID BeforeHalt(CONTEXT * ctxt)
{
    vaccs_record_factory factory;

    return_record *rrec = (return_record*)factory.make_vaccs_record(VACCS_RETURN);
    rrec = rrec->add_event_num(timestamp++);
    rrec->write(vaccs_fd);
    delete rrec;

    DEBUGL(LOG("Halt\n"));
    DEBUGL(LOG("exit BeforeHalt\n"));
}

/*
 * Instrumentation routines
 */
VOID ReturnImage(IMG img, VOID *v)
{
	if(Is_System_Image(img)) return;
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        // RTN_InsertCall() and INS_InsertCall() are executed in order of
        // appearance.  In the code sequence below, the IPOINT_AFTER is
        // executed before the IPOINT_BEFORE.
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            // Open the RTN.
            RTN_Open( rtn );

            // IPOINT_AFTER is implemented by instrumenting each return
            // instruction in a routine.  Pin tries to find all return
            // instructions, but success is not guaranteed.


            // Examine each instruction in the routine.
            for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) )
            {
                if( INS_IsRet(ins) )
                {
                    // instrument each return instruction.
                    // IPOINT_TAKEN_BRANCH always occurs last.
                    INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)BeforeReturn,
                                   IARG_CONTEXT, IARG_END);
                    INS_InsertCall( ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)AfterReturn,
                                   IARG_CONTEXT, IARG_END);
                } else if (INS_IsHalt(ins)) {
		    cerr << "Instrument a halt instruction\n";
                    INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)BeforeHalt,
                                   IARG_CONTEXT, IARG_END);
		}
            }
            // Close the RTN.
            RTN_Close( rtn );
        }
    }
}
