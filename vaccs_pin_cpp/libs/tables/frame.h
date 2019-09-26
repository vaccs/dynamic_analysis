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

#ifndef  frame_h_INC
#define  frame_h_INC

using namespace std;

#include	<string>
#include	<list>
#include	<tables/cu_table.h>
#include	<tables/var_table.h>
#include	<tables/type_table.h>
#include        <tables/heap.h>
#include        <util/general.h>

#include	<pin.H>

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
	frame_record ();                             /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	/**
	 * Get the name of the variable
	 *
	 * @return a string containing the variable name
	 */
	string get_variable_name() {
	    return variable_name;
	}

	/**
	 * Get the value of the variable
	 *
	 * @return a string containing the variable value
	 */
	string get_value() {
	    return value;
	}

	/**
	 * Get the points_to value of the variable
	 *
	 * @return a string containing the variable value
	 */
	string get_points_to_value() {
	    return points_to_value;
	}

	/**
	 * Get the var_record of the variable
	 *
	 * @return a var_record containing dwarf information for a variable
	 */
	var_record *get_var_record() {
	    return vrec;
	}

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add the variable name to this record
	 *
	 * @param a reference to a variable name
	 * @return a pointer to this record
	 */
	frame_record *add_variable_name(const string& name) {
	    variable_name = name;
	    return this;
	}

	/**
	* Add the value of this variable to the entry
	*
	* @param string the string representation of the value
	* @return a pointer to this record
	*/
	frame_record *add_value(string value) {
	    this->value = value;
	    return this;
	}

	/**
	* Add the points_to value of this variable to the entry
	*
	* @param string the string representation of the value
	* @return a pointer to this record
	*/
	frame_record *add_points_to_value(string value) {
	    this->points_to_value = value;
	    return this;
	}

	/**
	* Add the var_record of this variable to the entry
	*
	* @param vrec the var_record containing dwarf information for this variable
	* @return a pointer to this record
	*/
	frame_record *add_var_record(var_record *vrec) {
	    this->vrec = vrec;
	    return this;
	}

	/* ====================  OPERATORS     ======================================= */

    protected:

    private:

	/* ====================  DATA MEMBERS  ======================================= */
	string variable_name;	    /* the name of the variable for this record */
	string value;		    /* the last value stored in this location */
        string points_to_value;     /* the value pointed to by frame records that are pointers */
	var_record *vrec;	    /* the DWARF information for this variable */

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
	frame ();                             /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	/**
	 * Get the name of the function associated with this frame
	 *
	 * @return the name of the function associated with this frame
	 */
	string get_name() {
	    return name;
	}

	/**
	 * Get the pin context associated with this frame
	 *
	 * @return the pin context associated with this frame
	 */
	CONTEXT *get_context() {
	    return ctx;
	}

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add the name of the function associated with this frame using a builder pattern
	 *
	 * @param name a function name
	 * @return the instance of this object
	 */
	frame *add_name(string name) {
	    this->name = name;
	    return this;
	}

	/**
	 * Add the pin context associated with this frame using a builder pattern
	 *
	 * @param name a pin context
	 * @return the instance of this object
	 */
	frame *add_context(CONTEXT *ctx) {
	    this->ctx = ctx;
	    return this;
	}

	/**
	 * Add a frame_record to this frame using a builder pattern
	 *
	 * @param frec a frame record
	 * @return the instance of this object
	 */
	frame *add_frame_record(frame_record *frec) {
	    push_back(frec);
	    return this;
	}

	/* ====================  OPERATORS     ======================================= */

	/**
	 * Get a list of variable that refer directly to the given address
	 *
	 * @param addr the runtime address to check
	 * @param ttab a type table
	 * @return a list containing all variables that map to the given address
	 */
        list<frame_record*> *get(Generic addr, type_table *ttab);

	/* ====================  DATA MEMBERS  ======================================= */
    protected:

    private:

	string name; /* the name of the function */
	CONTEXT *ctx; /* a pin context at the time this frame is put on the stack */

}; /* -----  end of class frame  ----- */

class var_upd_record {

public:

  var_upd_record() {
    variable_name = "";
    vrec = NULL;
    address = -1;
    ctxt = NULL;
    update_is_points_to = false;
  };

  // GETTERS

  string& get_variable_name() {
    return variable_name;
  }

  var_record* get_var_record() {
    return vrec;
  }

  Generic get_address() {
    return address;
  }

  CONTEXT *get_context() {
    return ctxt;
  }

  string get_value() {
     return value;
  }

  bool get_update_is_points_to() {
     return update_is_points_to;
  }

  // BUILDERS

  var_upd_record *add_variable_name(string name) {
    variable_name = name;
    return this;
  }

  var_upd_record *add_var_record(var_record *vrec) {
    this->vrec = vrec;
    return this;
  }

  var_upd_record *add_address(Generic address) {
    this->address = address;
    return this;
  }

  var_upd_record *add_context(CONTEXT *ctxt) {
    this->ctxt = ctxt;
    return this;
  }

  var_upd_record *add_value(string value) {
    this->value = value;
    return this;
  }

  var_upd_record *add_type_name(string name) {
    type_name = name;
    return this;
  }

  var_upd_record *add_points_to(Generic points_to) {
	this->points_to = points_to;
	return this;
  }

  var_upd_record *add_points_to_value(string points_to_value) {
	this->points_to_value = points_to_value;
	return this;
  }

  var_upd_record *add_update_is_points_to() {
     update_is_points_to = true;
     return this;
  }

  var_upd_record *add_scope(string scope) {
	this->scope = scope;
	return this;
  }

  Generic get_points_to() {
	return points_to;
  }	

  string get_points_to_value() {
	return points_to_value;
  }	

  string get_scope() {
     return scope;
  }

  void write(NATIVE_FD vaccs_fd,string fileName,int line,cu_table *cutab,int timestamp);

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
	runtime_stack ();                             /* constructor */

	/* ====================  ACCESSORS     ======================================= */

	/* ====================  BUILDERS      ======================================= */

	/**
	 * Add a compilation unit table to the runtime_stack. This is where we
	 * get the stack compilation information from to build the runtime stack
	 *
	 * @param cutab a compilation unit table
	 * @return a pointer to this runtime_stack
	 */
	runtime_stack *add_cu_table(cu_table *cutab);

	/* ====================  OPERATORS     ======================================= */

	/**
	 * Push a frame on the runtime stack containing a map from addresses to
	 * var_records for each local variable and parameter in the routine.
	 *
	 * @param name the name of the function
	 * @param ip address in the function
	 * @param ctx the pin context for this stack frame
	 */
	void push(string name,Generic ip, CONTEXT *ctx);

	/**
	 * Pop a frame from the runtime stack
	 */
	void pop();

      /**
       * Compute a list of variables that reference a particular updated memory location
       *
       * @param addr the address of a memory location in memory
       * @param ttab a type table
       * @return a list of variables that reference the given memory location
       */
      list<var_upd_record*> *addr_get_updated_variables(Generic addr,cu_table *cutab);

      /**
       * Compute a list of variables that have been updated. Search the stack and global data area for any updated variable.
       * This method is used after a library routine has been called
       *
       * @param cutab a compilation unit table
       * @return a list of variables that have been updated  
       */
      list<var_upd_record *> *get_all_updated_variables(cu_table *cutab);

       /**
        * Check a single frame for variables that have been updated
        *
        * @param cutab a compilation unit table
        * @param fr a stack or global variable frame
        * @param addr the addres that has been updated, if addr = 0 then all locations are checked
        * @return a list of variables in this frame whose value hase changed
        */
      list<var_upd_record *> *get_updated_variables_from_frame(cu_table *cutab,frame *fr, Generic addr);

      /**
       * Check a single frame for variables whose points to location has been updated
       * 
       * @param cutab a compilation unit table
       * @param fr a stack or global variable frame
       * @return a list of variables in this frame whose value hase changed
       */
      list<var_upd_record *> *get_updated_points_to_frame(cu_table *cutab,frame *fr);

      /**
       * Compute a list of all pointers accessible from the stack or static data area whose points_to location has changed 
       *
       * @param cutab a table of compilation units
       * @return a list of variables that on the stack or in global memory that could
       * have possibly been accessed
       */
      list<var_upd_record*> *get_all_updated_points_to(cu_table *cutab);

 	/* ====================  DATA MEMBERS  ======================================= */
    protected:

    private:

	cu_table *cutab;	    /* a table of compilation units from the dwarf info */
	frame *global_frame;	    /* a frame holding global variable information */

}; /* -----  end of class Runtime_stack  ----- */

#endif   /* ----- #ifndef frame_h_INC  ----- */