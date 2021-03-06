/*
 * malloc_free.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "malloc_free.h"
#include <iostream>
#include <fstream>
#include "global.h"
#include <util/general.h>
#include <util/vaccs_config.h>
#include <tables/frame.h>
#include <tables/heap.h>

#include <io/malloc_record.h>
#include <io/free_record.h>
#include <io/vaccs_record_factory.h>

using namespace std;

heap_map *heap_m = NULL;
extern vaccs_config *vcfg;
extern list<ADDRINT> call_inst_ip;
extern runtime_stack *stack_model;

static list<malloc_record*> last_malloc;
/* ===================================================================== */
/* Instrumentation malloc and free calls to record the arguments         */
/* ===================================================================== */

VOID free_before(CHAR * name, ADDRINT addr)
{

	INT32 column;
	INT32 line;
	string fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &fileName);
	PIN_UnlockClient();

	if (line != 0) { // if this call came from user code
		DEBUGL(LOG("enter free_before, call ip = " + hexstr(call_inst_ip.front()) +
		           ", addr = " + hexstr(addr) + "\n"));
		DEBUGL(LOG("Call from user code, line = " + decstr(line) + "\n"));
		vaccs_record_factory factory;
		free_record *frec = (free_record*)factory.make_vaccs_record(VACCS_FREE);
		frec = frec->add_event_num(timestamp++)
		       ->add_c_line_num(line)
		       ->add_c_file_name(fileName.c_str())
		       ->add_address(addr);
		frec->write(vaccs_fd);
		delete frec;

		heap_block *hb = heap_m->find_block(addr);
		if (hb != NULL) {
			stack_model->remove_block(hb);
			heap_m->delete_block(addr);
		}
	}

}

VOID malloc_before(CHAR * name, ADDRINT size)
{
	INT32 column;
	INT32 line;
	string fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &fileName);
	PIN_UnlockClient();

	if (line != 0) { // if this call came from user code
		DEBUGL(LOG("enter malloc_before, call ip = " + hexstr(call_inst_ip.front()) +
		           ", size = " + decstr(size) + "\n"));
		DEBUGL(LOG("Call from user code, line = " + decstr(line) + "\n"));
		vaccs_record_factory factory;
		malloc_record *mrec = (malloc_record*)factory.make_vaccs_record(VACCS_MALLOC);
		mrec = mrec->add_event_num(timestamp++)
		       ->add_c_line_num(line)
		       ->add_c_file_name(fileName.c_str())
		       ->add_num_bytes(size);

		last_malloc.push_front(mrec);
	}
}

VOID malloc_after(ADDRINT ret)
{

	if (!last_malloc.empty()) { // if this is a return from a call from user code
		DEBUGL(LOG("enter malloc_after, ret = " + hexstr(ret) + "\n"));
		malloc_record *mrec = last_malloc.front();
		mrec = mrec->add_start_address(ret);
		mrec->write(vaccs_fd);
		delete mrec;

		heap_block *hb = new heap_block();
		hb->add_event_id(mrec->get_event_num())
			->add_size(mrec->get_num_bytes())
			->add_start_address(ret)
			->add_value();
		heap_m->add_block(hb);

		last_malloc.pop_front();
	}
}


/* ===================================================================== */
/* Analysis routine for malloc and free                                  */
/* ===================================================================== */
VOID MallocAndFreeImage(IMG img, VOID *v)
{
	// Instrument the malloc() and free() functions.  Print the input argument
	// of each malloc() or free(), and the return value of malloc().
	//
	//  Find the malloc() function.
	//if(Is_System_Image(img)) return;

	if (vcfg->get_malloc_free()) {
		RTN mallocRtn = RTN_FindByName(img, "malloc");

		if (RTN_Valid(mallocRtn)) {
			RTN_Open(mallocRtn);

			// Instrument malloc() to print the input argument value and the return value.
			RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)malloc_before,
			               IARG_ADDRINT, "malloc",
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_END);
			RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)malloc_after,
			               IARG_FUNCRET_EXITPOINT_VALUE,
			               IARG_END);


			RTN_Close(mallocRtn);
		}
		// Find the free() function.
		RTN freeRtn = RTN_FindByName(img, "free");
		if (RTN_Valid(freeRtn)) {
			RTN_Open(freeRtn);
			// Instrument free() to print the input argument value.
			RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)free_before,
			               IARG_ADDRINT, "free",
			               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			               IARG_END);
			RTN_Close(freeRtn);
		}
	}
}
