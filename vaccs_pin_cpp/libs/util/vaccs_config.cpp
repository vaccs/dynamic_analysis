//
//  vaccs_config
//  PAS
//
//  Created by Steven M. Carr on 12/03/2019.
//
//

#include <pin.H>
#include "vaccs_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <util/general.h>

bool vaccs_config::process_config_setting(string config_var, int config_setting)
{

  DEBUGL(LOG("process_config_setting: " + config_var + ", " + decstr(config_setting) + "\n"));
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

vaccs_config::vaccs_config()
{
  monitor_registers = false;
  user_code_only = false;
  malloc_free = false;
  secure_data = false;
  file_ops = false;
}

vaccs_config::vaccs_config(string config_file_name)
{

  ifstream config_file(config_file_name.c_str());
  string line;

  while (getline(config_file, line)) {
    istringstream iss(line);
    string config_var;
    int config_setting;
    char eq;
    if (!(iss >> config_var >> eq >> config_setting) || (eq != '=')
        || !process_config_setting(config_var, config_setting)) {
      cerr << "Error reading vaccs configuration file " << config_file_name << "\n";
      break;
    }
  }
  config_file.close();
}

vaccs_config::~vaccs_config()
{
}

void vaccs_config::dump_vaccs_config()
{
  DEBUGL(LOG(MONITOR_REGISTERS + " = " + (monitor_registers ? "true" : "false") + "\n"));
  DEBUGL(LOG(USER_CODE_ONLY + " = " + (user_code_only ? "true" : "false") + "\n"));
  DEBUGL(LOG(MALLOC_FREE + " = " + (malloc_free ? "true" : "false") + "\n"));
  DEBUGL(LOG(SECURE_DATA + " = " + (secure_data ? "true" : "false") + "\n"));
  DEBUGL(LOG(FILE_OPS + " = " + (file_ops ? "true" : "false") + "\n"));
}
