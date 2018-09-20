/******************************************************************************
 *
 * File: cu_table.cpp
 *
 * Summary: This file contains the code for building records of debug
 *          information for a compilation unit
 *
 *
 ******************************************************************************/

#include <tables/cu_table.h>
#include <tables/type_table.h>

#include <tables/symbol_table_factory.h>

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <list>

using namespace std;

cu_record::cu_record() :
		symbol_table_record(CU_RECORD) {
	vtab = NULL;
	ttab = NULL;
	low_pc = -1;
	high_pc = -1;
}

cu_record::~cu_record() {}

/**
 * Check if a pc is in this compilation unit
 *
 * @param pc the pc to check
 * @return true if the pc is found in this compilation unit, otherwise false.
 */
bool cu_record::pc_in_range(Generic pc) {
	DEBUGL(LOG("In cu_record::pc_in_range: check if pc = "+hexstr(pc)+" is between "+hexstr(low_pc+text_base_address)+" and "+hexstr(high_pc+text_base_address)+"\n"));
	return (pc >= (low_pc + text_base_address) && pc < (high_pc + text_base_address));
}

/**
 * Translate a memory access to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @return a key,var_record* pair that matches the given address, or default_var_pair
 */
pair<string,var_record*> cu_record::translate_address_to_function(Generic inst_addr) {
	pair<string,var_record*> vpair = default_var_pair;

	// look for the subprogram with that contains the given instruction
	// then look for a
	for (map<string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
		pair<string,var_record*> tvpair(it->first,(var_record*)it->second);
			if (tvpair.second->get_is_subprog() && tvpair.second->pc_in_range(inst_addr)) {
				vpair = tvpair;
				break;
			}
	}

	return vpair;
}

/**
 * Translate a memory access to an list of variables that point to it
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
list<pair<string,var_record*>> *
cu_record::translate_address_to_pointer_list(const CONTEXT *ctxt, Generic mem_addr) {
	list<pair<string,var_record*>> *pointer_list = new list<pair<string,var_record*>>();

	DEBUGL(LOG("In cu_record::translate_address_to_pointer\n"));

	// look in subprograms for local variables that point to the address
	for (map<string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
		pair<string,var_record*> tvpair(it->first,(var_record*)it->second);
			DEBUGL(LOG("Checking if " + it->first + " is a subprogram\n"));
			if (tvpair.second->get_is_subprog()) {
				DEBUGL(LOG("Checking subprogram "+it->first+" for the address " + hexstr(mem_addr)));
				list<pair<string,var_record*>> *tpl = tvpair.second->find_pointers_to_address_in_subprog(ctxt,mem_addr,ttab);
				pointer_list->splice(pointer_list->end(),*tpl);
				delete tpl;
			}
	}

	return pointer_list;
}

/**
 * Translate a instruction address to a function
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
pair<string,var_record*> cu_record::translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr) {
	pair<string,var_record*> vpair = default_var_pair;

	DEBUGL(LOG("In cu_record::translate_address_to_variable\n"));

	// look for the subprogram with that contains the given instruction
	// then look for a
	for (map<string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
		pair<string,var_record*> tvpair(it->first,(var_record*)it->second);
			DEBUGL(LOG("Checking if " + it->first + " is a subprogram\n"));
			if (tvpair.second->get_is_subprog() && tvpair.second->pc_in_range(inst_addr)) {
				DEBUGL(LOG("Checking subprogram "+it->first+" for the address " + hexstr(mem_addr)));
				vpair = tvpair.second->find_address_in_subprog(ctxt,mem_addr,ttab);
				break;
			}
	}

	return vpair;
}

/**
 * Get the scope name of a var_record
 *
 * @param vrec a variable record
 * @return a string containing the name of the scope (function name or *G*)
 */
string cu_record::get_scope(var_record *vrec) {
	string scope("");
	for (map<string,symbol_table_record*>::iterator it = vtab->begin();
			scope.empty() && it != vtab->end();
			it++) {
		var_record* tvrec = (var_record*)it->second;
		if (tvrec == vrec)
			scope = "*G*";
	}

	if (scope.empty())
		for (map<string,symbol_table_record*>::iterator it = vtab->begin();
				scope.empty() && it != vtab->end();
				it++) {
			var_record* tvrec = (var_record*)it->second;
			if (tvrec->get_is_subprog() && tvrec->get_scope(vrec))
				scope = it->first;
		}

	return scope;
}

/**
 * Create the member tables from the type declarations for local variables that are structures.
 *
 */
void cu_record::create_member_tables() {

	vtab->create_member_tables(ttab);
}

/**
 * Write a compilation unit record to a file
 *
 * @param fp a file pointer
 */
void cu_record::write(string key,NATIVE_FD fd) {
	DEBUGL(LOG("Begin cu_record::write()\n"));

	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size_t length = key.length();
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,key.c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(low_pc); assert(OS_WriteFD(fd,&low_pc,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(high_pc); assert(OS_WriteFD(fd,&high_pc,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	ttab->write(fd);
	vtab->write(fd);

	DEBUGL(LOG("End cu_record::write()\n"));

}

cu_table::~cu_table() {}

/**
 * Translate a memory access to an list of variables that point to it
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
list<pair<string,var_record*>> *
cu_table::translate_address_to_pointer_list(const CONTEXT *ctxt, Generic mem_addr) {
	list<pair<string,var_record*>> *pointer_list = new list<pair<string,var_record*>>();

	DEBUGL(LOG("In cu_table::translate_address_to_pointer\n"));
	// Look for a local variable with the given address
	for (map<string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		DEBUGL(LOG("Checking cu "+it->first+"\n"));
		list<pair<string,var_record*>> *tpl = curec->translate_address_to_pointer_list(ctxt,mem_addr);
		pointer_list->splice(pointer_list->end(), *tpl);
		delete tpl;
	}

	//  Look for global variables that point to the address

	for (map<string,symbol_table_record*>::iterator it = begin();
			it != end();
			it++) {
		cu_record* curec = (cu_record*)it->second;
		for (map<string,symbol_table_record*>::iterator it = curec->get_var_table()->begin();
				it != curec->get_var_table()->end();
				it++) {
			pair<string,var_record*> *vpair = new pair<string,var_record*>(it->first,(var_record*)it->second);
			type_record *ttrec = curec->get_type_table()->get(vpair->second->get_type());
			DEBUGL(LOG("Checking global variable "+it->first+" if points to address "+hexstr(mem_addr)+"\n"));
			if (ttrec->get_is_pointer() && vpair->second->points_to_address(ctxt,mem_addr,ttrec)) {
				DEBUGL(LOG(it->first+" points to address "+hexstr(mem_addr)));
				pointer_list->push_back(*vpair);
			}
		}
	}

	return pointer_list;
}

/**
 * Translate a memory access to a variable name
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
pair<string,var_record*> cu_table::translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr) {
	pair<string,var_record*> vpair = default_var_pair;

	DEBUGL(LOG("In cu_table::translate_address_to_variable\n"));
	// Look for a local variable with the given address
	for (map<string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		DEBUGL(LOG("Checking cu "+it->first+"\n"));
		if (curec->pc_in_range(inst_addr)) {
			DEBUGL(LOG("Found the CU for this instruction"));
			vpair = curec->translate_address_to_variable(ctxt,inst_addr,mem_addr);
			break;
		}
	}

	// if there is no local variable, look for a global variable with that address

	for (map<string,symbol_table_record*>::iterator it = begin();
			vpair == default_var_pair && it != end();
			it++) {
		cu_record* curec = (cu_record*)it->second;
		for (map<string,symbol_table_record*>::iterator it = curec->get_var_table()->begin();
				it != curec->get_var_table()->end();
				it++) {
			var_record *tvrec = (var_record*)it->second;
			type_record *ttrec = curec->get_type_table()->get(tvrec->get_type());
			DEBUGL(LOG("Checking global variable "+it->first+" if at address "+hexstr(mem_addr)));
			if (!tvrec->get_is_subprog() && tvrec->is_at_address(ctxt,mem_addr,ttrec)) {
				vpair.first = it->first;
				vpair.second = tvrec;
				break;
			}
		}
	}

	return vpair;
}

/**
 * Translate an instruction address to a function
 *
 * @param inst_addr the address of the instruction making the memory access
 * @return a key,var_record* pair that matches the given address, or default_var_pair
 */
pair<string,var_record*> cu_table::translate_address_to_function(Generic inst_addr) {
	pair<string,var_record*> vpair = default_var_pair;

	// Look for a local variable with the given address
	for (map<string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		if (curec->pc_in_range(inst_addr)) {
			vpair = curec->translate_address_to_function(inst_addr);
			break;
		}
	}

	return vpair;
}

/**
 * Get the scope name of a var_record
 *
 * @param vrec a variable record
 * @return a string containing the name of the scope (function name or *G*)
 */
string cu_table::get_scope(var_record *vrec) {
   string scope("");
   for (map<string,symbol_table_record*>::iterator it = begin();
         scope.empty() && it != end();
         it++) {
      cu_record* curec = (cu_record*)it->second;
      scope = curec->get_scope(vrec);
   }

   return scope;
}

/**
 * Get the type record of a type given the dwarf index
 *
 * @param dw_index a dwarf index for a type (string)
 * @return a pointer to a type_record for the give dwarf index
 */
type_record *cu_table::get_type_record(string dw_index) {
	type_record *trec = NULL;
	for (map<string,symbol_table_record*>::iterator it = begin();
			trec == NULL && it != end();
			it++) {
		cu_record* curec = (cu_record*)it->second;
		type_record *ttrec = curec->get_type_table()->get(dw_index);
		if (ttrec != NULL)
		   trec = ttrec;
	}

	return trec;
}

/**
 * Get the type table containing a type given the dwarf index
 *
 * @param dw_index a dwarf index for a type (string)
 * @return a pointer to a type_table containing the give dwarf index
 */
type_table *cu_table::get_type_table(string dw_index) {
	type_table *ttab = NULL;
	for (map<string,symbol_table_record*>::iterator it = begin();
			ttab == NULL && it != end();
			it++) {
		cu_record* curec = (cu_record*)it->second;
		type_record *ttrec = curec->get_type_table()->get(dw_index);
		if (ttrec != NULL)
		   ttab = curec->get_type_table();
	}

	return ttab;
}

/**
 * Create the member tables from the type declarations for structures.
 *
 */
void cu_table::create_member_tables() {



	// Create member tables for local variables
	for (map<string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		curec->create_member_tables();
	}
}

/**
 * Write the compilation unit table to a file
 *
 * @param fp a file pointer
 */
void cu_table::write(NATIVE_FD fd) {
	DEBUGL(LOG("Begin cu_table::write()\n"));
	USIZE dsize =  sizeof(id); assert(OS_WriteFD(fd,&id,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t num = size();
	dsize = sizeof(size_t); assert(OS_WriteFD(fd,&num,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	for (map<string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		curec->write(it->first,fd);
	}
	DEBUGL(LOG("End cu_table::write()\n"));
}


