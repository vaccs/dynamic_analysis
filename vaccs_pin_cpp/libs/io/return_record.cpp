/*
 * return_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/return_record.h>

/**
 * Constructor
 */
return_record::return_record() :
		vaccs_record(VACCS_RETURN) {
	event_num = -1;
}
/**
 * Write a return record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void return_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_RETURN;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a return record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a return record
 */
vaccs_record *return_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	return this;
}

