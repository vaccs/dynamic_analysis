#include <stdio.h>
#include <stdlib.h>
int a;
int main() {

   int b;
   int *c = malloc(sizeof(int));

   a = 1;
   b = 2;
   printf("&a = %p, &b - %p, &c = %p\n",&a, &b,c);
}
