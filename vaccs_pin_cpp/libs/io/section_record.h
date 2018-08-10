/*
 * section_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef section_record_h
#define section_record_h

#include <util/general.h>
#include <io/vaccs_record.h>

#include <util/c_string_utils.h>
#include <pin.H>

/**
 * Class: section_record
 *
 * A record of program section
 */
class section_record : public vaccs_record {
    private:
       char *info; /* section information */

    public:
      section_record();
      ~section_record();

      /**
       * Add the section info using a builder pattern
       *
       * @return the object
       */
      section_record *add_info(const char *info)
         { this->info = ssave(info);  return this; }

      /**
       * Get the section info
       *
       * @return the section info
       */
      char *get_info() { return info; }

      /**
       * Write an analysis record to a file
       *
       * @param fp the file pointer for the output file
       * @param rec the analysis record to be written to the file
       */
      virtual void write(NATIVE_FD fd);

      /**
       * Read an analysis record to a file
       *
       * @param fp the file pointer for the output file
       */
      virtual vaccs_record *read(NATIVE_FD fd);
};

#endif /* section_record_h */
