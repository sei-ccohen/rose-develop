#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16][16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c4;
    int c2;
    int c3;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c2 = (2 > 13 * c0 + 1?2 : 13 * c0 + 1); c2 <= ((30 < 13 * c0 + 27?30 : 13 * c0 + 27)); c2++) {
        for (c3 = (((0 > (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2))))?0 : (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2)))))) > -13 * c0 + c2 + -26?((0 > (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2))))?0 : (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2)))))) : -13 * c0 + c2 + -26); c3 <= ((((1 < (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2))?1 : (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2)))) < -13 * c0 + c2 + -1?((1 < (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2))?1 : (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2)))) : -13 * c0 + c2 + -1)); c3++) {
          for (c4 = (((13 * c0 > c3 + 1?13 * c0 : c3 + 1)) > c2 + -1 * c3 + -14?((13 * c0 > c3 + 1?13 * c0 : c3 + 1)) : c2 + -1 * c3 + -14); c4 <= ((((13 * c0 + 12 < c3 + 14?13 * c0 + 12 : c3 + 14)) < c2 + -1 * c3 + -1?((13 * c0 + 12 < c3 + 14?13 * c0 + 12 : c3 + 14)) : c2 + -1 * c3 + -1)); c4++) {
            A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4] = (A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4 + 1] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4 + 1] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
