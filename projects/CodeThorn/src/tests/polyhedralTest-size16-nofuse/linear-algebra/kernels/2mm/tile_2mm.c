#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
#   define NK ARRAYSIZE
#   define NL ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
# define _PB_NK ARRAYSIZE
# define _PB_NL ARRAYSIZE
/**
 * 2mm.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 16;
  int nj = 16;
  int nk = 16;
  int nl = 16;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double tmp[16][16];
  double A[16][16];
  double B[16][16];
  double C[16][16];
  double D[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c1;
    int c3;
    int c2;
{
      int c4;
      int c6;
      int c5;
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          tmp[c4][c5] = 0;
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 15; c6++) {
            tmp[c4][c5] += alpha * A[c4][c6] * B[c6][c5];
          }
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          D[c4][c5] *= beta;
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 15; c6++) {
            D[c4][c5] += tmp[c4][c6] * C[c6][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
