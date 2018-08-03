/*
 * pin_expr_context.h
 *
 *  Created on: Jul 18, 2018
 *      Author: carr
 *
 *      Code taken from https://blog.tartanllama.xyz/writing-a-linux-debugger-setup/
 */

#ifndef LIBS_UTIL_BASE_EXPR_CONTEXT_H_
#define LIBS_UTIL_BASE_EXPR_CONTEXT_H_

#include <libelfin/dwarf/dwarf++.hh>
#include <sys/ptrace.h>
#include <sys/user.h>

#ifdef __x86_64
#define CFA_OFFSET 16
#else
#define CFA_OFFSET 8
#endif

class base_expr_context: public dwarf::expr_context {
private:
	pid_t m_pid;

public:
	base_expr_context(pid_t pid) : m_pid(pid) {}
	virtual ~base_expr_context() {}

    dwarf::taddr reg (unsigned regnum) override { return 0; }

    dwarf::taddr deref_size (dwarf::taddr address, unsigned size) override {
        return 0;
    }

    dwarf::taddr dw_op_fbreg(dwarf::taddr offset) { return offset+CFA_OFFSET;}

};

#endif /* LIBS_UTIL_BASE_EXPR_CONTEXT_H_ */
