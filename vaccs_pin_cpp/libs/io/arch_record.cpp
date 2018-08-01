/*
 * arch_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/arch_record.h>

/**
 * Constructor
 */
arch_record::arch_record() :
		vaccs_record(VACCS_ARCH) {
	arch_type = -1;
}
/**
 * Write an architecture record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void arch_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_ARCH;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&arch_type, sizeof(arch_type), 1, fp) == 1);
}

/**
 * Read an architecture record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an architecture record
 */
vaccs_record *arch_record::read(FILE *fp) {
	assert(fread(&arch_type, sizeof(vaccs_arch_t), 1, fp) == 1);
	return this;
}

