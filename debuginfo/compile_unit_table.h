#ifndef compile_unit_table_h
#define compile_unit_table_h

#include <vaccs_util/general.h>
#include <vaccs_util/symtab.h>

#define SYM_LOW_PC "low_pc"
#define SYM_HIGH_PC "high_pc"
#define SYM_TYPE_TABLE_PTR "type_table_ptr"
#define SYM_VAR_TABLE_PTR "var_table_ptr"

EXTERN(SymTable, compile_unit_table_init, (int size));
EXTERN(void, compile_unit_table_fini, (SymTable compile_unit_table)); 
EXTERN(int,  compile_unit_table_index, (SymTable compile_unit_table,char *directory, char *file_name));
EXTERN(int,  compile_unit_table_query_index, (SymTable compile_unit_table,char *directory, char *file_name));
EXTERN(SymTable, compile_unit_table_get_type_table, (SymTable compile_unit_table,int index));
EXTERN(SymTable, compile_unit_table_get_var_table, (SymTable compile_unit_table,int index));
EXTERN(void*, compile_unit_table_get_low_pc, (SymTable compile_unit_table,int index));
EXTERN(void*, compile_unit_table_get_high_pc, (SymTable compile_unit_table,int index));
EXTERN(int, compile_unit_table_put_type_table, (SymTable compile_unit_table,int index, SymTable type_table));
EXTERN(int, compile_unit_table_put_var_table, (SymTable compile_unit_table,int index, SymTable var_table));
EXTERN(int, compile_unit_table_put_low_pc, (SymTable compile_unit_table,int index, void* low_pc));
EXTERN(int, compile_unit_table_put_high_pc, (SymTable compile_unit_table,int index, void* high_pc));

#endif
