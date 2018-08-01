#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<limits.h>
int a,b;
void bar(int a,int b){
	printf("bar$$:%p",__builtin_frame_address (0));
	int c = a;
	int d = b;
}
void foo(){
	printf("foo$$:%p",__builtin_frame_address (0));
	int x=0;
	int y = 1;
	bar(x,y);
}

int main(){
	printf("main$$:%p",__builtin_frame_address (0));
	int i =0;
	int j =1;
	int* x = (int*) malloc(100*sizeof(int));
	foo();
	printf("hello%d\n",i);
	/***********************Integer Overflow*****************************************/
	int a =INT_MAX;
	int b = INT_MIN;
	int c = 10;
	int d = 2;
	int e = 3;
	int x = a + c;
	printf("%d\n",x);
	printf("%d\n",b-c);
	printf("%d\n",a*d);
	printf("%d\n",a/d);
	/*************************Double Free***********************************************/
	char * p = (char*) malloc(100000);
	char* q;
	free(p);
	free(q);
	free(p);
	/*************************System Credential***********************************************/
	int f = 1;
	setgid(f);
	setregid(3,2);
	setresgid(5);
	setresuid(7);
	setreuid(9,2);

	setgid(6);
	setregid(7,8);
	setresgid(3);
	setresuid(2);
	setreuid(10,4);
}
