/*
 * line_change_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/line_change_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
line_change_record::line_change_record() :
                                         vaccs_record(VACCS_LINE_CHANGE)
{
  event_num = -1;
  c_line_num = -1;
  c_file_name = NULL;
}

/**
 * Constructor
 */
line_change_record::~line_change_record()
{
  sfree(c_file_name);
}

/**
 * Write an line_change record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void line_change_record::write(NATIVE_FD fd)
{
  vaccs_id_t id = VACCS_LINE_CHANGE;
  USIZE size =  sizeof(id); assert(OS_WriteFD(fd, &id, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(event_num); assert(OS_WriteFD(fd, &event_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  sizeof(c_line_num); assert(OS_WriteFD(fd, &c_line_num, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  assert((length = strnlen(c_file_name, PATH_MAX + 1)) <= PATH_MAX);
  size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
  size =  length; assert(OS_WriteFD(fd, c_file_name, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

}

/**
 * Read an line_change record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an line_change record
 */
vaccs_record *line_change_record::read(NATIVE_FD fd)
{

  USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd, &size, &event_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size =  sizeof(c_line_num); assert(OS_ReadFD(fd, &size, &c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

  size_t length;
  size =  sizeof(length); assert(OS_ReadFD(fd, &size, &length).generic_err == OS_RETURN_CODE_NO_ERROR);
  assert(length <= PATH_MAX);
  assert((c_file_name = (char*)malloc(length + 1)) != NULL);
  size =  length; assert(OS_ReadFD(fd, &size, c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
  c_file_name[length] = '\0';

  return this;
}

