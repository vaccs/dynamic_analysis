#include <stdio.h>
#include <stdlib.h>

int a;
int c = 3;
int main() {

   int b;
   int *p = malloc(sizeof(int));

   
   a = 1;
   b = 2;
   *p = 5;
   printf("a = %d, b = %d, *p = %d, c = %d\n",a, b, *p, c);
}
