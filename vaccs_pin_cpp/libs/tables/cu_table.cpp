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
	return (pc >= low_pc && pc < high_pc);
}

/**
 * Translate a memory access to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @return a key,var_record* pair that matches the given address, or default_var_pair
 */
std::pair<std::string,var_record*> cu_record::translate_address_to_function(Generic inst_addr) {
	std::pair<std::string,var_record*> vpair = default_var_pair;

	// look for the subprogram with that contains the given instruction
	// then look for a
	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
		std::pair<std::string,var_record*> tvpair(it->first,(var_record*)it->second);
			if (tvpair.second->get_is_subprog() && tvpair.second->pc_in_range(inst_addr)) {
				vpair = tvpair;
				break;
			}
	}

	return vpair;
}

/**
 * Translate a instruction address to a function
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
std::pair<std::string,var_record*> cu_record::translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr) {
	std::pair<std::string,var_record*> vpair = default_var_pair;

	// look for the subprogram with that contains the given instruction
	// then look for a
	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
		std::pair<std::string,var_record*> tvpair(it->first,(var_record*)it->second);
			if (tvpair.second->get_is_subprog() && tvpair.second->pc_in_range(inst_addr)) {
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
std::string cu_record::get_scope(var_record *vrec) {
	std::string scope("");
	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin();
			scope.empty() && it != vtab->end();
			it++) {
		var_record* tvrec = (var_record*)it->second;
		if (tvrec == vrec)
			scope = "*G*";
	}

	if (scope.empty())
		for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin();
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
void cu_record::write(std::string key,NATIVE_FD fd) {
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
 * Translate a memory access to a variable name
 *
 * @param ctxt a pin process context
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return a key,var_record* pair that matches the given address, or default_var_pai
 */
std::pair<std::string,var_record*> cu_table::translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr) {
	std::pair<std::string,var_record*> vpair = default_var_pair;

	// Look for a local variable with the given address
	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		if (curec->pc_in_range(inst_addr)) {
			vpair = curec->translate_address_to_variable(ctxt,inst_addr,mem_addr);
			break;
		}
	}

	// if there is no local variable, look for a global variable with that address

	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
			vpair == default_var_pair && it != end();
			it++) {
		cu_record* curec = (cu_record*)it->second;
		for (std::map<std::string,symbol_table_record*>::iterator it = curec->get_var_table()->begin();
				it != curec->get_var_table()->end();
				it++) {
			var_record *tvrec = (var_record*)it->second;
			type_record *ttrec = curec->get_type_table()->get(tvrec->get_type());
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
std::pair<std::string,var_record*> cu_table::translate_address_to_function(Generic inst_addr) {
	std::pair<std::string,var_record*> vpair = default_var_pair;

	// Look for a local variable with the given address
	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
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
std::string cu_table::get_scope(var_record *vrec) {
   std::string scope("");
   for (std::map<std::string,symbol_table_record*>::iterator it = begin();
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
type_record *cu_table::get_type_record(std::string dw_index) {
	type_record *trec = NULL;
	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
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
type_table *cu_table::get_type_table(std::string dw_index) {
	type_table *ttab = NULL;
	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
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
	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
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

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		curec->write(it->first,fd);
	}
	DEBUGL(LOG("End cu_table::write()\n"));
}


