//
//  file_open_record.cpp
//  libs
//
//  Created by Steven M. Carr on 12/10/2019.
//
//

#include <io/file_open_record.h>
#include <util/c_string_utils.h>

file_open_record::file_open_record() :
                                     vaccs_record(VACCS_FILE_OPEN)
{
  event_num = -1;
  c_line_num = -1;
  c_file_name = NULL;
  path = NULL;
  fd = -1;
  flags = 0;
  mode = 0;
  is_sym_link = 0;
}

file_open_record::~file_open_record()
{
}

/**
 * Write a file_open record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void file_open_record::write(NATIVE_FD fd)
{
  vaccs_id_t id = VACCS_MALLOC;
  USIZE size =  sizeof(id); assert(OS_WriteFD(fd, &id, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(event_num); assert(OS_WriteFD(fd, &event_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(c_line_num); assert(OS_WriteFD(fd, &c_line_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  assert((length = strnlen(c_file_name, PATH_MAX + 1)) <= PATH_MAX);
  size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  length; assert(OS_WriteFD(fd, c_file_name, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  assert((length = strnlen(path, PATH_MAX + 1)) <= PATH_MAX);
  size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  length; assert(OS_WriteFD(fd, path, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(this->fd); assert(OS_WriteFD(fd, &this->fd, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(flags); assert(OS_WriteFD(fd, &flags, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(mode); assert(OS_WriteFD(fd, &mode, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(is_sym_link); assert(OS_WriteFD(fd, &is_sym_link, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

}

/**
 * Read a file_open record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a file_open record
 */
vaccs_record *file_open_record::read(NATIVE_FD fd)
{

  USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd, &size, &event_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(c_line_num); assert(OS_ReadFD(fd, &size, &c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  size =  sizeof(length); assert(OS_ReadFD(fd, &size, &length).generic_err == OS_RETURN_CODE_NO_ERROR);
  assert(length <= PATH_MAX);
  assert((c_file_name = (char*)malloc(length + 1)) != NULL);
  size =  length; assert(OS_ReadFD(fd, &size, c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
  c_file_name[length] = '\0';

  size =  sizeof(length); assert(OS_ReadFD(fd, &size, &length).generic_err == OS_RETURN_CODE_NO_ERROR);
  assert(length <= PATH_MAX);
  assert((path = (char*)malloc(length + 1)) != NULL);
  size =  length; assert(OS_ReadFD(fd, &size, path).generic_err == OS_RETURN_CODE_NO_ERROR);
  path[length] = '\0';

  size =  sizeof(this->fd); assert(OS_ReadFD(fd, &size, &this->fd).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(flags); assert(OS_ReadFD(fd, &size, &flags).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(mode); assert(OS_ReadFD(fd, &size, &mode).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(is_sym_link); assert(OS_ReadFD(fd, &size, &is_sym_link).generic_err == OS_RETURN_CODE_NO_ERROR);

  return this;
}
