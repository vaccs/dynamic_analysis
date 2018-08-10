/*
 * asm_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef asm_record_h
#define asm_record_h

#include <util/general.h>
#include <io/vaccs_record.h>

#include <util/c_string_utils.h>

#include <pin.H>
/**
 * Class: asm_record
 *
 * A record indicating on which architecture the analysis was gathered
 */
class asm_record : public vaccs_record {
    private:
      vaccs_line_num_t asm_line_num;       /* The assembly line number */
      vaccs_line_num_t c_line_num;         /* The C line number for this assembly line */
      char *asm_file_name; /* ASM file name */
      char *c_file_name; /* C file name */
      char *asm_inst;         /* The assembly instruction */

    public:
      asm_record();
     ~asm_record();

      /**
       * Add the asm line number using a builder pattern
       *
       * @return the object
       */
      asm_record *add_asm_line_num(vaccs_line_num_t asm_line_num) 
         { this->asm_line_num = asm_line_num;  return this; }

      /**
       * Get the asm line number 
       *
       * @return the asm line number
       */
      vaccs_line_num_t get_asm_line_num() { return asm_line_num; }

      /**
       * Add the C line number using a builder pattern
       *
       * @return the object
       */
      asm_record *add_c_line_num(vaccs_line_num_t c_line_num) 
         { this->c_line_num = c_line_num;  return this; }

      /**
       * Get the C line number 
       *
       * @return the C line number
       */
      vaccs_line_num_t get_c_line_num() { return c_line_num; }

      /**
       * Add the asm file name using a builder pattern
       *
       * @return the object
       */
      asm_record *add_asm_file_name(const char *asm_file_name)
         { this->asm_file_name = ssave(asm_file_name);  return this; }

      /**
       * Get the asm file name 
       *
       * @return the asm file name
       */
      char *get_asm_file_name() { return asm_file_name; }

      /**
       * Add the C file name using a builder pattern
       *
       * @return the object
       */
      asm_record *add_c_file_name(const char *c_file_name)
         { this->c_file_name = ssave(c_file_name);  return this; }

      /**
       * Get the C file name 
       *
       * @return the C file name
       */
      char *get_c_file_name() { return c_file_name; }

      /**
       * Add the asm instruction using a builder pattern
       *
       * @return the object
       */
      asm_record *add_asm_inst(const char *asm_inst)
         { this->asm_inst = ssave(asm_inst);  return this; }

      /**
       * Get the asm instruction
       *
       * @return the asm instruction
       */
      char *get_asm_inst() { return asm_inst; }

      /**
       * Write an analysis record to a file
       *
       * @param fp the file pointer for the output file
       * @param rec the analysis record to be written to the file
       */
      virtual void write(NATIVE_FD fp);

      /**
       * Read an analysis record to a file
       *
       * @param fp the file pointer for the output file
       */
      virtual vaccs_record *read(NATIVE_FD fp);
};

#endif /* asm_record_h */
