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
#include "database_access_api.h"
#include <util/general.h>

using namespace std;
/* ===================================================================== */
/* Instrumentation malloc and free calls to record the arguments         */
/* ===================================================================== */

VOID freeBefore(CHAR * name, ADDRINT size)
{

   DEBUGL(LOG("Enter freeBefore\n"));
   DEBUGL(string sname(name));
	DEBUGL(LOG(sname + "(" + decstr(size) + ")" + "\n"));

   DEBUGL(LOG("Exit freeBefore\n"));

}
VOID mallocBefore(CHAR * name, ADDRINT size)
{
	DEBUGL(LOG("enter\n"));
   DEBUGL(string sname(name));
	DEBUGL(LOG(sname + "(" + decstr(size) + ")" + "\n"));
	if(invocation_stack.empty()) return;
   DEBUGL(sname = invocation_stack.top().function_name);
	DEBUGL(LOG("function "+ sname +
         "  Frame pointer was "+hexstr(invocation_stack.top().frame_pointer)+
         "\n"));

	malloc_event.id = timestamp++;


	malloc_event.frame_pointer = current_EBP;

	malloc_event.id_function_invocation_happened_in = current_invocation_id;

	malloc_event.allocated_size = (int) size;

	DEBUGL(LOG("exit\n"));
	//ou_malloc << malloc_event.id << "," << malloc_event.frame_pointer << "," << malloc_event.id_function_invocation_happened_in<<
			//"," << malloc_event.allocated_size + "\n");



}

VOID fopenAfter(ADDRINT ret)
{

	DEBUGL(LOG(" returns " + hexstr(ret) + "\n"));
	if (invocation_stack.empty()) return;
	DEBUGL(string sname = invocation_stack.top().function_name);
	DEBUGL(LOG("function "+sname+"  Frame pointer was "+
            hexstr(current_EBP)+"\n"));
	malloc_event.heap_address = ret;
	store_malloc_transaction(malloc_event);


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
    RTN mallocRtn = RTN_FindByName(img, "malloc");
    if (RTN_Valid(mallocRtn))
    {
        RTN_Open(mallocRtn);

        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)mallocBefore,
                       IARG_ADDRINT, "malloc",
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)fopenAfter,
                       IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);


        RTN_Close(mallocRtn);
    }

    // Find the free() function.
    RTN freeRtn = RTN_FindByName(img, "free");
    if (RTN_Valid(freeRtn))
    {
        RTN_Open(freeRtn);
        // Instrument free() to print the input argument value.
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)freeBefore,
                       IARG_ADDRINT, "free",
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_Close(freeRtn);
    }
}
