/*
 * sd.cpp
 *
 */
#include "setuid.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include "global.h"
#include <util/general.h>
#include <util/vaccs_config.h>
#include <tables/cu_table.h>
#include <tables/deref.h>
#include <tables/type_table.h>
#include <tables/var_table.h>
#include <tables/frame.h>

#include <io/vaccs_record_factory.h>

using namespace std;

extern runtime_stack * stack_model;
extern vaccs_config *vcfg;
extern list<ADDRINT> call_inst_ip; // managed in function_call.cpp

VOID setuid_before(UINT32 uid ) {

	DEBUGL(LOG("In setuid_before, uid = "+decstr(uid)+"\n"));

	INT32 column;
	INT32 line;
	string file_name;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
	PIN_UnlockClient();

	if (line != 0) {
		DEBUGL(LOG("Call from line = " + decstr(line)+"\n"));
	}
}

/* ===================================================================== */
/* Analysis routine for setuid                                           */
/* ===================================================================== */
VOID SetUIDImage(IMG img, VOID *v)
{
	DEBUGL(LOG("In SetUIDImage\n"));
	RTN setuid_rtn = RTN_FindByName(img, "seteuid@plt");

		if (RTN_Valid(setuid_rtn)) {
			DEBUGL(LOG("Instrumenting setuid\n"));
			RTN_Open(setuid_rtn);

			RTN_InsertCall(setuid_rtn, IPOINT_BEFORE, (AFUNPTR)setuid_before,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_END);
			RTN_Close(setuid_rtn);
		}

	DEBUGL(LOG("Exit SetUIDImage\n"));
}
