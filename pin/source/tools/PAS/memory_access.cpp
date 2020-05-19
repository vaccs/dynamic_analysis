/*
 * memory_access.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "memory_access.h"
#include <iostream>
#include  <iomanip>
#include <fstream>
#include "global.h"
#include <util/general.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/var_access_record.h>
#include <tables/var_table.h>
#include <tables/deref.h>
#include <string>
#include <map>
#include <tables/frame.h>

extern runtime_stack * stack_model;

/* void write_element_record(cu_table *cutab, var_upd_record *vurec, INT32 line,
 *                        string fileName, string var_prefix,  int event_num); */

/* EXTERN(void, write_variable_access_record,
 *     (string variable, Generic event_num, INT32 line, string fileName,
 *      string scope, ADDRINT addr, string type_name, string value, Generic points_to)); */
/* ===================================================================== */
/* Instrumentation the memory operation to record the memory accesses     */
/* ===================================================================== */
ofstream * output;
char *
get_address_dereference(char * string)
{
    int length    = strlen(string);
    char * output = (char *) malloc(length + 1);

    int start = -1;
    int end   = -1;

    for (int i = 0; i < length; i++) {
        if (string[i] == '[') {
            start = i;
        } else if (string[i] == ']') {
            end = i;
            break;
        }
    }
    if (start == -1 || end == -1) return 0;

    strncpy(output, string + start + 1, end - start - 1);
    return output;
}

bool
is_indirect(char * assembly)
{
    if (get_address_dereference(assembly) != 0)
        if (strchr(get_address_dereference(assembly), '*') != NULL) return true;

    return false;
}

// Print a memory read record
ADDRINT current_EBP;
PIN_LOCK lock;

/**
 * For a pointer type only, check if this points to a character array
 *
 * @param ttab type table
 * @param trec a type record for a pointer type
 * @return true if this pointer is to a character array
 */
bool
pointer_is_char_array(type_table * ttab, type_record * trec)
{
    type_record * btrec = ttab->get(*trec->get_base_type());

    return !btrec->get_is_pointer() && !btrec->get_is_array() &&
           btrec->get_name()->find("char") != string::npos;
}

VOID
AfterMemWrite(VOID * assembly, ADDRINT ip, ADDRINT addr, const CONTEXT * ctxt, UINT32 size)
{
    current_EBP = (ADDRINT) PIN_GetContextReg(ctxt, REG_GBP);
    bool is_segv;
    Generic value = dereference_memory((Generic *)addr,&is_segv);
    if  (is_segv) {
      DEBUGL(LOG("Address is written is seg fault. SHOULD NOT HAPPEN\n"));
      return;
    }

    DEBUGL(LOG("value after write to address "+hexstr(addr)+" is :" + hexstr(value)+ "\n"));

    INT32 column;
    INT32 line;
    string fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip, &column, &line, &fileName);
    PIN_UnlockClient();

    if (line == 0)
        fileName = NOCSOURCE;

    DEBUGL(LOG("line = " + decstr(line) + ", column = " + decstr(column) + ", file = "
      + fileName + "\n"));

    cu_table * cutab  = vdr->get_cutab();
    cu_record * curec = cutab->get(ip);

    if (curec == NULL) {
        DEBUGL(LOG("ip = " + hexstr(ip) + " is not in user code\n"));
    } else {
        DEBUGL(LOG("ip = " + hexstr(ip) + " is in user code checking variable access\n"));

        frame * fr = stack_model->top();

        // wait until the stack frame is set up before analyzing memory references
        // otherwise the registers will not be what the dwarf information expects

        if (fr != NULL && !fr->get_is_before_stack_setup()) {
            list<var_upd_record *> * vlist = stack_model->addr_get_updated_variables((Generic) addr, cutab);
            vlist->splice(vlist->end(), *stack_model->get_all_updated_points_to(cutab));

            if (vlist->empty()) {
                DEBUGL(LOG("there are no live variables\n"));
            } else   {
                for (list<var_upd_record *>::iterator it = vlist->begin(); it != vlist->end(); it++) {
                    var_upd_record * vurec = *it;
                    vurec->write(vaccs_fd, fileName, line, cutab, timestamp);
                }
            }

            list<return_addr_record *> * ralist = stack_model->get_updated_links();
            if (ralist->empty()) {
                DEBUGL(LOG("There were no link updates\n"));
            } else   {
                DEBUGL(LOG("There were link updates after memory access, timestamp = "+decstr(timestamp)+
                           ", ip = "+hexstr(ip)+"\n"));
                for (list<return_addr_record *>::iterator it = ralist->begin(); it != ralist->end(); it++) {
                    return_addr_record * rarec = *it;
                    rarec->write(vaccs_fd);
                }
            }
            timestamp++;
        } else {
            DEBUGL(LOG("PC is before stack frame is setup: " + hexstr(ip)));
        }
    }
} // AfterMemWrite

// Is called for every instruction and instruments reads and writes
VOID
MemoryAccessInstruction(INS ins, VOID * v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.

    UINT32 memOperands   = INS_MemoryOperandCount(ins);
    char * assembly_code = (char *) malloc(strlen(INS_Disassemble(ins).c_str()) + 1);

    strcpy(assembly_code, INS_Disassemble(ins).c_str());
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
                ins, where, (AFUNPTR) AfterMemWrite,
                IARG_PTR, assembly_code,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA,
                memOp, IARG_CONST_CONTEXT,
                IARG_MEMORYWRITE_SIZE,
                IARG_END);
        }
    }
}
