#include "mppl_compiler.h"

/*!
 * @brief Type for the name
 */
struct TYPE {
    int ttype;           /*! TPINT TPCHAR TPBOOL TPARRAY TPARRAYINT TPARRAYCHA RTPARRAYBOOL TPPROC */
    int arraysize;       /*! size of array, if TPARRAY */
    struct TYPE *etp;    /*! pointer to element type if TPARRAY */
    struct TYPE *paratp; /*! pointer to parameter's type list if ttype is TPPROC */
};

/*!
 * @brief List to store the line number
 */
struct LINE {
    int reflinenum;         /*! the line number */
    struct LINE *nextlinep; /*! pointer to next struct */
};

struct ID {
    char *name;                             /*! name */
    char *procname;                         /* procedure name within which this name is defined, NULL if global name */
    struct TYPE *itp;                       /*! Type for the name */
    int ispara;                             /*! 1:formal parameter, 0:else(variable) */
    int deflinenum;                         /*! Name defined line number */
    struct LINE *irefp;                     /*! List of line numbers where the name was referenced */
    struct ID *nextp;                       /*! pointer next struct */
} * globalidroot, *localidroot, *crtabroot; /*! Pointers to root of global only & local only & global + local symbol tables */

/*! Release the ID struct */
static void free_strcut(struct ID *);

/*! Initialise the table */
void init_crtab() {
    globalidroot = NULL;
    localidroot = NULL;
    crtabroot = NULL;
    return;
}

/*! Output the cross reference table */
void print_crtab() {
    struct ID *p;
    struct LINE *q;

    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Name\tType\tDef. | Ref\n");
    for (p = crtabroot; p != NULL; p = p->nextp) {
        fprintf(stdout, "%s", p->name);
        if (p->procname != NULL) {
            fprintf(stdout, ":%s", p->procname);
        }
        fprintf(stdout, "\t");
        fprintf(stdout, "%s\t", tokenstr[p->itp->ttype]);
        fprintf(stdout, "%d ", p->deflinenum);
        for (q = p->irefp; q != NULL; q = q->nextlinep) {
            fprintf(stdout, "%d", q->reflinenum);
            fprintf(stdout, "%s", q->nextlinep == NULL ? "\n" : ",");
        }
    }
    fprintf(stdout, "--------------------\n");
    return;
}

/*! Release tha data structure */
void release_crtab() {
    free_strcut(globalidroot);
    free_strcut(localidroot);
    free_strcut(crtabroot);

    init_crtab();
    return;
}

/*!
 * @brief Release the data struct
 * @param[in] root The root of the list struct
 */
static void free_strcut(struct ID *root) {
    struct ID *p, *q;

    for (p = root; p != NULL; p = q) {
        free(p->name);
        free(p->procname);
        free(p->itp);
        free(p->irefp);
        free(p->nextp);
        q = p->nextp;
        free(p);
    }
    return;
}
