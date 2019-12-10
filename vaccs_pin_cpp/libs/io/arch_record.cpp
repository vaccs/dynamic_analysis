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
#include <pin.H>

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
void arch_record::write(NATIVE_FD fp) {
	vaccs_id_t id = VACCS_ARCH;
	USIZE size = sizeof(id);
	assert(OS_WriteFD(fp,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(arch_type);
	assert(OS_WriteFD(fp, &arch_type, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(heap_start);
	assert(OS_WriteFD(fp, &heap_start, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(heap_end);
	assert(OS_WriteFD(fp, &heap_end, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an architecture record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an architecture record
 */
vaccs_record *arch_record::read(NATIVE_FD fp) {
	USIZE size = sizeof(vaccs_arch_t);
	assert(OS_ReadFD(fp, &size, &arch_type).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(OS_ReadFD(fp, &heap_start, &arch_type).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(OS_ReadFD(fp, &heap_end, &arch_type).generic_err == OS_RETURN_CODE_NO_ERROR);
	return this;
}
