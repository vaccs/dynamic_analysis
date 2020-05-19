# VACCS Dynamic Analysis

This project contains the runtime analysis used in the VACCS project. The analyses currrently supported are

1. Program Address Space - monitors the static data area, runtime stack and heap for modifications to variables
2. Malloc and Free - monitors calls to malloc and free and detects memory errors
3. Secure Data - monitors variables declared as secure data to make sure they are not written to disk
4. File Operations - this is currently under construction

The command line syntax is

`pas [-u] [-r] [-m] [-s] [-f] <executable> [executable args]`

- `-u` - analyze user code only
- `-r` - monitor register update
- `-m` - monitor calls to malloc and free
- '-s' - monitor secure data
- '-f' - monitor file operations

The file `dyanmic_analysis/bashrc` has all of environment variables that are needed. Make sure `VACCS` is defined to be the
directory containing `dynamic_analysis`.

The software needed to run the dyanmic analysis is:

1. Ubuntu 18.04
2. gcc 7.5
3. g++ 7.5
4. make
5. git
6. python 3.7

To compile the dynamic analysis type `make` in the `dynamic_analysis` directory. The `pas` script resides in `dynamic_analysis/pin/scripts`.
This directory is added to your path in `dynamic_analysis/bashrc`.
