import tblis

#proc `[]`[T](arg: ptr T; idx: SomeInteger): var T =
#  let tmp = cast[uint](arg) + uint(sizeof(T) * idx)
#  cast[ptr T](tmp)[]

proc `[]`[T](arg: ptr T; idx: SomeInteger): var T {.importcpp: "(#[#])", nodecl.}

proc iota_fill_double(data: ptr float64; len: int64): void =
  let view = cast[ptr array[high(int), float64]](data)
  for i in 0 ..< len:
    view[i] = float64(i)

proc print_gen[T](arg: tensor) =
  let N = arg.len[arg.ndim-1] * arg.stride[arg.ndim-1]
  let data = cast[ptr T](arg.data)
  for i in 0 ..< N:
    echo data[i]
    for j in 1 ..< arg.ndim:
      let stride = arg.stride[j]
      if i mod stride == stride-1:
        echo()

proc print(arg: tensor) =
  case arg.kind
  of skSingle:
    print_gen[float32](arg)
  of skDouble:
    print_gen[float64](arg)
  of skSComplex:
    print_gen[complex64](arg)
  of skDComplex:
    print_gen[complex128](arg)

proc main(): void =
  var len_A = [10, 9, 2, 5]
  var stride_A = [1, 10, 90, 180]
  let dataA = create(float64, 10 * 9 * 2 * 5)
  iota_fill_double(data_A, 10*9*2*5);
  var A: tblis.tensor
  A.kind = skDouble
  A.scalar.value_f64 = 1.0
  A.data = data_A;
  A.ndim = 4;
  A.len = len_A[0].addr
  A.stride = stride_A[0].addr;

  var len_B    = [7, 5, 9, 8]
  var stride_B = [1, 7, 35, 315]
  let data_B   = create(float64, 7 * 5 * 9 * 8);
  iota_fill_double(data_B, 7*5*9*8);

  var B: tblis.tensor
  B.kind = skDouble
  B.scalar.value_f64 = 1.0
  B.data = data_B
  B.ndim = 4;
  B.len = len_B[0].addr;
  B.stride = stride_B[0].addr;

  var len_C = [7, 2, 10, 8]
  var stride_C = [1, 7, 14, 140]
  let data_C = create(float64, 7 * 2 * 10 * 8)

  var C: tblis.tensor

  C.kind = skDouble
  B.scalar.value_f64 = 1.0;
  C.data = data_C;
  C.ndim = 4;
  C.len = len_C[0].addr
  C.stride = stride_C[0].addr

  # initialize data_A and data_B...
  # this computes C[abcd] += A[cebf] B[afed]
  tblis.mult(nil, nil, A.addr, "cebf", B.addr, "afed", C.addr, "abcd");

  echo "tensor A"
  A.print

  echo "tensor B"
  B.print

  echo "tensor C"
  C.print

main()
