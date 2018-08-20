#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "vaccs_printf.h"
int a;
int main() {

   int b;
   int *p = &b;

   vaccs_printf("start program\n");
   
   a = 1;
   b = 2;
   vaccs_printf("a = %d, b = %d\n",a, b);
   vaccs_printf("p = %ld, *p = %d\n",p, *p);
}
