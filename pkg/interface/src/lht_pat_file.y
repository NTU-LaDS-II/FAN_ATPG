%{
// **************************************************************************
// File       [ lht_pat_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/14 created ]
// **************************************************************************

// #define YYPARSE_PARAM param

#include <cstdio>
#include <cstring>

#include "lht_pat_file.h"

using namespace std;
using namespace IntfNs;

extern char *lht_pat_filetext;
extern int  lht_pat_fileline;
extern int  lht_pat_filelex(void);

void lht_pat_fileerror(void* param, char const *msg);

LhtPatFile *lht_pat;

%}

%parse-param {void* param}

%union {
    char            *ychar;
    IntfNs::PatType ypatType;
}


%token PATTERN
%token <ychar>    VALUE
%token <ypatType> PATTERN_TYPE

%type <ychar>  valueq

%start start

%%

start
    : initialize patterns
    ;

initialize
    : { lht_pat = (LhtPatFile *)(param); }
    ;

patterns
    : patterns pattern
    |
    ;

pattern
    : PATTERN VALUE '_' valueq '|' VALUE '_' valueq {
        lht_pat->setPatternType(IntfNs::BASIC_SCAN);
        lht_pat->addPattern($2, NULL, $4, NULL, $6, NULL, $8);
        delete [] $2;
        delete [] $4;
        delete [] $6;
        delete [] $8;
    }
    | PATTERN VALUE '-' '>' VALUE '_' VALUE '@' VALUE '|' VALUE '_' VALUE {
        lht_pat->setPatternType(IntfNs::LAUNCH_SHIFT);
        lht_pat->addPattern($2, $5, $7, $9, NULL, $11, $13);
        delete [] $2;
        delete [] $5;
        delete [] $7;
        delete [] $9;
        delete [] $11;
        delete [] $13;
    }
    | PATTERN VALUE '-' '>' VALUE '_' valueq '|' VALUE '_' valueq {
        lht_pat->setPatternType(IntfNs::LAUNCH_CAPTURE);
        lht_pat->addPattern($2, $5, $7, NULL, NULL, $9, $11);
        delete [] $2;
        delete [] $5;
        delete [] $7;
        delete [] $9;
        delete [] $11;
    }
    ;

valueq
    : VALUE { $$ = $1;   }
    |       { $$ = NULL; }
    ;

%%

void lht_pat_fileerror(void* a,const char *msg) {
    fprintf(stderr, "**ERROR pat_fileerror(): ");
    fprintf(stderr, "At line %d. ", lht_pat_fileline);
    fprintf(stderr, "Near `%s'. %s\n", lht_pat_filetext, msg);
}

