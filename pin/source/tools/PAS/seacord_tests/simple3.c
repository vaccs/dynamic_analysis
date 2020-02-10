#include <stdio.h>
#include <stdlib.h>

int global;
const int roglobal;

int main() {
  int local = 1;
  global = local + roglobal;
  return 0;
}
