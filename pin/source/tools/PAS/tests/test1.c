#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int a;
int main() {

   int b;

   
   printf("client stdout address = 0x%lx, value = 0x%lx, fd = %d\n",
         (long int)&stdout,
         (long int)stdout,
         STDOUT_FILENO);
   printf("again\n");
   a = 1;
   printf("again\n");
   b = 2;
   printf("a = %d, b = %d\n",a, b);
}
