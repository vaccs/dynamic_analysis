/**
 * fd_table.cpp
 *
 * This file contains the implementation of a table for file descriptor information
 *
 */

#include <tables/fd_table.h>
#include <util/general.h>

fd_table::~fd_table()
{
}

bool fd_table::put(long key, fd_record* rec)
{
  return this->insert(std::pair<long, fd_record *>(key, rec)).second;
}

fd_record::fd_record()
{
  path = "";
  flags = 0;
  mode = 0;
  is_sym_link = false;
}

fd_record::~fd_record()
{
}

fd_record *fd_record::add_str_mode(string s_mode)
{
  if (s_mode.find("r+") != string::npos)
    flags |= O_RDWR;
  else if (s_mode.find("r") != string::npos)
    flags |= O_RDONLY;

  if (s_mode.find("w+") != string::npos ||
      s_mode.find("a+") != string::npos)
    flags |= O_RDWR;
  else if (s_mode.find("w") != string::npos ||
           s_mode.find("a") != string::npos)
    flags |= O_WRONLY;

  return this;
}

bool fd_record::is_read_valid()
{
  return (flags | O_RDONLY) || (flags | O_RDWR);
}

bool fd_record::is_write_valid()
{
  return (flags | O_WRONLY) || (flags | O_RDWR);
}
