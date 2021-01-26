**Objectives**

Students will be able to write C code that obeys the following programming practices

1. Design with security in mind; not an “add-on”
1. Include lots of logging
1. Avoid buffer overflows

1. Check all library and system call return values

1. Avoid use of system and popen calls; if programs must be spawned, use only execve(), execv(), or execl() and be careful

1. Don’t design your program to depend on UNIX environment variables

1. Use full pathnames for any filename argument, for both commands and data files

1. Do not trust user input

1. Avoid file system errors

1. Test for assumptions about operating environment

1. Ensure secure data such as cryptographic keys are not written to disk
