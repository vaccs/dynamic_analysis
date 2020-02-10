#include <stdio.h>
#include <stdlib.h>


size_t assignSize() {
  return sizeof("Rappan Athuk\n");
}

int main() {
  char* words = malloc
   (
      assignSize()
   );
  sprintf(words, "Rappan Athuk\n");
  printf("%s", words);
  return 0;
} /* */
/*
typedef struct str {
  int a;
  float b[5];
} str;

int main() {
  str strin;                                                    // ACC
  strin.a = 999;
  strin.b[2] = 777.77;
  strin.a = 1500;
  int x = 5;                                                    // ACC
  int numbers[8] = { 5, 10, 15, 20, 25 };                       // ACC
  float* fractions = malloc(10 * (sizeof *fractions));          // ACC
  int* y[5];                                                    // ACC
  char* z[5];                                                   // ACC
  int** doublePointer = malloc(5 * (sizeof *doublePointer));    // ACC
  char** doublePointer2 = malloc(5 * (sizeof *doublePointer2)); // ACC
  doublePointer[1] = &x;
  z[1] = malloc(sizeof("Tomb of Horrors\n"));
  sprintf(z[1], "Tomb of Horrors\n");
  fractions[3] = 6.6;
  fractions[5] = 9.9;
  char* words = malloc(sizeof("Rappan Athuk\n"));            // ACC
  sprintf(words, "Rappan Athuk\n");
  printf("%s", words);
  return 0;
} /* */
