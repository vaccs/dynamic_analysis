#include <stdio.h>
#include <stdlib.h>


void subFunc() {
  int x = 4;
  int y = 3;
  int z = x + y;
}

int main() {
  int a = 2;
  int b = 8;
  subFunc();
  int c = a + b;
}
