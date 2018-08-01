#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<limits.h>
int a[10];
void bar(){
	int b[10];
        int i;
       for (i=0;i<10;i++){
          b[i] = i;
          a[i] = b[i] +1;
       }
}
void foo(){
	
}

int main(){
	foo();
        bar();
}
