#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * syr2k.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[16][16];
  double A[16][16];
  double B[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c6;
    int c3;
    int c4;
    int c2;
    int c1;
#pragma omp parallel for private(c2, c4, c6)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 1; c2++) {
        for (c4 = 13 * c1; c4 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 13 * c2; c6 <= ((15 < 13 * c2 + 12?15 : 13 * c2 + 12)); c6++) {
            C[c4][c6] *= beta;
          }
        }
      }
    }
#pragma omp parallel for private(c2, c4, c3, c6)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 1; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c4 = 13 * c1; c4 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 13 * c2; c6 <= ((15 < 13 * c2 + 12?15 : 13 * c2 + 12)); c6++) {
              C[c4][c6] += alpha * A[c4][c3] * B[c6][c3];
              C[c4][c6] += alpha * B[c4][c3] * A[c6][c3];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
