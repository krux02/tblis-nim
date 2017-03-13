#include "tblis/src/tblis.h"

int main() {
  len_type len_A = {10, 9, 2, 5};
  stride_type stride_A = {1, 10, 90, 180};
  double data_A[10 * 9 * 2 * 5];
  tblis_tensor A;
  A.type = TYPE_DOUBLE;
  *(double *)A.scalar = 1.0;
  A.data = data_A;
  A.ndim = 4;
  A.len = len_A;
  A.stride = stride_A;

  len_type len_B = {7, 5, 9, 8};
  stride_type stride_B = {1, 7, 35, 315};
  double data_B[7 * 5 * 9 * 8];
  tblis_tensor B;
  B.type = TYPE_DOUBLE;
  *(double *)B.scalar = 1.0;
  B.data = data_B;
  B.ndim = 4;
  B.len = len_B;
  B.stride = stride_B;

  len_type len_C = {7, 2, 10, 8};
  stride_type stride_C = {1, 7, 14, 140};
  double data_C[7 * 2 * 10 * 8];
  tblis_tensor C;
  C.type = TYPE_DOUBLE;
  *(double *)B.scalar = 1.0;
  C.data = data_C;
  C.ndim = 4;
  C.len = len_C;
  C.stride = stride_C;

  // initialize data_A and data_B...
  // this computes C[abcd] += A[cebf] B[afed]
  tblis_tensor_mult(NULL, NULL, &A, "cebf", &B, "afed", &C, "abcd");
}
