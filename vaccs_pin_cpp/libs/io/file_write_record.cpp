/**
 */

#include <io/file_write_record.h>
#include <util/c_string_utils.h>

file_write_record::file_write_record() :
                                       vaccs_record(VACCS_FILE_WRITE)
{
  event_num = -1;
  c_line_num = -1;
  c_file_name = NULL;
  fd = -1;
}

file_write_record::~file_write_record()
{
}

/**
 * Write a file_write record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void file_write_record::write(NATIVE_FD fd)
{
  vaccs_id_t id = VACCS_MALLOC;
  USIZE size =  sizeof(id); assert(OS_WriteFD(fd, &id, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(event_num); assert(OS_WriteFD(fd, &event_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(c_line_num); assert(OS_WriteFD(fd, &c_line_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  assert((length = strnlen(c_file_name, PATH_MAX + 1)) <= PATH_MAX);
  size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  length; assert(OS_WriteFD(fd, c_file_name, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(this->fd); assert(OS_WriteFD(fd, &this->fd, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(this->valid); assert(OS_WriteFD(fd, &this->valid, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

}

/**
 * Read a file_write record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a file_write record
 */
vaccs_record *file_write_record::read(NATIVE_FD fd)
{

  USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd, &size, &event_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(c_line_num); assert(OS_ReadFD(fd, &size, &c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  size =  sizeof(length); assert(OS_ReadFD(fd, &size, &length).generic_err == OS_RETURN_CODE_NO_ERROR);
  assert(length <= PATH_MAX);
  assert((c_file_name = (char*)malloc(length + 1)) != NULL);
  size =  length; assert(OS_ReadFD(fd, &size, c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
  c_file_name[length] = '\0';

  size =  sizeof(this->fd); assert(OS_ReadFD(fd, &size, &this->fd).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(this->valid); assert(OS_ReadFD(fd, &size, &this->valid).generic_err == OS_RETURN_CODE_NO_ERROR);

  return this;
}