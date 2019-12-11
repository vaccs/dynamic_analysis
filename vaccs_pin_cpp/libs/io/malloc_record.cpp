//
//  malloc_record.cpp
//  libs
//
//  Created by Steven M. Carr on 12/10/2019.
//
//

#include <io/malloc_record.h>
#include <util/c_string_utils.h>

malloc_record::malloc_record() :
		vaccs_record(VACCS_MALLOC)  {
  event_num = -1;
  num_bytes = -1;
  start_address = -1;
  c_line_num = -1;
  c_file_name = NULL;
}

malloc_record::~malloc_record() {}

/**
 * Write a malloc record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void malloc_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_MALLOC;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_WriteFD(fd,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(start_address); assert(OS_WriteFD(fd,&start_address,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(num_bytes); assert(OS_WriteFD(fd,&num_bytes,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

}

/**
 * Read a malloc record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a malloc record
 */
vaccs_record *malloc_record::read(NATIVE_FD fd) {

	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_ReadFD(fd,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(start_address); assert(OS_ReadFD(fd,&size,&start_address).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(num_bytes); assert(OS_ReadFD(fd,&size,&num_bytes).generic_err == OS_RETURN_CODE_NO_ERROR);

	return this;
}
