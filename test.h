

typedef enum
{
    TYPE_SINGLE   = 0,
    TYPE_FLOAT    = TYPE_SINGLE,
    TYPE_DOUBLE   = 1,
    TYPE_SCOMPLEX = 2,
    TYPE_DCOMPLEX = 3
} type_t;

type
  type_t* = enum
    TYPE_SINGLE = 0, TYPE_DOUBLE = 1, TYPE_SCOMPLEX = 2, TYPE_DCOMPLEX = 3

const
  TYPE_FLOAT = TYPE_SINGLE


typedef enum
{
    TYPE_SINGLE   = 0,
    TYPE_FLOAT    = TYPE_SINGLE,
    TYPE_DOUBLE   = 1,
    TYPE_SCOMPLEX = 2,
    TYPE_DCOMPLEX = 3
} type_t;
