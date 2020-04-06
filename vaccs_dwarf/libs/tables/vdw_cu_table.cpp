/******************************************************************************
 *
 * File: cu_table.cpp
 *
 * Summary: This file contains the code for building records of debug
 *          information for a compilation unit
 *
 *
 ******************************************************************************/

#include "vdw_cu_table.h"

#include <tables/symbol_table_factory.h>

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "vdw_type_table.h"



vdw_cu_record::vdw_cu_record() :
		symbol_table_record(CU_RECORD) {
	vtab = NULL;
	ttab = NULL;
	low_pc = -1;
	high_pc = -1;
}

vdw_cu_record::~vdw_cu_record() {}

/**
 * Check if a pc is in this compilation unit
 *
 * @param pc the pc to check
 * @return true if the pc is found in this compilation unit, otherwise false.
 */
bool vdw_cu_record::pc_in_range(Generic pc) {
	return (pc >= low_pc && pc <= high_pc);
}

/**
 * Translate a memory access by an instruction in this compilation unit to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return the variable record of the variable being accessed if possible to determine, otherwise
 * 			NULL
 */
vdw_var_record* vdw_cu_record::translate_address_to_variable(Generic inst_addr, Generic mem_addr) {
	vdw_var_record *vrec = NULL;

	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
			vdw_var_record* tvrec = (vdw_var_record*)it->second;
			if (tvrec->get_is_subprog() && tvrec->pc_in_range(inst_addr))
				vrec = tvrec->find_address(mem_addr);
	}

	return vrec;
}

/**
 * Write a compilation unit record to a file
 *
 * @param fp a file pointer
 */
void vdw_cu_record::write(std::string key,FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length = key.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(key.c_str(), length, 1, fp) == 1);
	assert(fwrite(&low_pc, sizeof(low_pc), 1, fp) == 1);
	assert(fwrite(&high_pc, sizeof(high_pc), 1, fp) == 1);

	ttab->write(fp);
	vtab->write(fp);

}

vdw_cu_table::~vdw_cu_table() {}

/**
 * Translate a memory access to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return the variable record of the variable being accessed if possible to determine, otherwise
 * 			NULL
 */
vdw_var_record* vdw_cu_table::translate_address_to_variable(Generic inst_addr, Generic mem_addr) {
	vdw_var_record *vrec = NULL;

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		vdw_cu_record* curec = (vdw_cu_record*)it->second;
		if (curec->pc_in_range(inst_addr)) {
			vrec = curec->translate_address_to_variable(inst_addr,mem_addr);
			break;
		}
	}

	return vrec;
}

/**
 * Write the compilation unit table to a file
 *
 * @param fp a file pointer
 */
void vdw_cu_table::write(FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		vdw_cu_record* curec = (vdw_cu_record*)it->second;
		curec->write(it->first,fp);
	}
}

void vdw_cu_table::resolve_base_types() {
	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		vdw_cu_record *curec = (vdw_cu_record*)it->second;
		curec->get_type_table()->resolve_base_types();
	}
}


