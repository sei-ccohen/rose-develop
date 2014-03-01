/**
 * jacobi-1d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16];
  double B[16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c0;
    for (c0 = 0; c0 <= 1; c0++) {
      B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
      for (c1 = 2 * c0 + 2; c1 <= 2 * c0 + 14; c1++) {
        B[-2 * c0 + c1] = 0.33333 * (A[-2 * c0 + c1 - 1] + A[-2 * c0 + c1] + A[-2 * c0 + c1 + 1]);
        A[-2 * c0 + c1 + -1] = B[-2 * c0 + c1 + -1];
      }
      A[14] = B[14];
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
