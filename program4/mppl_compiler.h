#ifndef _MPPL_COMPILER_H_
#define _MPPL_COMPILER_H_

/* pretty-printer.h  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define NORMAL 0

/*! maximum length of a string */
#define MAXSTRSIZE 1024

/*! @name definition of token code */
/* @{ */
/*! number of tokens */
#define NUMOFTOKEN 49
/*! Name : Alphabet { Alphabet | Digit } */
#define TNAME 1
/*! program : Keyword */
#define TPROGRAM 2
/*! var : Keyword */
#define TVAR 3
/*! array : Keyword */
#define TARRAY 4
/*! of : Keyword */
#define TOF 5
/*! begin : Keyword */
#define TBEGIN 6
/*! end : Keyword */
#define TEND 7
/*! if : Keyword */
#define TIF 8
/*! then : Keyword */
#define TTHEN 9
/*! else : Keyword */
#define TELSE 10
/*! procedure : Keyword */
#define TPROCEDURE 11
/*! return : Keyword */
#define TRETURN 12
/*! call : Keyword */
#define TCALL 13
/*! while : Keyword */
#define TWHILE 14
/*! do : Keyword */
#define TDO 15
/*! not : Keyword */
#define TNOT 16
/*! or : Keyword */
#define TOR 17
/*! div : Keyword */
#define TDIV 18
/*! and : Keyword */
#define TAND 19
/*! char : Keyword */
#define TCHAR 20
/*! integer : Keyword */
#define TINTEGER 21
/*! boolean : Keyword */
#define TBOOLEAN 22
/*! readln : Keyword */
#define TREADLN 23
/*! writeln : Keyword */
#define TWRITELN 24
/*! true : Keyword */
#define TTRUE 25
/*! false : Keyword */
#define TFALSE 26
/*! unsigned integer */
#define TNUMBER 27
/*! String */
#define TSTRING 28
/*! + : symbol */
#define TPLUS 29
/*! - : symbol */
#define TMINUS 30
/*! * : symbol */
#define TSTAR 31
/*! = : symbol */
#define TEQUAL 32
/*! <> : symbol */
#define TNOTEQ 33
/*! < : symbol */
#define TLE 34
/*! <= : symbol */
#define TLEEQ 35
/*! > : symbol */
#define TGR 36
/*! >= : symbol */
#define TGREQ 37
/*! ( : symbol */
#define TLPAREN 38
/*! ) : symbol */
#define TRPAREN 39
/*! [ : symbol */
#define TLSQPAREN 40
/*! ] : symbol */
#define TRSQPAREN 41
/*! := : symbol */
#define TASSIGN 42
/*! . : symbol */
#define TDOT 43
/*! , : symbol */
#define TCOMMA 44
/*! : : symbol */
#define TCOLON 45
/*! ; : symbol */
#define TSEMI 46
/*! read : Keyword */
#define TREAD 47
/*! write : Keyword */
#define TWRITE 48
/*! break : Keyword */
#define TBREAK 49
/* @} */

/*! @name definition of Type code */
/* @{ */
/*! number of TYPE */
#define NUMOFTYPE 8
/*! NONE */
#define TPNONE 0
/*! INT */
#define TPINT 1
/*! CHAR */
#define TPCHAR 2
/*! BOOL */
#define TPBOOL 3
/*! ARRAY */
#define TPARRAY 4
/*! INT ARRAY */
#define TPARRAYINT 5
/*! CHAR ARRAY */
#define TPARRAYCHAR 6
/*! BOOL ARRAY */
#define TPARRAYBOOL 7
/*! PROC */
#define TPPROC 8
/* @} */

/*! maximum number of an unsigned integer */
#define MAX_NUM_ATTR 32767

/* pretty-printer.c */
/*! number of keywords */
#define KEYWORDSIZE 28

/*!
 * @brief A pair of token codes for a keyword
 */
extern struct KEY {
    char *keyword; /*! keyword strings */
    int keytoken;  /*! token codes for keywords */
} key[KEYWORDSIZE];

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
    char *name;         /*! name */
    char *procname;     /* procedure name within which this name is defined, NULL if global name */
    struct TYPE *itp;   /*! Type for the name */
    int ispara;         /*! 1:formal parameter, 0:else(variable) */
    int deflinenum;     /*! Name defined line number */
    struct LINE *irefp; /*! List of line numbers where the name was referenced */
    struct ID *nextp;   /*! pointer next struct */
};

extern struct ID *crtabroot;

extern int error(char *mes);

/*! @name scan.c */
/* @{ */
extern FILE *fp;
extern int num_attr;
extern char string_attr[MAXSTRSIZE];
extern int init_scan(char *filename);
extern int scan(void);
extern int get_linenum(void);
extern int end_scan(void);
/* @} */

/*! @name cross_reference.c */
/* @{ */
extern int parse_program(void);
extern int in_subprogram_declaration;
extern int definition_procedure_name;
extern int is_formal_parameter;
/* @} */

/*! @name id-list.c */
/* @{ */
extern char current_procedure_name[MAXSTRSIZE];
extern void set_procedure_name(char *name);
extern int add_globalid_to_crtab(void);
extern void init_crtab(void);
extern void release_crtab(void);
extern int release_localidroot(void);
extern int id_register_without_type(char *name);
extern int id_register_as_type(struct TYPE **type);
extern struct TYPE *std_type(int type);
extern struct TYPE *array_type(int type);
extern int register_linenum(char *name);
extern struct ID *search_procedure(char *procname);
extern void print_tab(struct ID *root);
/* @} */

/*! @name assembler.c */
/* @{ */
extern FILE *out_fp;
extern int init_assemble(char *filename_mppl);
extern int end_assemble(void);
extern int assemble_start(char *program_name);
extern int create_newlabel(char **out);
extern void assemble_procedure_definition();
extern void assemble_if_condition(char *else_label);
extern void assemble_else(char *if_end_label, char *else_label);
extern void assemble_variable_declaration(char *variable_name, char *procname, struct TYPE **type);
extern void assemble_call(struct ID *id_procedure);
extern void assemble_MULA();
extern void assemble_DIVA();
extern void assemble_AND();

/* @} */

/*! @name main.c */
/* @{ */
extern char *tokenstr[NUMOFTOKEN + 1];
extern char *typestr[NUMOFTYPE + 1];
extern int token;
/* @} */

#endif
