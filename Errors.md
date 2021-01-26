**The list of errors detected by the dynamic analysis are:**

1. Invalid memory access
1. Illegal instruction
1. Corrupted stack
1. Stack smashing of old frame pointer
1. Stack smashing of return address
1. Memory protection violation
1. Invalid memory page
1. Divide by zero
1. Integer overflow
1. Array out of bounds
1. Floating-point divide by zero
1. Floating-point overflow
1. Floating-point undeflow
1. Floating-point inexact result
1. Floating-point invalid operation
1. Floating-point denormalized operand
1. Floating-point stack error


**Future errors reported**

1. Malloc and free errors
1. Secure data errors
1. File operation errors


The analysis of malloc and free and secure data have been implemented. However, errors can currently only be observed using the visualization. Additional, code will need to be added to detect and report the errors at runtime.

File operation analysis has been started but not completed.