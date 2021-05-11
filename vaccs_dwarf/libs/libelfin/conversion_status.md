# C++03 Conversion Notes

## Changes made

### Static Asserts

Usage of `std::static_assert` has been changed to an older-style static assert.

Example:

```
static_assert(sizeof(tmp1) == sizeof(taddr), "taddr is not 64 bits");
// changed to
typedef int static_assert_64bits[sizeof(tmp1) == sizeof(taddr) ? 1 : -1];
```

The second assert works by trying to `typedef` a type to an array of size 1 or -1. If the check fails, it will try to make the array -1 units long, which is impossible, hence a compile time error.

### `std::is_integral`

`std::is_inegral`, a check for whether a type is an integer type or not, has been implemented by hand.

### `std::uint8` Etc.

Changed these C++ types to their C equivalents.

### Smart Pointers

C++11 libelfin made heavy use of `std::shared_ptr`, which have all been blindly converted to normal pointers. This of course implies massive memory leaks. Given then scale of usage of libelfin in this project, it was deemed not to be a large problem.

### Unordered Map

`std::unordered_map` is only available in C++11. In general, these can be swapped out for `std::map`.

### Scoped Enums

C++11 has "scoped enums" where unum's inner types are not exposed globally and their underlying type can be specified.

This is tricky to replicate in C++03, especially the underlying type feature.

I replaced enums with typedefs to their underlying type and made a list of const variables of that type in a namespace.

### auto

The auto keyword is C++11 only. These have been replaced with explicit type names.

### initializer lists

Initializer lists have been replaced with std::vectors.

### std::to_string

std::to_string is C++11 only. I created a fakestd::to_string which has the same functionality and replaced calls to std::to_string.

### move semantics and std::move

Any calls to std::move have been removed. Move semantics are mostly a memory saving and optimization technique, and can be removed.

### auto generated to_string.cc

In the makefile of this library, there are calls to ./elf/enum-print.py. This script takes a header file of enums and generates a 
c++ file with to_string methods. This had to be modified since I changed usage of "enum class" to typedefs and namespaces.

## What's still broken

### to_string.cc

The auto generated to_string.cc now suffers from function redefinition errors. This is because using typedef does not actually
make a unique type, only an alias to the underlying type. e.g.

```c++
typedef int foo_t;
typedef int bar_t;

void func(foo_t f) {};
void func(bar_t f) {};
```
Is not allowed, since foo_t and bar_t are both ints, meaning `func(int)` gets defined twice.

### Unit tests

A workaround for the above problem is to comment out and conflicting functions. This would of course make the calls to to_string
print the wrong value, but you can get the library to compile this way.

After the library is compiled, running `make check` will run unit tests.

These tests currently fail. At this time, it's not known exactly why.