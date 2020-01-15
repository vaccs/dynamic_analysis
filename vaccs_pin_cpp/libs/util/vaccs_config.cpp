//
//  vaccs_config
//  PAS
//
//  Created by Steven M. Carr on 12/03/2019.
//
//

#include "vaccs_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <util/general.h>

bool vaccs_config::process_config_setting(string config_var,int config_setting) {

  if (config_var == MONITOR_REGISTERS)
    if (config_setting)
      monitor_registers = true;
    else
      monitor_registers = false;
  else if (config_var == USER_CODE_ONLY)
    if (config_setting)
      user_code_only = true;
    else
      user_code_only = false;
  else if (config_var == MALLOC_FREE)
    if (config_setting)
      malloc_free = true;
    else
      malloc_free = false;
  else if (config_var == SECURE_DATA)
    if (config_setting)
      secure_data = true;
    else
      secure_data = false;
  else if (config_var == FILE_OPS)
    if (config_setting)
      file_ops = true;
    else
      file_ops = false;
  else
    return false;

  return true;
}

vaccs_config::vaccs_config(string config_file_name)  {

  ifstream config_file(config_file_name.c_str());
  string line;

  while (getline(config_file,line)) {
    istringstream iss(line);
    string config_var;
    int config_setting;
    char eq;
    if (!(iss >> config_var >> eq >> config_setting) || (eq != '=')
        || !process_config_setting(config_var,config_setting)) {
      cerr << "Error reading vaccs configuration file " << config_file_name << "\n";
      break;
    }
  }
  config_file.close();
}

vaccs_config::~vaccs_config() {}
