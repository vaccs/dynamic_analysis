#ifndef type_table_h
#define type_table_h

#include <vaccs_util/general.h>
#include <vaccs_util/symtab.h>

#define SYM_TYPE_SIZE "type_size"

EXTERN(SymTable, type_table_init, (int size));
EXTERN(void, type_table_fini, (SymTable type_table)); 
EXTERN(int, type_table_index, (SymTable type_table,char *name));
EXTERN(int, type_table_query_index, (SymTable type_table,char *name));
EXTERN(int, type_table_put_size, (SymTable type_table,int index, long size));
EXTERN(long, type_table_get_size, (SymTable type_table,int index));

#endif
