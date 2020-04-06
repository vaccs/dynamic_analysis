/**
 * symbol_table.cpp
 *
 * This file contains the implementation of a general symbol table
 *
 */

#include <tables/symbol_table.h>

#include <util/general.h>

symbol_table::symbol_table(int id)  : std::map<std::string, symbol_table_record *>() {
   this->id = id;
}

symbol_table::~symbol_table() {}

symbol_table_record *symbol_table::get(std::string str) {
   if (this->find(str) == this->end())
      return NULL;
   else
      return (*this)[str];
}

bool symbol_table::put(std::string key, symbol_table_record* rec) {
   return this->insert(std::pair<std::string,symbol_table_record *>(key,rec)).second;
}

symbol_table_record::symbol_table_record(int id) {
   this->id = id;
}

symbol_table_record::~symbol_table_record() {}
