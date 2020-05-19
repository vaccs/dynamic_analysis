/*
 * =====================================================================================
 *
 *       Filename:  frame.h
 *
 *    Description:  This is a header file for stack frames
 *
 *        Version:  1.0
 *        Created:  11/13/2018 04:52:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steve Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#ifndef frame_h_INC
#define frame_h_INC

using namespace std;

#include <string>
#include <list>
#include <tables/cu_table.h>
#include <tables/var_table.h>
#include <tables/type_table.h>
#include <tables/heap.h>
#include <util/general.h>
#include <io/return_addr_record.h>

#include <pin.H>

class frame;

/**
 * =====================================================================================
 *        Class:  frame_record
 *  Description:  This class represent and entry in a stack frame. It is for a single
 *		  variable
 * =====================================================================================
 */
class frame_record
{
public:
	/* ====================  LIFECYCLE     ======================================= */

	/**
	 * Create an instance of a frame_record and initialize instance variables.
	 */
	frame_record(); /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	/**
	 * Get the name of the variable
	 *
	 * @return a string containing the variable name
	 */
	string
	get_variable_name()
	{
		return variable_name;
	}

	/**
	 * Get the value of the variable
	 *
	 * @return a string containing the variable value
	 */
	string
	get_value()
	{
		return value;
	}

	/**
	 * Get the points_to value of the variable
	 *
	 * @return a string containing the variable value
	 */
	string
	get_points_to_value()
	{
		return points_to_value;
	}

	/**
	 * Get the var_record of the variable
	 *
	 * @return a var_record containing dwarf information for a variable
	 */
	var_record *
	get_var_record()
	{
		return vrec;
	}

	/**
	 * Get the sensitive data information
	 *
	 * @return true if this variable has been marked as sensitive; otherwise false
	 */
	bool
	get_sensitive_data()
	{
		return sensitive_data;
	}

	/**
	 * Get the heap block
	 *
	 * @return a pointer to the heap block information
	 */
	heap_block *
	get_heap_block()
	{
		return hb;
	}

	/**
	 * Get the stack frame
	 *
	 * @return a pointer to the stack frame for this record
	 */
	frame *
	get_frame()
	{
		return stack_frame;
	}

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add the variable name to this record
	 *
	 * @param a reference to a variable name
	 * @return a pointer to this record
	 */
	frame_record *
	add_variable_name(const string &name)
	{
		variable_name = name;
		return this;
	}

	/**
	 * Add the value of this variable to the entry
	 *
	 * @param string the string representation of the value
	 * @return a pointer to this record
	 */
	frame_record *
	add_value(string value)
	{
		this->value = value;
		return this;
	}

	/**
	 * Add the points_to value of this variable to the entry
	 *
	 * @param string the string representation of the value
	 * @return a pointer to this record
	 */
	frame_record *
	add_points_to_value(string value)
	{
		this->points_to_value = value;
		return this;
	}

	/**
	 * Add the var_record of this variable to the entry
	 *
	 * @param vrec the var_record containing dwarf information for this variable
	 * @return a pointer to this record
	 */
	frame_record *
	add_var_record(var_record *vrec)
	{
		this->vrec = vrec;
		return this;
	}

	/**
	 * Mark this variable as sensitive data
	 *
	 * @return a pointer to this record
	 */
	frame_record *
	add_sensitive_data()
	{
		this->sensitive_data = true;
		return this;
	}

	/**
	 * Add the heap block information for this variable
	 *
	 * @return a pointer to this record
	 */
	frame_record *
	add_heap_block(heap_block *hb)
	{
		this->hb = hb;
		return this;
	}

	/**
	 * Add the frame for this record
	 *
	 * @return a pointer to this record
	 */
	frame_record *
	add_frame(frame *fr)
	{
		this->stack_frame = fr;
		return this;
	}

	/**
	 * Remove the heap block information for this variable
	 *
	 */
	void
	remove_heap_block(heap_block *hb)
	{
		this->hb = NULL;
		delete data_clear_status;
		data_clear_status = NULL;
		delete data_set_status;
		data_set_status = NULL;
	}
	/* ====================  OPERATORS     ======================================= */

	void mark_data_clear_status(cu_table *cutab, CONTEXT *ctxt, Generic addr, int c, Generic n);
	bool is_data_clear();
	bool is_data_set();
	void reset_clear_set_status();

protected:
private:
	/* ====================  DATA MEMBERS  ======================================= */
	string variable_name;   /* the name of the variable for this record */
	string value;			/* the last value stored in this location */
	string points_to_value; /* the value pointed to by frame records that are pointers */
	var_record *vrec;		/* the DWARF information for this variable */
	bool sensitive_data;
	heap_block *hb;
	bool *data_clear_status;
	bool *data_set_status;
	Generic num_bytes;
	frame *stack_frame;
}; /* -----  end of class Frame_record  ----- */

/*
 * =====================================================================================
 *        Class:  frame
 *  Description:  This class implements a stack frame and holds symbol table
 *		  entries for variables in the scope.
 * =====================================================================================
 */
class frame : public list<frame_record *>
{
public:
	/* ====================  LIFECYCLE     ======================================= */

	/**
	 * Create and instance of a stack frame
	 */
	frame(); /* constructor */

	void dump();

	/* ====================  ACCESSORS     ======================================= */

	/**
	 * Get the name of the function associated with this frame
	 *
	 * @return the name of the function associated with this frame
	 */
	string
	get_name()
	{
		return name;
	}

	/**
	 * Get the pin context associated with this frame
	 *
	 * @return the pin context associated with this frame
	 */
	CONTEXT *
	get_context()
	{
		return ctxp;
	}

	bool
	get_is_first_access()
	{
		return is_first_access;
	}

	bool
	get_is_before_stack_setup()
	{
		return is_before_stack_setup;
	}

	Generic
	get_old_rbp()
	{
		return old_rbp;
	}

	Generic
	get_return_addr()
	{
		return return_addr;
	}

	Generic
	get_start_pc()
	{
		return start_pc;
	}

	Generic
	get_high_pc()
	{
		return high_pc;
	}

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add the name of the function associated with this frame using a builder pattern
	 *
	 * @param name a function name
	 * @return the instance of this object
	 */
	frame *
	add_name(string name)
	{
		this->name = name;
		return this;
	}

	/**
	 * Add the pin context associated with this frame using a builder pattern
	 *
	 * @param name a pin context
	 * @return the instance of this object
	 */
	frame *
	add_context(const CONTEXT *ctx)
	{
		ctxp = &(this->ctx);
		PIN_SaveContext(ctx, ctxp);
		return this;
	}

	frame *
	add_old_rbp(Generic addr)
	{
		this->old_rbp = addr;
		return this;
	}

	frame *
	add_return_addr(Generic addr)
	{
		this->return_addr = addr;
		return this;
	}

	frame *
	add_start_pc(Generic addr)
	{
		this->start_pc = addr + text_base_address;
		return this;
	}

	frame *
	add_high_pc(Generic addr)
	{
		this->high_pc = addr + text_base_address;
		return this;
	}

	/**
	 * Add a frame_record to this frame using a builder pattern
	 *
	 * @param frec a frame record
	 * @return the instance of this object
	 */
	frame *
	add_frame_record(frame_record *frec)
	{
		push_back(frec);
		return this;
	}

	/* ====================  OPERATORS     ======================================= */

	void
	clear_is_first_access()
	{
		is_first_access = false;
	}

	void
	clear_is_before_stack_setup()
	{
		is_before_stack_setup = false;
	}

	/**
	 * Get a list of variable that refer directly to the given address
	 *
	 * @param addr the runtime address to check
	 * @param cutab a compilation unit table
	 * @return a list containing all variables that map to the given address
	 */
	list<frame_record *> *get(Generic addr, cu_table *cutab);

	/**
	 * Remove heap block from frame_record for a freed block
	 *
	 * @param hb the heap block that has been freed
	 */
	void remove_block(heap_block *hb);

	/**
	 *
	 * Add the old rbp and return address to the frames
	 *
	 * @param ctx a PIN CONTEXT
	 * @return the frame
	 */
	frame *
	add_links(CONTEXT *ctx);

	/**
	 * Determine if an instruction pointer address is in the section of user
	 * code for a particular active function call
	 *
	 * @param ip an instruction pointer
	 * @return true if the ip is within the bounds of the function instruction
	 *              space and after the stack setup code
	 */
	bool
	in_user_code(Generic ip);

	/* ====================  DATA MEMBERS  ======================================= */
protected:
private:
	string name; /* the name of the function */
	CONTEXT *ctxp;
	CONTEXT ctx; /* a pin context at the time this frame is put on the stack */
	bool is_first_access;
	bool is_before_stack_setup;
	Generic old_rbp;
	Generic return_addr;
	Generic start_pc;
	Generic high_pc;
}; /* -----  end of class frame  ----- */

class var_upd_record
{
public:
	var_upd_record()
	{
		variable_name = "";
		vrec = NULL;
		address = -1;
		ctxt = NULL;
		update_is_points_to = false;
		prefix = "";
	}

	// GETTERS

	string &
	get_variable_name()
	{
		return variable_name;
	}

	var_record *
	get_var_record()
	{
		return vrec;
	}

	Generic
	get_address()
	{
		return address;
	}

	CONTEXT *
	get_context()
	{
		return ctxt;
	}

	string
	get_value()
	{
		return value;
	}

	bool
	get_update_is_points_to()
	{
		return update_is_points_to;
	}

	string
	get_prefix()
	{
		return prefix;
	}

	// BUILDERS

	var_upd_record *
	add_variable_name(string name)
	{
		variable_name = name;
		return this;
	}

	var_upd_record *
	add_var_record(var_record *vrec)
	{
		this->vrec = vrec;
		return this;
	}

	var_upd_record *
	add_address(Generic address)
	{
		this->address = address;
		return this;
	}

	var_upd_record *
	add_context(CONTEXT *ctxt)
	{
		this->ctxt = ctxt;
		return this;
	}

	var_upd_record *
	add_value(string value)
	{
		this->value = value;
		return this;
	}

	var_upd_record *
	add_type_name(string name)
	{
		type_name = name;
		return this;
	}

	var_upd_record *
	add_points_to(Generic points_to)
	{
		this->points_to = points_to;
		return this;
	}

	var_upd_record *
	add_points_to_value(string points_to_value)
	{
		this->points_to_value = points_to_value;
		return this;
	}

	var_upd_record *
	add_update_is_points_to()
	{
		update_is_points_to = true;
		return this;
	}

	var_upd_record *
	add_scope(string scope)
	{
		this->scope = scope;
		return this;
	}

	var_upd_record *
	add_prefix(string prefix)
	{
		this->prefix = prefix;
		return this;
	}

	Generic
	get_points_to()
	{
		return points_to;
	}

	string
	get_points_to_value()
	{
		return points_to_value;
	}

	string
	get_scope()
	{
		return scope;
	}

	void
	write(NATIVE_FD vaccs_fd, string fileName, int line, cu_table *cutab, int timestamp);

private:
	string variable_name;
	var_record *vrec;
	Generic address;
	CONTEXT *ctxt;
	string value;
	string type_name;
	string scope;
	Generic points_to;
	string points_to_value;
	string prefix;
	bool update_is_points_to;
};

/*
 * =====================================================================================
 *        Class:  runtime_stack
 *  Description:  This class represents the runtime stack so that addresses can be
 *                mapped to debug information for variables.
 * =====================================================================================
 */
class runtime_stack : public list<frame *>
{
public:
	/* ====================  LIFECYCLE     ======================================= */

	/**
	 * Create an instance of a runtime stack and initialize the instance variables
	 */
	runtime_stack(); /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add a compilation unit table to the runtime_stack. This is where we
	 * get the stack compilation information from to build the runtime stack
	 *
	 * @param cutab a compilation unit table
	 * @return a pointer to this runtime_stack
	 */
	runtime_stack *
	add_cu_table(cu_table *cutab);

	/* ====================  OPERATORS     ======================================= */

	/**
	 * Push a frame on the runtime stack containing a map from addresses to
	 * var_records for each local variable and parameter in the routine.
	 *
	 * @param name the name of the function
	 * @param ip address in the function
	 * @param ctx the pin context for this stack frame
	 */
	frame *
	push(string name, Generic ip, CONTEXT *ctx);

	frame *
	top()
	{
		if (!this->empty())
			return this->front();
		else
			return NULL;
	}

	/**
	 * Pop a frame from the runtime stack
	 */
	void
	pop();

	/**
	 * Determine the last user function called 
	 */
	string get_last_user_function_called();

	/**
	 * Compute a list of variables that reference a particular updated memory location
	 *
	 * @param addr the address of a memory location in memory
	 * @param ttab a type table
	 * @return a list of variables that reference the given memory location
	 */
	list<var_upd_record *> *addr_get_updated_variables(Generic addr, cu_table *cutab);

	/**
	 * Compute a list of variables that have been updated. Search the stack and global data area for any updated variable.
	 * This method is used after a library routine has been called
	 *
	 * @param cutab a compilation unit table
	 * @return a list of variables that have been updated
	 */
	list<var_upd_record *> *get_all_updated_variables(cu_table *cutab);

	list<var_upd_record *> *get_updated_struct_members(cu_table *curtab, frame *fr,
													   frame_record *frec,
													   var_record *vrec,
													   Generic addr,
													   string prefix);

	list<var_upd_record *> *get_updated_array_elements(cu_table *curtab, frame *fr,
													   frame_record *frec,
													   var_record *vrec,
													   Generic addr,
													   string prefix);

	/**
	 * Check a single frame for variables that have been updated
	 *
	 * @param cutab a compilation unit table
	 * @param fr a stack or global variable frame
	 * @param addr the addres that has been updated, if addr = 0 then all locations are checked
	 * @return a list of variables in this frame whose value hase changed
	 */
	list<var_upd_record *> *get_updated_variables_from_frame(cu_table *cutab, frame *fr, Generic addr);

	list<var_upd_record *> *get_updated_points_to_from_struct(cu_table *cutab, frame *fr,
															  frame_record *frec,
															  var_record *vrec,
															  Generic addr,
															  string prefix);

	/**
	 * Check a single frame for variables whose points to location has been updated
	 *
	 * @param cutab a compilation unit table
	 * @param fr a stack or global variable frame
	 * @return a list of variables in this frame whose value hase changed
	 */
	list<var_upd_record *> *get_updated_points_to_frame(cu_table *cutab, frame *fr);

	/**
	 * Compute a list of all pointers accessible from the stack or static data area whose points_to location has changed
	 *
	 * @param cutab a table of compilation units
	 * @return a list of variables that on the stack or in global memory that could
	 * have possibly been accessed
	 */
	list<var_upd_record *> *get_all_updated_points_to(cu_table *cutab);

	/**
	 * Compute a list a frames with updated old_rbp or return addresses
	 *
	 * @return a list of return address records with updated information
	 */
	list<return_addr_record *> *
	get_updated_links();

	/**
	 * Compute a list a frame records whose address matches addr
	 *
	 * @param addr a variable address
	 * @return a list of return address records with updated information
	 */
		list<frame_record *> *get_variables_with_address(Generic addr, cu_table *cutab);

	/**
 * Remove heap block from frame_record for a freed block
 *
 * @param hb the heap block that has been freed
 */
	void remove_block(heap_block *hb);

	/* ====================  DATA MEMBERS  ======================================= */
protected:
private:
	cu_table *cutab;	 /* a table of compilation units from the dwarf info */
	frame *global_frame; /* a frame holding global variable information */
};						 /* -----  end of class Runtime_stack  ----- */

#endif /* ----- #ifndef frame_h_INC  ----- */
