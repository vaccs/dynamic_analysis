/*
 * =====================================================================================
 *
 *       Filename:  fd_table.h
 *
 *    Description:  This is a header file for file information
 *
 *        Version:  1.0
 *        Created:  11/13/2018 04:52:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steve Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#ifndef  fd_table_h
#define  fd_table_h

#include <map>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;

class fd_record;

class fd_table : public std::map<long, fd_record *> {
public:
  fd_table()
    : std::map<long, fd_record *>()
  {
  }
  virtual ~fd_table();
  bool put(long fd, fd_record *fd_rec);
};

class fd_record {

private:
  string path;
  int flags;
  mode_t mode;
  bool is_sym_link;

public:

  fd_record();
  virtual ~fd_record();

  string get_path()
  {
    return path;
  }

  int get_flags()
  {
    return flags;
  }

  mode_t get_mode()
  {
    return mode;
  }

  bool get_is_sym_link()
  {
    return is_sym_link;
  }

  fd_record *add_path(const char *path)
  {
    this->path.assign(path);
    return this;
  }

  fd_record *add_flags(int flags)
  {
    this->flags = flags;
    return this;
  }

  fd_record *add_mode(mode_t mode)
  {
    this->mode = mode;
    return this;
  }

  fd_record *add_str_mode(string mode);

  fd_record *add_is_sym_link()
  {
    this->is_sym_link = true;
    return this;
  }

  bool is_read_valid();
  bool is_write_valid();

};

#endif
