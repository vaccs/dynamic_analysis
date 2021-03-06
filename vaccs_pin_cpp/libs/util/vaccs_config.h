//
//  vaccs_config.h
//
//
//  Created by Steven M. Carr on 12/03/2019.
//
//

#ifndef vaccs_config_h
#define vaccs_config_h

#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;

class vaccs_config {
private:

  string const USER_CODE_ONLY = "user_code_only";
  string const MONITOR_REGISTERS = "monitor_registers";
  string const MALLOC_FREE = "malloc_free";
  string const SECURE_DATA = "secure_data";
  string const FILE_OPS = "file_ops";

  bool user_code_only = true;
  bool monitor_registers = false;
  bool malloc_free = false;
  bool secure_data = false;
  bool file_ops = false;

  bool process_config_setting(string config_var, int config_setting);

public:
  vaccs_config(string config_file_name);
  vaccs_config();
  ~vaccs_config();

  bool get_monitor_registers()
  {
    return monitor_registers;
  }

  bool get_user_code_only()
  {
    return user_code_only;
  }

  bool get_malloc_free()
  {
    return malloc_free;
  }

  bool get_secure_data()
  {
    return secure_data;
  }

  bool get_file_ops()
  {
    return file_ops;
  }

 vaccs_config* add_monitor_registers()
  {
    monitor_registers = true;
    return this;
  }

  vaccs_config* add_user_code_only()
  {
    user_code_only = true;
    return this;
  }

  vaccs_config* add_malloc_free()
  {
    malloc_free = true;
    return this;
  }

  vaccs_config* add_secure_data()
  {
    secure_data = true;
    return this;
  }

  vaccs_config* add_file_ops()
  {
    file_ops = true;
    return this;
  }

  void dump_vaccs_config();
};


#endif /* vaccs_config.cpp_hpp */
