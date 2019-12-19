/*
 * sd.cpp
 *
 */
#include "sd.h"
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
#include <io/sd_clear_record.h>
#include <io/sd_set_record.h>
#include <io/sd_lock_record.h>
#include <io/sd_unlock_record.h>
#include <io/sd_czero_record.h>

using namespace std;

extern runtime_stack * stack_model;
extern vaccs_config *vcfg;
extern list<ADDRINT> call_inst_ip; // managed in function_call.cpp

VOID vaccs_declare_as_secure_data(ADDRINT addr)
{
	DEBUGL(LOG("In vaccs_declare_as_secure_data\n"));
	cu_table * cutab = vdr->get_cutab();

	list<frame_record *> * sdlist = stack_model->get_variables_with_address((Generic)addr, cutab);

	for (list<frame_record *>::iterator it = sdlist->begin(); it != sdlist->end(); it++) {
		frame_record *frec = *it;
		string scope = cutab->get_scope(frec->get_var_record());
		DEBUGL(LOG("Variable " + frec->get_variable_name() + " from scope " + scope + " declared as secure data\n"));
		frec->add_sensitive_data();
	}
	DEBUGL(LOG("Exit vaccs_declare_as_secure_data\n"));
}

VOID vaccs_sd_clear_set_check(ADDRINT ip, ADDRINT addr, UINT32 c, UINT64 n)
{

	DEBUGL(LOG("In vaccs_sd_clear_set_check, c = "+decstr(c)+", n = "+decstr(n)+"\n"));
	INT32 column;
	INT32 line;
	string file_name;

	PIN_LockClient();
	PIN_GetSourceLocation(ip, &column, &line, &file_name);
	PIN_UnlockClient();

	if (line != 0) {
		DEBUGL(LOG("In vaccs_sd_clear_set_check, call from user code"));
		DEBUGL(LOG("ip = " + hexstr(ip) + ", line = " + decstr(line) + ", addr = " + hexstr(addr)));
		DEBUGL(LOG("c = " + decstr(c) + ", n = " + decstr(n) + "\n"));
		cu_table * cutab = vdr->get_cutab();

		list<frame_record *> * sdlist = stack_model->get_variables_with_address((Generic)addr, cutab);

		for (list<frame_record *>::iterator it = sdlist->begin(); it != sdlist->end(); it++) {
			frame_record *frec = *it;

			if (frec->get_sensitive_data()) {
				var_record *vrec = frec->get_var_record();

				frec->mark_data_clear_status(cutab,frec->get_frame()->get_context(),addr,c,n);

				string scope = cutab->get_scope(vrec);
				DEBUGL(LOG("Variable " + frec->get_variable_name() + " from scope " + scope + " set/cleared\n"));
				vaccs_record_factory factory;

				if (frec->is_data_clear()) {
					DEBUGL(LOG("Write sd_clear, c = 0, n = "+decstr(n)+"\n"));
					sd_clear_record * crec = (sd_clear_record*)factory.make_vaccs_record(VACCS_SD_CLEAR);
					crec->add_event_num(timestamp)
					->add_c_line_num(line)
					->add_c_file_name(file_name.c_str())
					->add_scope(scope.c_str())
					->add_variable(frec->get_variable_name().c_str());
					crec->write(vaccs_fd);
					delete crec;
					frec->reset_clear_set_status();
				} else if (frec->is_data_set()) {
					DEBUGL(LOG("Write sd_set, c = "+decstr(c)+", n = "+decstr(n)+"\n"));
					sd_set_record * srec = (sd_set_record*)factory.make_vaccs_record(VACCS_SD_SET);
					srec->add_event_num(timestamp)
					->add_c_line_num(line)
					->add_c_file_name(file_name.c_str())
					->add_scope(scope.c_str())
					->add_variable(frec->get_variable_name().c_str());
					srec->write(vaccs_fd);
					delete srec;
					frec->reset_clear_set_status();
				}
			}
		}
		timestamp++;
	}
	DEBUGL(LOG("Exit vaccs_sd_clear_set_check\n"));
}

VOID vaccs_sd_clear_set_memset(ADDRINT addr, UINT32 c, UINT64 n)
{
	DEBUGL(LOG("In vaccs_sd_clear_set_memset, calling vaccs_sd_clear_set_check\n"));
	vaccs_sd_clear_set_check(call_inst_ip.front(), addr, c, n);
	DEBUGL(LOG("Exit vaccs_sd_clear_set_memcpy\n"));
}

VOID vaccs_sd_clear_set_memcpy(ADDRINT dest_addr, ADDRINT src_addr, UINT64 n)
{
	DEBUGL(LOG("In vaccs_sd_clear_set_memcpy, calling vaccs_sd_clear_set_check\n"));
	int val = 0;
	char *src = (char*)src_addr;
	for (unsigned long i = 0; i < *(unsigned long*)n && val == 0; i++, src++)
		if (*src != '\0')
			val = 1;
	vaccs_sd_clear_set_check(call_inst_ip.front(), dest_addr, val, n);
	DEBUGL(LOG("Exit vaccs_sd_clear_set_memcpy\n"));
}

VOID vaccs_sd_clear_set_bcopy(ADDRINT src_addr, ADDRINT dest_addr, UINT64 n)
{
	DEBUGL(LOG("In vaccs_sd_clear_set_bcopy, calling vaccs_sd_clear_set_memcpy\n"));
	vaccs_sd_clear_set_memcpy(dest_addr, src_addr, n);
	DEBUGL(LOG("Exit vaccs_sd_clear_set_bcopy\n"));
}

VOID vaccs_sd_clear_set_bzero(ADDRINT addr, UINT64 n)
{
	DEBUGL(LOG("In vaccs_sd_clear_set_bzero, calling vaccs_sd_clear_set_check\n"));
	vaccs_sd_clear_set_check(call_inst_ip.front(), addr, 0, n);
	DEBUGL(LOG("Exit vaccs_sd_clear_set_bzero\n"));
}


VOID vaccs_sd_clear_set_mem_write(ADDRINT ip, ADDRINT addr, const CONTEXT *ctxt, UINT32 size)
{

	DEBUGL(LOG("In vaccs_sd_clear_set_mem_write, calling vaccs_sd_clear_set_memcpy\n"));
	cu_table * cutab = vdr->get_cutab();
	cu_record * curec = cutab->get(ip);

	if (curec == NULL) {
		DEBUGL(LOG("ip = " + hexstr(ip) + " is not in user code\n"));
		return;
	} else {
		int val;
		DEBUGL(LOG("ip = " + hexstr(ip) + " is in user code checking sd clear of atomic value\n"));
		switch (size) {
		case 1:
			val = (int)*((char*)addr);
			break;
		case 2:
			val = (int)*((short*)addr);
			break;
		case 4:
			val = *((int*)addr);
			break;
		case 8:
		{
			long lval = (long)*((long*)addr);
			val = (lval == 0) ? 0 : 1; // the exact nonzero value does not matter here
			break;
		}
		default:
			return;
		}
		vaccs_sd_clear_set_check(ip, addr, val, size);
	}
	DEBUGL(LOG("Exit vaccs_sd_clear_set_mem_write\n"));
}

VOID vaccs_sd_lock(ADDRINT addr, UINT64 n) {

	DEBUGL(LOG("In vaccs_sd_lock, addr = "+hexstr(addr)+", n = "+decstr(n)+"\n"));
	INT32 column;
	INT32 line;
	string file_name;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
	PIN_UnlockClient();

	if (line != 0) {
		DEBUGL(LOG("In vaccs_sd_lock, call from user code"));
		DEBUGL(LOG("ip = " + hexstr(call_inst_ip.front()) + ", line = " + decstr(line) + ", addr = " + hexstr(addr)));
		DEBUGL(LOG(" n = " + decstr(n) + "\n"));
		cu_table * cutab = vdr->get_cutab();

		list<frame_record *> * sdlist = stack_model->get_variables_with_address((Generic)addr, cutab);

		for (list<frame_record *>::iterator it = sdlist->begin(); it != sdlist->end(); it++) {
			frame_record *frec = *it;

			if (frec->get_sensitive_data()) {
				var_record *vrec = frec->get_var_record();
				type_record *trec = cutab->get_type_record(vrec->get_type());


				string scope = cutab->get_scope(vrec);
				DEBUGL(LOG("Variable " + frec->get_variable_name() + " from scope " + scope + " unlocked\n"));
				vaccs_record_factory factory;

				if (n >= trec->get_size()) {
					DEBUGL(LOG("Write sd_lock,  n = "+decstr(n)+"\n"));
					sd_lock_record * lrec = (sd_lock_record*)factory.make_vaccs_record(VACCS_SD_LOCK);
					lrec->add_event_num(timestamp++)
					->add_c_line_num(line)
					->add_c_file_name(file_name.c_str())
					->add_scope(scope.c_str())
					->add_variable(frec->get_variable_name().c_str());
					lrec->write(vaccs_fd);
					delete lrec;
				}
			}
		}
	}
}

VOID vaccs_sd_unlock(ADDRINT addr, UINT64 n) {

	DEBUGL(LOG("In vaccs_sd_unlock, addr = "+hexstr(addr)+", n = "+decstr(n)+"\n"));
	INT32 column;
	INT32 line;
	string file_name;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
	PIN_UnlockClient();

	if (line != 0) {
		DEBUGL(LOG("In vaccs_sd_unlock, call from user code"));
		DEBUGL(LOG("ip = " + hexstr(call_inst_ip.front()) + ", line = " + decstr(line) + ", addr = " + hexstr(addr)));
		DEBUGL(LOG("n = " + decstr(n) + "\n"));
		cu_table * cutab = vdr->get_cutab();

		list<frame_record *> * sdlist = stack_model->get_variables_with_address((Generic)addr, cutab);

		for (list<frame_record *>::iterator it = sdlist->begin(); it != sdlist->end(); it++) {
			frame_record *frec = *it;

			if (frec->get_sensitive_data()) {
				var_record *vrec = frec->get_var_record();
				type_record *trec = cutab->get_type_record(vrec->get_type());


				string scope = cutab->get_scope(vrec);
				DEBUGL(LOG("Variable " + frec->get_variable_name() + " from scope " + scope + " locked\n"));
				vaccs_record_factory factory;

				if (n >= trec->get_size()) {
					DEBUGL(LOG("Write sd_unlock,  n = "+decstr(n)+"\n"));
					sd_unlock_record * ulrec = (sd_unlock_record*)factory.make_vaccs_record(VACCS_SD_UNLOCK);
					ulrec->add_event_num(timestamp++)
					->add_c_line_num(line)
					->add_c_file_name(file_name.c_str())
					->add_scope(scope.c_str())
					->add_variable(frec->get_variable_name().c_str());
					ulrec->write(vaccs_fd);
					delete ulrec;
				}
			}
		}
	}
}

VOID vaccs_sd_czero(UINT32 resource, ADDRINT rlim) {

	DEBUGL(LOG("In vaccs_sd_czero, resource = "+decstr(resource)+"\n"));

	INT32 column;
	INT32 line;
	string file_name;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
	PIN_UnlockClient();

	if (line != 0) {
		if (resource == RLIMIT_CORE) {
			struct rlimit *rl = (struct rlimit *)rlim;
			if (rl->rlim_max == 0) {
				vaccs_record_factory factory;
				sd_czero_record * czrec = (sd_czero_record*)factory.make_vaccs_record(VACCS_SD_CZERO);
				czrec->add_event_num(timestamp++)
						->add_c_line_num(line)
						->add_c_file_name(file_name.c_str());
				czrec->write(vaccs_fd);
				delete czrec;
			}
		}
	}
}

// called from InstructionInstrumentation.cpp
VOID vaccs_sd_clear_set_instrument_inst(INS ins, VOID * v)
{
	DEBUGL(LOG("In vaccs_sd_clear_instrument_ins\n"));
	if (vcfg->get_secure_data()) {
		DEBUGL(LOG("Instrumenting instructions to monitor sd_clear\n"));
		UINT32 memOperands = INS_MemoryOperandCount(ins);

		// Iterate over each memory operand of the instruction.
		for (UINT32 memOp = 0; memOp < memOperands; memOp++) {
			// Note that in some architectures a single memory operand can be
			// both read and written (for instance incl (%eax) on IA-32)
			// In that case we instrument it once for read and once for write.
			if (INS_MemoryOperandIsWritten(ins, memOp)) {
				IPOINT where = IPOINT_AFTER;
				if (!INS_HasFallThrough(ins))
					where = IPOINT_TAKEN_BRANCH;
				INS_InsertPredicatedCall(
					ins, where, (AFUNPTR)vaccs_sd_clear_set_mem_write,
					IARG_INST_PTR,
					IARG_MEMORYOP_EA,
					memOp, IARG_CONST_CONTEXT,
					IARG_MEMORYWRITE_SIZE,
					IARG_END);
			}
		}
	}
	DEBUGL(LOG("Exit vaccs_sd_clear_set_instrument_ins\n"));
}

/* ===================================================================== */
/* Analysis routine for secure data                                      */
/* ===================================================================== */
VOID SecureDataImage(IMG img, VOID *v)
{
	DEBUGL(LOG("In SecureDataImage\n"));
	if (vcfg->get_secure_data()) {
		DEBUGL(LOG("looking for _vaccs_declare_as_secure_data\n"));
		RTN secure_data_rtn = RTN_FindByName(img, "_vaccs_declare_as_secure_data");

		if (RTN_Valid(secure_data_rtn)) {
			DEBUGL(LOG("Instrumenting _vaccs_delcare_as_secure_data\n"));
			RTN_Open(secure_data_rtn);

			// Instrument malloc() to print the input argument value and the return value.
			RTN_InsertCall(secure_data_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_declare_as_secure_data,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_END);
			RTN_Close(secure_data_rtn);
		}

		DEBUGL(LOG("looking for memset\n"));
		RTN memset_rtn = RTN_FindByName(img, "memset@plt");
		if (RTN_Valid(memset_rtn)) {
			DEBUGL(LOG("Instrumenting memset\n"));
			RTN_Open(memset_rtn);

			RTN_InsertCall(memset_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_clear_set_memset,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			               IARG_END);

			RTN_Close(memset_rtn);
		}

		DEBUGL(LOG("looking for memcpy\n"));
		RTN memcpy_rtn = RTN_FindByName(img, "memcpy@plt");
		if (RTN_Valid(memcpy_rtn)) {
			DEBUGL(LOG("Instrumenting memcpy\n"));
			RTN_Open(memcpy_rtn);

			RTN_InsertCall(memcpy_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_clear_set_memcpy,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			               IARG_END);

			RTN_Close(memcpy_rtn);
		}

		DEBUGL(LOG("looking for bzero\n"));
		RTN bzero_rtn = RTN_FindByName(img, "bzero@plt");
		if (RTN_Valid(bzero_rtn)) {
			DEBUGL(LOG("Instrumenting bzero\n"));
			RTN_Open(bzero_rtn);

			RTN_InsertCall(bzero_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_clear_set_bzero,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_END);

			RTN_Close(bzero_rtn);
		}

		DEBUGL(LOG("looking for bcopy\n"));
		RTN bcopy_rtn = RTN_FindByName(img, "bcopy@plt");
		if (RTN_Valid(bcopy_rtn)) {
			DEBUGL(LOG("Instrumenting bcopy\n"));
			RTN_Open(bcopy_rtn);

			RTN_InsertCall(bcopy_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_clear_set_bcopy,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			               IARG_END);

			RTN_Close(bcopy_rtn);
		}

		DEBUGL(LOG("looking for mlock\n"));
		RTN mlock_rtn = RTN_FindByName(img, "mlock@plt");
		if (RTN_Valid(mlock_rtn)) {
			DEBUGL(LOG("Instrumenting mlock\n"));
			RTN_Open(mlock_rtn);

			RTN_InsertCall(mlock_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_lock,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_END);

			RTN_Close(mlock_rtn);
		}

		DEBUGL(LOG("looking for munlock\n"));
		RTN munlock_rtn = RTN_FindByName(img, "munlock@plt");
		if (RTN_Valid(munlock_rtn)) {
			DEBUGL(LOG("Instrumenting munlock\n"));
			RTN_Open(munlock_rtn);

			RTN_InsertCall(munlock_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_unlock,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_END);

			RTN_Close(munlock_rtn);
		}

		DEBUGL(LOG("looking for setrlimit\n"));
		RTN setrlimit_rtn = RTN_FindByName(img, "setrlimit@plt");
		if (RTN_Valid(setrlimit_rtn)) {
			DEBUGL(LOG("Instrumenting setrlimit\n"));
			RTN_Open(setrlimit_rtn);

			RTN_InsertCall(setrlimit_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_sd_czero,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			               IARG_END);

			RTN_Close(setrlimit_rtn);
		}
	}
	DEBUGL(LOG("Exit SecureDataImage\n"));
}
