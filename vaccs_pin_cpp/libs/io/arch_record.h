/*
 * arch_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef arch_record_h
#define arch_record_h

#include <util/general.h>
#include <io/vaccs_record.h>
#include <types.h>
#include <pin.H>


/**
 * Class: arch_record
 *
 * A record indicating on which architecture the analysis was gathered
 */
class arch_record : public vaccs_record {
    private:
      vaccs_arch_t arch_type;         /* The type of architecture */
      vaccs_address_t heap_start;     /* the start address of the heap */
      vaccs_address_t heap_end;       /* the end address of the heap */
      vaccs_address_t stack_start;     /* the start address of the stack */
      vaccs_address_t stack_end;       /* the end address of the stack (will be the smaller of two addresses) */ 

    public:
      arch_record();
      ~arch_record() {}

      /**
       * Add the architecture type using a builder pattern
       *
       * @return the object
       */
      arch_record *add_arch_type(vaccs_arch_t atype)
         { this->arch_type = atype;  return this; }

      /**
       * Add the heap start using a builder pattern
       *
       * @return the object
       */
      arch_record *add_heap_start(vaccs_address_t hstart)
         { this->heap_start = hstart;  return this; }

      /**
       * Add the stack end using a builder pattern
       *
       * @return the object
       */
      arch_record *add_stack_end(vaccs_address_t send)
         { this->stack_end = send;  return this; }

      /**
       * Add the stack start using a builder pattern
       *
       * @return the object
       */
      arch_record *add_stack_start(vaccs_address_t sstart)
         { this->stack_start = sstart;  return this; }
      /**
       * Add the heap end using a builder pattern
       *
       * @return the object
       */
      arch_record *add_heap_end(vaccs_address_t hend)
         { this->heap_end = hend;  return this; }

      /**
       * Get the architecture type
       *
       * @return the architecture type
       */
      vaccs_arch_t get_arch_type() { return arch_type; }

      /**
       * Get the heap start
       *
       * @return the heap start
       */
      vaccs_address_t get_heap_start() { return heap_start; }

      /**
       * Get the heap end
       *
       * @return the heap end
       */
      vaccs_address_t get_heap_end() { return heap_end; }

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

#endif /* arch_record_h */
