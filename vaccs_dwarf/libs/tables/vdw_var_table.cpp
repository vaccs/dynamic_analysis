/******************************************************************************
 *
 * File: var_table.cpp
 *
 * Summary: This file contains the code for building tables for mapping
 *          dwarf variables to their attributes
 *
 *
 ******************************************************************************/

#include "vdw_var_table.h"

#include <util/general.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
vdw_var_record::vdw_var_record() :
		symbol_table_record(VAR_RECORD) {
	is_subprog = false;
	local_var_table = NULL;
	is_local = false;
	is_param = false;
	decl_line = -1;
	low_pc = -1;
	high_pc = -1;
	type = void_type;
	location = 0;
	start_pc = 0;
}

vdw_var_record::~vdw_var_record() {}

/**
 * Check if a pc is in this subprogram
 *
 * @param pc the pc to check
 * @return true if the pc is found in this subprogram, otherwise false.
 */
bool vdw_var_record::pc_in_range(Generic pc) {
	return (pc >= low_pc && pc <= high_pc);
}

/**
 * Find the variable declared in this procedure that has the given address. If there is no
 * local variable, then check the global symbol table for a global variable
 *
 * @param mem_addr the memory address for some variable
 * @return a pointer to the var_record of a variable having the given address, NULL if no such
 * 			variable
 */
vdw_var_record *vdw_var_record::find_address(Generic mem_addr) {
	vdw_var_record* vrec = NULL;
	for (std::map<std::string,symbol_table_record*>::iterator it = local_var_table->begin();
			it != local_var_table->end();
			it++) {
		vdw_var_record* tvrec = (vdw_var_record*)it->second;
	}

	return vrec;

}

/**
 * Write a var table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void vdw_var_record::write(std::string key,FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length = key.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(key.c_str(), length, 1, fp) == 1);
	assert(fwrite(&is_local, sizeof(is_local), 1, fp) == 1);
	assert(fwrite(&is_param, sizeof(is_param), 1, fp) == 1);
	length = decl_file.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(decl_file.c_str(), length, 1, fp) == 1);
	assert(fwrite(&decl_line,sizeof(decl_line),1,fp) == 1);
	assert(fwrite(&low_pc, sizeof(low_pc), 1, fp) == 1);
	assert(fwrite(&high_pc, sizeof(high_pc), 1, fp) == 1);
	assert(fwrite(&start_pc,sizeof(start_pc),1,fp) == 1);
	assert(fwrite(&location, sizeof(location), 1, fp) == 1);
	length = type.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(type.c_str(), length, 1, fp) == 1);
	assert(fwrite(&is_subprog,sizeof(is_subprog),1,fp) == 1);

	if (local_var_table != NULL) {
		local_var_table->write(fp);
	}

}

vdw_var_table::~vdw_var_table() {}

/**
 * Write the var table to a file
 *
 * @param fp a file pointer
 */
void vdw_var_table::write(FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		vdw_var_record* vrec = (vdw_var_record*)it->second;
		vrec->write(it->first,fp);
	}
}
