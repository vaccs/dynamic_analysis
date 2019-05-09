/******************************************************************************
 *
 * File: type_table.cpp
 *
 * Summary: This file contains the code for building tables for mapping
 *          dwarf types to their attributes
 *
 *
 ******************************************************************************/

#include <util/general.h>
#include <tables/var_table.h>
#include <tables/type_table.h>

#include <cassert>
#include <cstring>

#include <sstream>

#include <iostream>


/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
type_record::type_record() : symbol_table_record(TYPE_RECORD) {

   type_size = -1;
   name = NULL;
   is_array = false;
   is_pointer = false;
   is_struct = false;
   is_typedef = false;
   is_const = false;
   upper_bound = -1;
   base_type = NULL;
   member_tab = NULL;
}

type_record::~type_record() {}

/**
 * Add the upper bound to the object using a builder pattern
 *
 * @param upper_bound the upper bound of this array type
 * @return the object
 */
type_record* type_record::add_upper_bound(Generic upper_bound, bool compute_name_and_size) {
   this->upper_bound = upper_bound;
   if (compute_name_and_size) {
      this->name->append("[");

      std::ostringstream convert;
      convert << upper_bound;

      this->name->append(convert.str());

      this->name->append("]");
      this->type_size *= (upper_bound+1);
   }

   return this;
}

/**
 * Write a type table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void type_record::write(std::string key,NATIVE_FD fd) {

   DEBUGL(LOG("Begin type_record::write()\n"));

   USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size_t length = key.length();
   size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size =  length; assert(OS_WriteFD(fd,key.c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size = sizeof(type_size); assert(OS_WriteFD(fd,&type_size,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

   length = strlen(name->c_str());
   size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size =  length; assert(OS_WriteFD(fd,name->c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size =  sizeof(is_array); assert(OS_WriteFD(fd,&is_array,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size =  sizeof(upper_bound); assert(OS_WriteFD(fd,&upper_bound,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size = sizeof(is_pointer); assert(OS_WriteFD(fd,&is_pointer,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   size = sizeof(is_struct); assert(OS_WriteFD(fd,&is_struct,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

   if (base_type != NULL) {
      length = base_type->length();
      size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
      size =  length; assert(OS_WriteFD(fd,base_type->c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   } else {
      length = 0;
      size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
   }

   if (member_tab != NULL)
      member_tab->write(fd);

   DEBUGL(LOG("End type_record::write()\n"));
}

void type_record::debug_emit(string type_id) {
   DEBUGL(LOG("Type record for " + type_id + "\n"));
   DEBUGL(LOG("\t type size = " + decstr(type_size) + "\n"));
   DEBUGL(LOG("\t type name = " + *name + "\n"));
   DEBUGL(LOG("\t is_array =" + decstr((int)is_array) + "\n"));
   DEBUGL(LOG("\t upper bound = " + decstr(upper_bound) + "\n"));
   DEBUGL(LOG("\t is_pointer = " + decstr((int)is_pointer) + "\n"));
   DEBUGL(LOG("\t is_array = " + decstr((int)is_array) + "\n"));
   DEBUGL(LOG("\t is_struct = " + decstr((int)is_struct) + "\n"));

   if (base_type != NULL) {
      DEBUGL(LOG("\t base_type = " + *base_type + "\n"));
   }
}

type_table::~type_table() {}
/**
 * Write the type table to a file
 *
 * @param fp a file pointer
 */
void type_table::write(NATIVE_FD fd) {
   DEBUGL(LOG("Begin type_table::write()\n"));
   USIZE dsize =  sizeof(id); assert(OS_WriteFD(fd,&id,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

   size_t num = size();
   dsize = sizeof(num); assert(OS_WriteFD(fd,&num,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

   for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
      type_record* trec = (type_record*)it->second;
      trec->write(it->first,fd);
   }
   DEBUGL(LOG("End type_table::write()\n"));

}
