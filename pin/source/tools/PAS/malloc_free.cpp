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
#include <tables/heap.h>

using namespace std;

static HeapBlock *new_hb = NULL;
HeapMap *heapMap = NULL;

/* ===================================================================== */
/* Instrumentation malloc and free calls to record the arguments         */
/* ===================================================================== */

VOID freeBefore(CHAR * name, ADDRINT addr)
{

   DEBUGL(LOG("entern free\n"));
   heapMap->delete_block(addr);
}
VOID mallocBefore(CHAR * name, ADDRINT size)
{
	DEBUGL(LOG("enter malloc\n"));
        new_hb = (new HeapBlock())->add_event_id(timestamp++)
                     ->add_size((Generic)size);
	DEBUGL(LOG("exit\n"));



}

VOID malloc_after(ADDRINT ret)
{

   DEBUGL(LOG("malloc returns " + hexstr(ret) + "\n"));
   new_hb->add_start_address((Generic)ret);
   heapMap->add_block(new_hb);
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
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)malloc_after,
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
