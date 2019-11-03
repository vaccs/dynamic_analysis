/*
 * return_addr_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef return_addr_record_h
#define return_addr_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

/**
 * Class: return_addr_record
 *
 * A record function return data
 */
class return_addr_record : public vaccs_record {
private:
    vaccs_address_t dynamic_link;   /* the dynamic link */
    vaccs_address_t return_address; /* the return address */
    char * c_func_name;             /* function name */

public:
    return_addr_record();
    ~return_addr_record();

    /**
     * Add the dynamic link using a builder pattern
     *
     * @return the object
     */
    return_addr_record *
    add_dynamic_link(vaccs_address_t dynamic_link)
    {
        this->dynamic_link = dynamic_link;
        return this;
    }

    /**
     * Get the dynamic link
     *
     * @return the dynamic link
     */
    vaccs_address_t
    get_dynamic_link()
    {
        return dynamic_link;
    }

    /**
     * Add the return address using a builder pattern
     *
     * @return the object
     */
    return_addr_record *
    add_return_address(vaccs_address_t return_address)
    {
        this->return_address = return_address;
        return this;
    }

    /**
     * Get the return address
     *
     * @return the return address
     */
    vaccs_address_t
    get_return_address()
    {
        return return_address;
    }

    /**
     * Add the c function name using a builder pattern
     *
     * @return the object
     */
    return_addr_record *
    add_c_func_name(const char * c_func_name)
    {
        this->c_func_name = ssave(c_func_name);
        return this;
    }

    /**
     * Get the c function name
     *
     * @return the c function name
     */
    char *
    get_c_func_name()
    {
        return c_func_name;
    }

    /**
     * Write an analysis record to a file
     *
     * @param fp the file pointer for the output file
     * @param rec the analysis record to be written to the file
     */
    virtual void
    write(NATIVE_FD fd);

    /**
     * Read an analysis record to a file
     *
     * @param fp the file pointer for the output file
     */
    virtual vaccs_record *
    read(NATIVE_FD fd);
};

#endif /* return_addr_record_h */
