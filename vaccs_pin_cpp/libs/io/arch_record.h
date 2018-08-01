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


/**
 * Class: arch_record
 *
 * A record indicating on which architecture the analysis was gathered
 */
class arch_record : public vaccs_record {
    private:
      vaccs_arch_t arch_type;         /* The type of architecture */

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
       * Get the architecture type
       *
       * @return the architecture type
       */
      vaccs_arch_t get_arch_type() { return arch_type; }

      /**
       * Write an analysis record to a file
       *
       * @param fp the file pointer for the output file
       * @param rec the analysis record to be written to the file
       */
      virtual void write(FILE *fp);

      /**
       * Read an analysis record to a file
       *
       * @param fp the file pointer for the output file
       */
      virtual vaccs_record *read(FILE *fp);
};

#endif /* arch_record_h */
