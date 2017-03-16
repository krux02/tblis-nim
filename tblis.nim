
# const libtblis = "tblis-c/target/lib/libtblis.a"
# const libtci   = "tblis-c/target/lib/libtci.a"
# {.link: libtblis .}
# {.link: libtci .}
# {.passL: "-lstdc++ -lm -lhwloc -lomp".}
import strutils

const tblisLibDir = staticExec("readlink -f .") & "/tblis-c/target/lib"
{.passL: format("-L$1 -Wl,-rpath,$1 -rdynamic -ltblis", tblisLibDir).}

type
  comm = object
  config = object

  ReduceKind*  = enum
    rkSum      = 0,
    rkSum_Abs  = 1,
    rkMax      = 2,
    rkMax_Abs  = 3,
    rkMin      = 4,
    rkMin_Abs  = 5,
    rkNorm_2   = 6,

  ScalarKind* = enum
    skSingle = 0, skDouble = 1, skSComplex = 2, skDComplex = 3

  complex64*  = tuple[re,im: float32]
  complex128* = tuple[re,im: float64]

  scalar_union* {.union.} = object
    value_f32*: float32
    value_f64*: float64
    value_c64*: complex64
    value_c128*: complex128

  scalar = object
    case kind: ScalarKind
    of skSingle:
      value_f32: float32
    of skDouble:
      value_f64: float64
    of skSComplex:
      value_c64: complex64
    of skDComplex:
      value_c128: complex128

  tensor* = object
    kind*: ScalarKind
    conj*: int32
    scalar*: scalar_union
    data*: pointer
    ndim*: int32
    len*,stride*: ptr int

const
  rkNorm_1*   = rkSumAbs
  rkNorm_Inf* = rkMaxAbs
  skFloat*    = skSingle

proc add*(comm: ptr comm; cfg: ptr config;
             A: ptr tensor; idx_A: cstring;
             B: ptr tensor; idx_B: cstring) {.importc: "tblis_tensor_add".}

proc set*(comm: ptr comm, cfg: ptr config,
         alpha: ptr scalar, A: ptr tensor, idx_A: cstring) {.importc: "tblis_tensor_set".}

proc scale*(comm: ptr comm, cfg: ptr config,
               A: ptr tensor, idx_A: cstring) {.importc: "tblis_tensor_scale".}

proc reduce*(comm: ptr comm, cfg: ptr config,
                         op: ReduceKind, A: ptr tensor, idx_A: cstring,
                         result: ptr scalar, idx: ptr int) {.importc: "tblis_tensor_reduce".}

proc dot*(comm: ptr comm, cfg: ptr config,
             A: ptr tensor, idx_A: cstring,
             B: ptr tensor, idx_B: cstring,
        result: ptr scalar) {.importc: "tblis_tensor_dot".}

proc mult*(comm: ptr comm, cfg: ptr config,
              A: ptr tensor, idx_A: cstring,
              B: ptr tensor, idx_B: cstring,
              C: ptr tensor, idx_C: cstring) {.importc: "tblis_tensor_mult".}

#[
void tblis_tensor_add(const tblis_comm* comm, const tblis_config* cfg,
                      const tblis_tensor* A, const label_type* idx_A,
                            tblis_tensor* B, const label_type* idx_B);

void tblis_tensor_set(const tblis_comm* comm, const tblis_config* cfg,
                      const tblis_scalar* alpha, tblis_tensor* A, const label_type* idx_A);

void tblis_tensor_scale(const tblis_comm* comm, const tblis_config* cfg,
                        tblis_tensor* A, const label_type* idx_A);

void tblis_tensor_reduce(const tblis_comm* comm, const tblis_config* cfg,
                         reduce_t op, const tblis_tensor* A, const label_type* idx_A,
                         tblis_scalar* result, len_type* idx);

void tblis_tensor_dot(const tblis_comm* comm, const tblis_config* cfg,
                      const tblis_tensor* A, const label_type* idx_A,
                      const tblis_tensor* B, const label_type* idx_B,
                      tblis_scalar* result);

void tblis_tensor_mult(const tblis_comm* comm, const tblis_config* cfg,
                       const tblis_tensor* A, const label_type* idx_A,
                       const tblis_tensor* B, const label_type* idx_B,
                             tblis_tensor* C, const label_type* idx_C);
]#
