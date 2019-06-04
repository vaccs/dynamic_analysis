cmake_minimum_required(VERSION 3.14)
project(PAS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(PAS
    database_access_api.cpp
    database_access_api.h
    database_direct_access.cpp
    database_direct_access.h
    function_call.cpp
    function_call.h
    functionInvocation.cpp
    functionInvocation.h
    global.cpp
    global.h
    InstructionInstrumentation.cpp
    InstructionInstrumentation.h
    makefile
    makefile.rules
    malloc_free.cpp
    malloc_free.h
    memory_access.cpp
    memory_access.h
    monitor_registers.cpp
    monitor_registers.h
    pas_analysis.cpp
    return.cpp
    return.h
    section_info.cpp
    section_info.h
    stack_operation.cpp
    stack_operation.h)
