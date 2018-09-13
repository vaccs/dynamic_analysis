#include <stdio.h>
int a;
int main() {

   int b;
   int *p = &b;

   
   a = 1;
   b = 2;
   printf("a = %d, b = %d, *p = %d\n",a, b, *p);
}
