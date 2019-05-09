/*
 * =====================================================================================
 *
 *       Filename:  frame.cpp
 *
 *    Description:  This file implements a stack frame so that there is a mapping from
 *          addresses to variables.
 *
 *        Version:  1.0
 *        Created:  11/13/2018 04:50:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steven Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#include   <tables/frame.h>
#include <tables/deref.h>

/**
 * Create an instance of a frame_record and initialize instance variables.
 */
frame_record::frame_record() {
   variable_name = "";
   value = "";
}

/**
 * Create and instance of a stack frame
 */
frame::frame() : list() { }

/**
 * Get a list of variables that refer directly to the given address
 *
 * @param addr the runtime address to check
 * @param ttab a type table
 * @return a list containing all variables that map to the given address
 */
list<frame_record*> *frame::get(Generic addr, type_table *ttab) {

   DEBUGL(LOG("In frame::get for address " + hexstr(addr) + "\n"));
   list<frame_record *> *flist = new list<frame_record*>();

   for (list<frame_record*>::iterator it = begin(); it != end(); it++) {

      frame_record *frec = *it;

      DEBUGL(LOG("Check variable "+frec->get_variable_name()+ "\n"));

      var_record *vrec = frec->get_var_record();

      vrec->debug_emit(frec->get_variable_name());

      type_record *trec = ttab->get(vrec->get_type());

      trec->debug_emit(vrec->get_type());

      // check to see if this variable is at the memory address being written

      if (vrec->is_at_address(ctx,addr,trec)) {
         DEBUGL(LOG("Variable " + frec->get_variable_name() + " has been accessed\n"));

         string new_value = vrec->read_value(ttab,trec,addr);

         if (new_value != frec->get_value()) {
            DEBUGL(LOG("Value for " + frec->get_variable_name() + " has changed to" + new_value + "\n"));
            frec->add_value(new_value);
            flist->push_back(*it);
         }

      } else {
         DEBUGL(LOG("Variable " + frec->get_variable_name() + " has not been accessed\n"));
      }
   }

   return flist;
}

/**
 * Get a list of variable that refer indirectly to the given address
 *
 * @param addr the runtime address to check
 * @param ttab a type table
 * @return a list containing all variables that map indirectly to the given address
 */
list<frame_record*> *frame::get_pointers(Generic addr, type_table *ttab) {
   list<frame_record *> *flist = new list<frame_record*>();
   for (list<frame_record*>::iterator it = begin(); it != end(); it++) {

      frame_record *frec = *it;
      var_record *vrec = frec->get_var_record();
      type_record *trec = ttab->get(vrec->get_type());

      DEBUGL(LOG("Checking if pointed to by " + frec->get_variable_name() + "\n"));

      if (vrec->points_to_address(ctx,addr,trec)) {
         DEBUGL(LOG(frec->get_variable_name() + " points to " + hexstr(addr) +"\n"));
         flist->push_back(*it);
      }
   }

   return flist;
}


/**
 * Create an instance of a runtime stack and initialize the instance variables
 */
runtime_stack::runtime_stack() {
   cutab = NULL;
   global_frame = NULL;
}

/**
 * Add a compilation unit table to the runtime_stack. This is where we
 * get the stack compilation information from to build the runtime stack
 *
 * @param cutab a compilation unit table
 * @return a pointer to this runtime_stack
 */
runtime_stack *runtime_stack::add_cu_table(cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::add_cu_table for globals\n"));

   this->cutab = cutab;
   global_frame = (new frame())->add_name("*G*");

   for (map<string,symbol_table_record*>::iterator it = cutab->begin(); it != cutab->end(); it++) {

      cu_record *curec = (cu_record *)it->second;
      var_table *vtab = curec->get_var_table();

      for (map<string,symbol_table_record*>::iterator vit = vtab->begin(); vit != vtab->end(); vit++) {

         var_record *vrec = (var_record*)vit->second;

         if (!vrec->get_is_subprog()) {
            DEBUGL(LOG("Add variable " + vit->first + " for frame *G*\n"));
            frame_record *frec = (new frame_record())->add_variable_name(vit->first)->add_var_record(vrec);
            global_frame->push_back(frec);
         }
      }
   }
   return this;
}

/**
 * Push a frame on the runtime stack containing a map from addresses to
 * var_records for each local variable and parameter in the routine.
 *
 * @param name the name of the function
 * @param ip address in the function
 * @param ctx the pin context for this stack frame
 */
void runtime_stack::push(string name,Generic ip,CONTEXT *ctx) {
   DEBUGL(LOG("In runtime_stack::push for routine" + name + "\n"));

   var_table *vtab = cutab->get_function_var_table(ip);

   frame *new_frame = (new frame())->add_name(name)->add_context(ctx);

   if (vtab != NULL)
      for (map<string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {

      var_record *vrec = (var_record*)it->second;

      if (!vrec->get_is_subprog()) {

         DEBUGL(LOG("Add variable " + it->first + " for frame " + name + "\n"));

         type_table *ttab = cutab->get_type_table(vrec->get_type());
         type_record *trec = ttab->get(vrec->get_type());
         Generic addr = vrec->deref_if_by_reference(trec,vrec->get_base_address(ctx));
         string value = vrec->read_value(ttab,trec,addr);

         frame_record *frec = (new frame_record())->add_variable_name(it->first)
                                    ->add_var_record(vrec)
                                    ->add_value(value);

         new_frame->add_frame_record(frec);
       }
   }

   push_front(new_frame);
}

/**
 * Pop a frame from the runtime stack
 */
void runtime_stack::pop() {
   pop_front();
}

/**
* Compute a list of variables that reference a memory location
*
* @param addr the address of a memory location in memory
* @param ttab a type table
* @return a list of variables that reference the given memory location
*/
list<frame_record*> *runtime_stack::get_variables_accessed(Generic addr,type_table *ttab) {

   DEBUGL(LOG("In runtime_stack::get_variables_accessed\n"));

   list<frame_record*> *vlist = new list<frame_record*>();

    /* check the entire stack for a variable at this address */

   for (list<frame*>::iterator it = begin(); it != end(); it++) {
      frame *fr = *it;
      vlist->splice(vlist->end(),*fr->get(addr,ttab));
   }

    /* check globals for a variable at this address */

   vlist->splice(vlist->end(),*global_frame->get(addr,ttab));

   return vlist;
}

/**
* Compute a list of pointer variables that point to a memory location
*
* @param addr the address of a memory location in memory
* @param ttab a type table
* @return a list of pointer variables that point to the given memory location
*/
list<frame_record*> *runtime_stack::get_pointers_to_address(Generic addr,type_table *ttab) {

   DEBUGL(LOG("In runtime_stack::get_pointers_to_address\n"));

   list<frame_record*> *vlist = new list<frame_record*>();

    /* check the entire stack for a variable at this address */
   for (list<frame*>::iterator it = begin(); it != end(); it++){
      frame *fr = *it;
      vlist->splice(vlist->end(),*fr->get_pointers(addr,ttab));
   }

   /* check globals for a variable at this address */
   vlist->splice(vlist->end(),*global_frame->get_pointers(addr,ttab));

   return vlist;
}

/**
* Compute a list of variables that could possibly have been accessed
*
* @param cutab a table of compilation units
* @return a list of variables that on the stack or in global memory that could
* have possibly been accessed
*/
list<var_upd_record*> *runtime_stack::get_updated_variables(cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::get_updated_variables\n"));

   list<var_upd_record*> *vlist = new list<var_upd_record*>();

  /* check the entire stack for a variable at this address */

   for (list<frame*>::iterator it = begin(); it != end(); it++) {
      frame *fr = *it;
      for (list<frame_record*>::iterator it2 = fr->begin(); it2 != fr->end(); it2++) {
         frame_record *frec = *it2;
         type_record *trec = cutab->get_type_record(frec->get_var_record()->get_type());
         Generic addr = frec->get_var_record()->get_base_address(fr->get_context());
         var_upd_record *vurec = (new var_upd_record())
            ->add_variable_name(frec->get_variable_name())
            ->add_var_record(frec->get_var_record())
            ->add_context(fr->get_context())
            ->add_address(addr);
         vlist->push_back(vurec);

         if (trec->get_is_pointer()) {
            bool is_segv;
            addr = dereference_memory((Generic*)addr,&is_segv);
            if (!is_segv) {
            }
         }
      }
   }

   return vlist;
}
