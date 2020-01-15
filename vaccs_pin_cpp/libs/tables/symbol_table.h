/**
 * symbol_table.H
 *
 * This file contains a "abstract class" for symbol tables. This should be the
 * base class of any symbol table used so that they are all related to each
 * other. Specifically, all should be able to be put on a symbol table stack.
 *
 */
#ifndef symbol_table_h
#define symbol_table_h

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

#include <pin.H>

#define CU_TABLE 0
#define TYPE_TABLE 1
#define VAR_TABLE 2

#define CU_RECORD 0
#define TYPE_RECORD 1
#define VAR_RECORD 2

class symbol_table_record {

protected:
	int id;

   public:
      symbol_table_record(int id);
      virtual ~symbol_table_record();

      virtual void write(std::string key,NATIVE_FD fd)= 0;
};

class symbol_table : public std::map<std::string, symbol_table_record *> {

protected:
	int id;

   public:
      symbol_table(int id);
      virtual ~symbol_table();

      symbol_table_record *get(std::string str);

      bool put(std::string str,symbol_table_record *sym_rec);

      virtual void write(NATIVE_FD fd) = 0;
};


#endif  /* symtab_h */
