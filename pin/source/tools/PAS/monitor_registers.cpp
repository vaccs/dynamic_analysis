/*
 * memory_access.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "monitor_registers.h"
#include <iostream>
#include "global.h"
#include <util/general.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/register_record.h>
#include <map>
#include <list>
#include <tables/frame.h>
#include <util/general.h>
#include <util/vaccs_config.h>

extern runtime_stack * stack_model;
extern vaccs_config *vcfg;

/* EXTERN(void, write_variable_access_record,
 *     (string variable, Generic event_num, INT32 line, string fileName,
 *      string scope, ADDRINT addr, string type_name, string value, Generic points_to));*/

using namespace std;

VOID
AfterRegMod(ADDRINT ip, CONTEXT * ctxt, REG reg, UINT32 opcode)
{
  INT32 column;
  INT32 line;
  string fileName;

  static map<string, Generic> old_values;

  PIN_LockClient();
  PIN_GetSourceLocation(ip, &column, &line, &fileName);
  PIN_UnlockClient();

  string reg_name = REG_StringShort(reg);
  if (line == 0) {
    if (vcfg->get_user_code_only())
      return;
    fileName = NOCSOURCE;
  } else {
    frame * fr = stack_model->top();
    if (fr != NULL && fr->get_is_before_stack_setup()) {
      DEBUGL(LOG("Before stack setup, line =  " + decstr(line) + "\n"));
      if (fr->in_user_code(ip)) {
        // if we've just entered a function and the stack and frame pointer are now
        // set up, set the initial context properly

        fr->clear_is_before_stack_setup();
        DEBUGL(LOG("After stack setup, start_pc = " + hexstr(fr->get_start_pc())
                   + " ip = " + hexstr(ip) + "\n"));
        fr->add_context(ctxt);

        // Just entering a function
        // Initialize the visualization with records for all variables

        cu_table * cutab = vdr->get_cutab();

        list<var_upd_record *> * vlist = stack_model->addr_get_updated_variables((Generic)0, cutab);
        vlist->splice(vlist->end(), *stack_model->get_all_updated_points_to(cutab));

        if (vlist->empty())
          DEBUGL(LOG("there are no local variables"));
        else {
          for (list<var_upd_record *>::iterator it = vlist->begin(); it != vlist->end(); it++) {
            var_upd_record * vurec = *it;
            vurec->write(vaccs_fd, fileName, line, vdr->get_cutab(), timestamp);
          }
        }

        list<return_addr_record *> * ralist = stack_model->get_updated_links();
        if (ralist->empty())
          DEBUGL(LOG("There were no link updates\n"));
        else {
          DEBUGL(LOG("There were link updates after register update, timestamp = " + decstr(timestamp) + "\n"));
          for (list<return_addr_record *>::iterator it = ralist->begin(); it != ralist->end(); it++) {
            return_addr_record * rarec = *it;
            rarec->write(vaccs_fd);
          }
        }
        timestamp++;
      }
    }
  }

  if (!vcfg->get_monitor_registers())
    return;

  vaccs_record_factory factory;
  PIN_REGISTER value;
  PIN_GetContextRegval(ctxt, reg, (UINT8*)&value);
  Generic regval;

  // No vector registers handled nor floating point registers
  switch (REG_Size(reg)) {
  case 1:
    regval = (Generic)value.byte[0];
    break;
  case 2:
    regval = (Generic)value.word[0];
    break;
  case 4:
    regval = (Generic)value.dword[0];
    break;
  default:
    regval = (Generic)value.qword[0];
    break;
  }

  DEBUGL(LOG("Checking values"));
  map<string, Generic>::iterator it;

  it = old_values.find(reg_name);

  if (it == old_values.end() || it->second != regval) {
    register_record * rrec = (register_record*)factory.make_vaccs_record(VACCS_REGISTER);
    DEBUGL(LOG("Register Record\n"));
    DEBUGL(LOG("\tEvent num: " + decstr(timestamp) + "\n"));
    DEBUGL(LOG("\tLine #: " + decstr(line) + "\n"));
    DEBUGL(LOG("\tFile: " + fileName + "\n"));
    DEBUGL(LOG("\tRegister: " + reg_name + "\n"));
    DEBUGL(LOG("\tValue: 0x" + hexstr(regval) + "\n"));
    rrec = rrec->add_event_num(timestamp++)
           ->add_c_line_num(line)
           ->add_c_file_name(fileName.c_str())
           ->add_register_name(reg_name.c_str())
           ->add_value(regval);

    rrec->write(vaccs_fd);
    delete rrec;

    old_values[reg_name] = regval;
  }
} // AfterRegMod

// Is called for every instruction and instruments all of them that have
// corresponding C code and modify a register
VOID
MonitorRegisterInstruction(INS ins, VOID * v)
{
  if (INS_MaxNumWRegs(ins) > 0) {
    string assembly_code = INS_Disassemble(ins);
    IPOINT where = IPOINT_AFTER;
    if (!INS_HasFallThrough(ins))
      where = IPOINT_TAKEN_BRANCH;
    UINT32 num_wregs = INS_MaxNumWRegs(ins);
    for (UINT32 i = 0; i < num_wregs; i++) {
      REG reg = INS_RegW(ins, i);
      UINT32 opcode = INS_Opcode(ins);
      INS_InsertPredicatedCall(ins, where, (AFUNPTR)AfterRegMod,
                               IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_UINT32,
                               reg, IARG_UINT32, opcode, IARG_END);
    }
  }
}
