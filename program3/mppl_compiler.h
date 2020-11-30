#ifndef _PRETTY_PRINTER_H_
#define _PRETTY_PRINTER_H_

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

/*! number of tokens */
#define NUMOFTOKEN 49

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
/* @} */

/*! @name id-list.c */
/* @{ */
/* @} */
extern void init_crtab();
extern void release_crtab();

/*! @name main.c */
/* @{ */
extern char *tokenstr[NUMOFTOKEN + 1];
extern int token;
/* @} */

#endif
