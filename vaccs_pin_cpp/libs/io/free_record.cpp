//
//  free_record.cpp
//  libs
//
//  Created by Steven M. Carr on 12/10/2019.
//
//

#include <io/free_record.h>
#include <util/c_string_utils.h>

free_record::free_record() :
		vaccs_record(VACCS_FREE)  {
  event_num = -1;
  address = -1;
  c_line_num = -1;
  c_file_name = NULL;
}

free_record::~free_record() {}

/**
 * Write a free record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void free_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_FREE;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_WriteFD(fd,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(address); assert(OS_WriteFD(fd,&address,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

}

/**
 * Read a free record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a free record
 */
vaccs_record *free_record::read(NATIVE_FD fd) {

	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_ReadFD(fd,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(address); assert(OS_ReadFD(fd,&size,&address).generic_err == OS_RETURN_CODE_NO_ERROR);

	return this;
}
