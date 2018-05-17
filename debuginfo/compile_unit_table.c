/******************************************************************************
*
* File: compile_unit_table.c
*
* Summary: This file contains the code for building tables for mapping 
*          dwarf abbreviations to their format
*
*
******************************************************************************/

#include <stdlib.h>

#include <vaccs_util/string_utils.h>
#include <compile_unit_table.h>
#include <type_table.h>

/**
*
* Free the type table for a compile unit
*
* @param type_table a pointer to a type table
*
*/
static void free_type_table(Generic type_table) {
   if (type_table != NULL)
     type_table_fini((SymTable)type_table);
}

/**
* 
* Initialize the compile unit symbol table
*
* @param size the size of the compile unit table
* @return a symbol table for a compilation unit
*
*/
SymTable compile_unit_table_init(int size) {
	
   SymTable compile_unit_table = SymInit(size);
    
   SymInitField(compile_unit_table,SYM_TYPE_TABLE_PTR,NULL,free_type_table);
   SymInitField(compile_unit_table,SYM_VAR_TABLE_PTR,NULL,NULL);
   SymInitField(compile_unit_table,SYM_LOW_PC,NULL,NULL);
   SymInitField(compile_unit_table,SYM_HIGH_PC,NULL,NULL);

   return compile_unit_table;
}

/**
* 
* Free the compile unit symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
*/
void compile_unit_table_fini(SymTable compile_unit_table) {

   SymKill(compile_unit_table);
   compile_unit_table = NULL;
}

/**
* 
* Look for a compile unit in the symbol table. If it is not there, insert it. Return the index.
*
* @param compile_unit_table the symbol table for a compilation unit
* @param directory the directory where the compile unit source is located
* @param file_name the file name where the compile unit source is located
* @return the index in the symbol table for the compile_unit
*
*/
int compile_unit_table_index(SymTable compile_unit_table,char *directory,char *file_name) {

   return SymIndex(compile_unit_table,nssave(3,directory,"/",file_name));

}

/**
* 
* Look for a compile unit in the symbol table. If it is not there, return -1, 
* otherwise return the index.
*
* @param compile_unit_table the symbol table for a compilation unit
* @param directory the directory where the compile unit source is located
* @param file_name the file name where the compile unit source is located
* @return the index in the symbol table for the compilation unit, -1 if not found
*
*/
int compile_unit_table_query_index(SymTable compile_unit_table,char *directory,char *file_name) {

   char *path = nssave(3,directory,"/",file_name);
   int index = SymQueryIndex(compile_unit_table,path);
   sfree(path);
  
   return index;

}

/**
* 
* Put the type table field value for a compile unit in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @param type_table a pointer to a type table
* @return 1 if successful, 0 otherwise
*/
int compile_unit_table_put_type_table(SymTable compile_unit_table,int index, SymTable type_table) {

   if (SymFieldExists(compile_unit_table,SYM_TYPE_TABLE_PTR)) {
      SymPutFieldByIndex(compile_unit_table,index,SYM_TYPE_TABLE_PTR,type_table);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Put the low pc field value for a compile unit in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @param low_pc the instruction address of the lowest pc in the compile unit
* @return 1 if successful, 0 otherwise
*/
int compile_unit_table_put_low_pc(SymTable compile_unit_table,int index, void* low_pc) {

   if (SymFieldExists(compile_unit_table,SYM_LOW_PC)) {
      SymPutFieldByIndex(compile_unit_table,index,SYM_LOW_PC,low_pc);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Put the high pc field value for a compile unit in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @param high_pc the instruction address of the highest pc in the compile unit
* @return 1 if successful, 0 otherwise
*/
int compile_unit_table_put_high_pc(SymTable compile_unit_table,int index, void* high_pc) {

   if (SymFieldExists(compile_unit_table,SYM_HIGH_PC)) {
      SymPutFieldByIndex(compile_unit_table,index,SYM_HIGH_PC,high_pc);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Get the type table field value for a compile unit name in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @return a pointer to a type table for this compile unit if present, else NULL
*
*/
SymTable compile_unit_table_get_type_table(SymTable compile_unit_table,int index) {
   if (SymFieldExists(compile_unit_table,SYM_TYPE_TABLE_PTR)) {
      return (SymTable)SymGetFieldByIndex(compile_unit_table,index,SYM_TYPE_TABLE_PTR);
   }
   else {
      return NULL;
   }
}

/**
* 
* Get the low pc field value for a compile unit name in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @return the lowest pc for this compile unit if present, else NULL
*
*/
void* compile_unit_table_get_low_pc(SymTable compile_unit_table,int index) {
   if (SymFieldExists(compile_unit_table,SYM_LOW_PC)) {
      return (void*)SymGetFieldByIndex(compile_unit_table,index,SYM_LOW_PC);
   }
   else {
      return NULL;
   }
}

/**
* 
* Get the high pc field value for a compile unit name in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @return the highest pc for this compile unit if present, else NULL
*
*/
void* compile_unit_table_get_high_pc(SymTable compile_unit_table,int index) {
   if (SymFieldExists(compile_unit_table,SYM_HIGH_PC)) {
      return (void*)SymGetFieldByIndex(compile_unit_table,index,SYM_HIGH_PC);
   }
   else {
      return NULL;
   }
}

/**
* 
* Put the variable table field value for a compile unit in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @param var_table a pointer to a var table
* @return 1 if successful, 0 otherwise
*/
int compile_unit_table_put_var_table(SymTable compile_unit_table,int index, SymTable var_table) {

   if (SymFieldExists(compile_unit_table,SYM_VAR_TABLE_PTR)) {
      SymPutFieldByIndex(compile_unit_table,index,SYM_VAR_TABLE_PTR,var_table);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Get the variable table field value for a compile unit name in the symbol table
*
* @param compile_unit_table the symbol table for a compilation unit
* @param index the index in the symbol table for a compile unit
* @return a pointer to a var table for this compile unit if present, else NULL
*
*/
SymTable compile_unit_table_get_var_table(SymTable compile_unit_table,int index) {
   if (SymFieldExists(compile_unit_table,SYM_VAR_TABLE_PTR)) {
      return (SymTable)SymGetFieldByIndex(compile_unit_table,index,SYM_VAR_TABLE_PTR);
   }
   else {
      return NULL;
   }
}
