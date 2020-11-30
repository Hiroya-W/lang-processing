#include "mppl_compiler.h"

struct TYPE {
    int ttype;           /* TPINT TPCHAR TPBOOL TPARRAY TPARRAYINT TPARRAYCHA RTPARRAYBOOL TPPROC */
    int arraysize;       /* size of array, if TPARRAY */
    struct TYPE *etp;    /* pointer to element type if TPARRAY */
    struct TYPE *paratp; /* pointer to parameter's type list if ttype is TPPROC */
};

struct LINE {
    int reflinenum;
    struct LINE *nextlinep;
};

struct ID {
    char *name;
    char *procname; /* procedure name within which this name is defined */ /* NULL if global name */
    struct TYPE *itp;
    int ispara; /* 1:formal parameter, 0:else(variable) */
    int deflinenum;
    struct LINE *irefp;
    struct ID *nextp;
} * globalidroot, *localidroot; /* Pointers to root of global & local symbol tables */

/* Initialise the table */
void init_idtab() {
    globalidroot = NULL;
    localidroot = NULL;
}
