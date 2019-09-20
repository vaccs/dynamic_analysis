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
#include <string>
#include <map>
#include <tables/frame.h>

extern runtime_stack *stack_model;

void write_element_record(cu_table *cutab, pair<string,var_record*> vpair, const CONTEXT *ctxt,
     ADDRINT addr, INT32 line, string fileName, string var_prefix,
     string scope, int event_num);

/* ===================================================================== */
/* Instrumentation the memory operation to record the memory accesses     */
/* ===================================================================== */
ofstream* output;
char* get_address_dereference(char* string){
	int length = strlen(string);
	char* output = (char*) malloc(length+1);

	int start = -1;
	int end = -1;

	for(int i=0;i<length;i++){
		if(string[i]=='['){
			start = i;
		}

		else if(string[i] == ']'){
			end = i;
			break;
		}

	}
	if(start == -1 || end == -1) return 0;
	strncpy(output,string+start+1,end-start-1);
	return output;
}
bool is_indirect(char* assembly){
	if(get_address_dereference(assembly)!=0){
		if(strchr(get_address_dereference(assembly),'*')!=NULL) return true;
	}
	return false;
}
// Print a memory read record
ADDRINT current_EBP;
PIN_LOCK lock;
VOID BeforeMemRead(VOID* assembly,ADDRINT ip, VOID *addr,const CONTEXT *ctxt,UINT32 size) {
    char* assembly_code = (char*) assembly;
    assembly_code = process_string_for_csv(assembly_code);
    DEBUGL(LOG(hexstr(ip)+ "R "+hexstr(addr)+" "+decstr(size)+"\n"));

    int id = timestamp++;

    get_registers(ctxt,id);

    INT32 column;
    INT32 line;
    string 	fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip,&column,&line,&fileName);
    PIN_UnlockClient();

    DEBUGL(LOG("line number "+decstr(line)+"\n"));
    current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);

    char* location = (char*) malloc(size);
    DEBUGL( LOG( "value before read :"));
    if(is_indirect(assembly_code)) {

	for(unsigned int i=0;i<size;i++) {
	    location[i] = ((char*)addr)[i];
    	    DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
    	 }

    }
    else {
	for(unsigned int i=0;i<size;i++){
	   location[i] = ((char*)addr)[i];
	   DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
    	}
    }


    print_global_context();


}

VOID AfterMemRead(VOID * ip, VOID *addr,const CONTEXT *ctxt,UINT32 size) {

    char* location = (char*) malloc(size);
    DEBUGL(LOG( "value after read :"));

    for(unsigned int i=0;i<size;i++){
    	location[i] = ((char*)addr)[i];
    	DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
    }
    DEBUGL(LOG( "\n"));

}

// Print a memory write record
VOID BeforeMemWrite(VOID* assembly,ADDRINT  ip, VOID *addr,const CONTEXT *ctxt, UINT32 size) {

    INT32	column;
    INT32	line;
    string 	fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip,&column,&line,&fileName);
    PIN_UnlockClient();

    char* assembly_code = (char*) assembly;
    assembly_code = process_string_for_csv(assembly_code);

    DEBUGL(LOG(hexstr(ip)+": W "+hexstr(addr)+" "+decstr(size) +"\n"));

    int id = timestamp++;
    get_registers(ctxt,id);

    current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
    if(is_indirect(assembly_code)) {
	char* location = (char*) malloc(size);
    	DEBUGL(LOG( "value before write :"));
    	for(unsigned int i=0;i<size;i++){
	   location[i] = ((char*)addr)[i];
    	   DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
    	}

    	DEBUGL(LOG( "\n"));
    }
    else {
    	char* location = (char*) malloc(size);
    	DEBUGL(LOG( "value before write :"));
    	for(unsigned int i=0;i<size;i++){
	   location[i] = ((char*)addr)[i];
    	   DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
    	}

    	DEBUGL(LOG( "\n"));
    }


}

void write_variable_access_record(string variable, Generic event_num, INT32 line, string fileName,
				  string scope, ADDRINT addr, string type_name, string value) { 

   vaccs_record_factory factory;
   var_access_record *varec;

   DEBUGL(LOG( "Write to variable " + variable + "\n"));
   DEBUGL(LOG( "\tEvent num: " + decstr(event_num) + "\n"));
   DEBUGL(LOG( "\tC file name: " + fileName + "\n"));
   DEBUGL(LOG( "\tScope: " + scope + "\n"));
   DEBUGL(LOG( "\tAddress: " + hexstr((Generic)addr) + "\n"));
   DEBUGL(LOG( "\tType: " + type_name + "\n"));
   DEBUGL(LOG( "\tValue: " + value + "\n\n"));

   varec = (var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS);
   varec = varec->add_event_num(event_num)
      ->add_c_line_num(line)
      ->add_c_file_name(fileName.c_str())
      ->add_scope(scope.c_str())
      ->add_address((Generic)addr)
      ->add_name(variable.c_str())
      ->add_type(type_name.c_str())
      ->add_value(value.c_str());

   if (type_name.find("*") != string::npos)
	   varec = varec->add_points_to(addr);
   varec->write(vaccs_fd);
   delete varec;
}

void write_pointer_access(string variable, var_record *vrec, cu_table *cutab, const CONTEXT *ctxt,
			  Generic event_num, INT32 line, string fileName, ADDRINT addr) {

    type_table *ttab = cutab->get_type_table(vrec->get_type());
    type_record *trec = ttab->get(vrec->get_type());
    type_record *btrec = ttab->get(*trec->get_base_type());

    string scope = cutab->get_scope(vrec);
    Generic var_addr = vrec->get_base_address(ctxt);
    string value = vrec->read_value(ttab,btrec,addr);
    string type_name = *trec->get_name();

    vaccs_record_factory factory;
    var_access_record *varec;

    DEBUGL(LOG( "Write to variable " + variable + "\n"));
    DEBUGL(LOG( "\tEvent num: " + decstr(event_num) + "\n"));
    DEBUGL(LOG( "\tC file name: " + fileName + "\n"));
    DEBUGL(LOG( "\tScope: " + scope + "\n"));
    DEBUGL(LOG( "\tAddress: " + hexstr((Generic)var_addr) + "\n"));
    DEBUGL(LOG( "\tPoints to Address: " + hexstr((Generic)addr) + "\n"));
    DEBUGL(LOG( "\tType: " + type_name + "\n"));
    DEBUGL(LOG( "\tValue: " + value + "\n\n"));

    varec = (var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS);
    varec = varec->add_event_num(event_num)
	->add_c_line_num(line)
	->add_c_file_name(fileName.c_str())
	->add_scope(scope.c_str())
	->add_address(var_addr)
	->add_name(variable.c_str())
	->add_type(type_name.c_str())
	->add_value(value.c_str())
	->add_points_to(addr);

    varec->write(vaccs_fd);
    delete varec;
}

void write_array_element_record(cu_table *cutab, pair<string,var_record*> vpair, const CONTEXT *ctxt, ADDRINT addr,
				INT32 line, string fileName, string var_prefix, string scope, int event_num) {

   DEBUGL(LOG( "Write to array element for " + vpair.first + "\nprefix = "+var_prefix + "\n"));

   type_table *ttab =cutab->get_type_table(vpair.second->get_type());
   type_record *trec = ttab->get(vpair.second->get_type());

   Generic element_size = ttab->get(*trec->get_base_type())->get_size();
   symbol_table_record_factory factory;

   if (vpair.second->is_first_access()) {

      //
      // On first access write out the entire array so the visualization has the inital values
      //
      vpair.second->clear_first_access();

      write_variable_access_record(var_prefix + vpair.first, event_num, line, fileName, scope,
            addr, *trec->get_name(), "<multielement>");

      for (unsigned int i = 0; i <= trec->get_upper_bound(); i++) {
         string base_type = *ttab->get(vpair.second->get_type())->get_base_type();
         var_record *varec = (var_record*)factory.make_symbol_table_record(VAR_RECORD);
   	   varec = varec->add_decl_file(fileName)
	   	   ->add_decl_line(vpair.second->get_decl_line())
         ->add_type(base_type)
		     ->add_location(vpair.second->get_location() + (element_size * i));

         if (vpair.second->get_is_local())
            varec = varec->add_is_local();
         else if (vpair.second->get_is_param())
            varec = varec->add_is_param();

         pair<string,var_record*> nvpair(vpair.first + ":[" + decstr(i) + "]",varec);

         write_element_record(cutab,nvpair,ctxt,addr+(element_size * i),line,fileName,
            var_prefix,scope,event_num);

         delete varec;
      }
    } else {
         int index = (addr - vpair.second->get_base_address(ctxt))/element_size;
         DEBUGL(LOG("Computed index = "+decstr(index) + "\n"));
         var_record *varec = (var_record*)factory.make_symbol_table_record(VAR_RECORD);
         string base_type = *ttab->get(vpair.second->get_type())->get_base_type();
   	     varec = varec->add_decl_file(fileName)
	   	     ->add_decl_line(vpair.second->get_decl_line())
           ->add_type(base_type)
		       ->add_location(vpair.second->get_location() + (element_size * index));

         if (vpair.second->get_is_local())
            varec = varec->add_is_local();
         else if (vpair.second->get_is_param())
            varec = varec->add_is_param();

         pair<string,var_record*> mvpair(vpair.first + ":[" + decstr(index) + "]",varec);

         write_element_record(cutab,mvpair,ctxt,addr,line,fileName,
           var_prefix,scope,event_num);
    }
}

void write_struct_record(cu_table *cutab, pair<string,var_record*> vpair, const CONTEXT *ctxt, ADDRINT addr,
			 INT32 line, string fileName, string var_prefix, string scope, int event_num) {

   DEBUGL(LOG( "Write to structure element for " + vpair.first + " at address " + hexstr(addr)));
   DEBUGL(LOG("\nprefix = " + var_prefix + "\n"));

   type_table *ttab =cutab->get_type_table(vpair.second->get_type());
   type_record *trec = ttab->get(vpair.second->get_type());

   var_table *memtab = vpair.second->get_member_table();
   if (vpair.second->is_first_access()) {

      //
      // On first access write out the entire structure so the visualization has the inital values
      //

      vpair.second->clear_first_access();

      write_variable_access_record(var_prefix + vpair.first, event_num, line, fileName, scope,
				   addr, *trec->get_name(), "<multielement>");


      Generic member_address = addr;

      for (std::map<std::string,symbol_table_record*>::iterator it = memtab->begin();
	   it != memtab->end();
	   it++) {

         pair<string,var_record*> mpair(it->first,(var_record*)it->second);
			write_element_record(cutab,mpair,ctxt,member_address,line,fileName,
					     *trec->get_name() + ":",scope,event_num);

         member_address += ttab->get(mpair.second->get_type())->get_size();
      }
   } else {

	for (std::map<std::string,symbol_table_record*>::iterator it = memtab->begin();
	     it != memtab->end();
	     it++) {

        pair<string,var_record*> mpair(it->first,(var_record *)it->second);
			type_record *mtrec = ttab->get(mpair.second->get_type());

        if (mpair.second->is_at_address(ctxt, addr , mtrec)) {
            write_element_record(cutab,mpair,ctxt,addr,line,fileName, *trec->get_name() + ":",scope,event_num);
            break;
         }
      }
   }
}

void write_element_record(cu_table *cutab, pair<string,var_record*> vpair, const CONTEXT *ctxt, ADDRINT addr,
			  INT32 line, string fileName, string var_prefix, string scope, int event_num) {

   DEBUGL(LOG( "Write to address " + hexstr(addr)+ "is variable " + vpair.first + "prefix = " + var_prefix+"\n"));

   type_table *ttab =cutab->get_type_table(vpair.second->get_type());
   type_record *trec = ttab->get(vpair.second->get_type());

   if (trec->get_is_array())
      write_array_element_record(cutab,vpair,ctxt,addr,line,fileName, var_prefix,scope, event_num);
   else if (trec->get_is_struct())
      write_struct_record(cutab,vpair,ctxt,addr,line,fileName,var_prefix,scope, event_num);
   else
      write_variable_access_record(var_prefix+vpair.first, event_num, line, fileName, scope,
				   addr, *trec->get_name(), vpair.second->read_value(ttab,trec,addr));
}

VOID AfterMemWrite(VOID* assembly, ADDRINT ip, ADDRINT addr,const CONTEXT *ctxt, UINT32 size) {
	 
    char* assembly_code = (char*) assembly;
    char* location = (char*) malloc(size);

    current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
    DEBUGL(LOG( "value after write :"));

    if(is_indirect(assembly_code)) {

	for(unsigned int i=0;i<size;i++){
	    location[i] = ((char*)addr)[i];
    	    DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
	}
	DEBUGL(LOG( "\n"));
    } else {

	for(unsigned int i=0;i<size;i++){
	    location[i] = ((char*)addr)[i];
    	    DEBUGL(LOG( hexstr(location[i] & 0xff) + " " ));
	}
	DEBUGL(LOG( "\n"));
    }

    INT32	column;
    INT32	line;
    string 	fileName;

    PIN_LockClient();
    PIN_GetSourceLocation(ip,&column,&line,&fileName);
    PIN_UnlockClient();

    if (line == 0)
	fileName = NOCSOURCE;

    DEBUGL(LOG("line = " + decstr(line) + ", column = " + decstr(column) + ", file = " + fileName));

    cu_table *cutab = vdr->get_cutab();
    cu_record *curec = cutab->get(ip);

    if (curec == NULL) {
      DEBUGL(LOG("ip = " + hexstr(ip) + " is not in user code"));
    }
    else {
      DEBUGL(LOG("ip = " + hexstr(ip) + " is in user code checking variable access"));

      list<var_upd_record*> *vlist = stack_model->addr_get_updated_variables((Generic)addr,cutab);
      vlist->splice(vlist->end(),*stack_model->get_all_updated_points_to(cutab));

      if (vlist->empty()) {
  	     DEBUGL(LOG("there are no live variables"));
      } else {

      	for (list<var_upd_record*>::iterator it = vlist->begin(); it != vlist->end(); it++) {
	    var_upd_record* vurec = *it;
    	    pair<string,var_record*> vpair(vurec->get_variable_name(),vurec->get_var_record());
    	    write_element_record(cutab,vpair,vurec->get_context(),vurec->get_address(),line,fileName,"",
    				 cutab->get_scope(vpair.second),timestamp);
    	}
      }
      timestamp++;
    }
}

// Is called for every instruction and instruments reads and writes
VOID MemoryAccessInstruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.

				UINT32 memOperands = INS_MemoryOperandCount(ins);
				char* assembly_code = (char*) malloc(strlen(INS_Disassemble(ins).c_str())+1);
				strcpy(assembly_code,INS_Disassemble(ins).c_str());
				// Iterate over each memory operand of the instruction.
				for (UINT32 memOp = 0; memOp < memOperands; memOp++)
				{
					if (INS_MemoryOperandIsRead(ins, memOp))
					{

				        INS_InsertPredicatedCall(
				                ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemRead,
				                IARG_PTR,assembly_code,
				                IARG_INST_PTR,
				                IARG_MEMORYOP_EA, memOp	,
								IARG_CONTEXT,
								IARG_MEMORYREAD_SIZE,
				                IARG_END);
//				        IPOINT where = IPOINT_AFTER;
//				        if (!INS_HasFallThrough(ins))
//				        	where = IPOINT_TAKEN_BRANCH;
//				        INS_InsertPredicatedCall(
//				        				                ins, where, (AFUNPTR)AfterMemRead,
//				        				                IARG_INST_PTR,
//				        				                IARG_MEMORYOP_EA, memOp	,IARG_CONTEXT,IARG_MEMORYREAD_SIZE,
//				        				                IARG_END);

				        }

				        // Note that in some architectures a single memory operand can be
				        // both read and written (for instance incl (%eax) on IA-32)
				        // In that case we instrument it once for read and once for write.
				        if (INS_MemoryOperandIsWritten(ins, memOp))
				        {
				            INS_InsertPredicatedCall(
				                ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemWrite,
				                IARG_PTR,assembly_code,
				                IARG_INST_PTR,
				                IARG_MEMORYOP_EA, memOp,
								IARG_CONTEXT,
								IARG_MEMORYWRITE_SIZE,
				                IARG_END);
				            IPOINT where = IPOINT_AFTER;
				            if (!INS_HasFallThrough(ins))
				                        where = IPOINT_TAKEN_BRANCH;
				            INS_InsertPredicatedCall(
				            				                ins, where, (AFUNPTR)AfterMemWrite,
															IARG_PTR,assembly_code,
				            				                IARG_INST_PTR,
				            				                IARG_MEMORYOP_EA,
															memOp,IARG_CONTEXT,
															IARG_MEMORYWRITE_SIZE,
				            				                IARG_END);
				        }
				    }
}
