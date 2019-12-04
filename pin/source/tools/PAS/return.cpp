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
#include <util/vaccs_config.h>

extern vaccs_config *vcfg;

using namespace std;

VOID BeforeHalt(ADDRINT ip,CONTEXT * ctxt)
{
  INT32 column, line;
  string fileName;

  PIN_LockClient();
  PIN_GetSourceLocation(ip, &column, &line, &fileName);
  PIN_UnlockClient();	vaccs_record_factory factory;

	if (line == 0 && vcfg->get_user_code_only())
		return;

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
	if (Is_System_Image(img)) return;
	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
		// RTN_InsertCall() and INS_InsertCall() are executed in order of
		// appearance.  In the code sequence below, the IPOINT_AFTER is
		// executed before the IPOINT_BEFORE.
		for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
			// Open the RTN.
			RTN_Open( rtn );

			// IPOINT_AFTER is implemented by instrumenting each return
			// instruction in a routine.  Pin tries to find all return
			// instructions, but success is not guaranteed.


			// Examine each instruction in the routine.
			for ( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) ) {
				if (INS_IsHalt(ins)) {
					DEBUGL(LOG("Instrument a halt instruction\n"));
					INS_InsertCall( ins, IPOINT_BEFORE, (AFUNPTR)BeforeHalt,
					                IARG_INST_PTR, IARG_CONTEXT, IARG_END);
				}
			}
			// Close the RTN.
			RTN_Close( rtn );
		}
	}
}
