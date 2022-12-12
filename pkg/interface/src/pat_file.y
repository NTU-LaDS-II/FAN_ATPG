%{
// **************************************************************************
// File       [ pat_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ ]
// **************************************************************************

// #define YYPARSE_PARAM param

#include <cstdio>
#include <cstring>

#include "pat_file.h"

using namespace std;
using namespace IntfNs;

extern char *pat_filetext;
extern int  pat_fileline;
extern int  pat_filelex(void);

int pat_fileparse(void*);
void pat_fileerror(void* a,char const *msg);
void pat_filefreeNames(PatNames *names);

PatFile *pat;

%}

%parse-param {void* param}


%union {
    char             *ychar;
    int              yint;
    IntfNs::PatType  ypatType;
    IntfNs::PatNames *ynames;
}


%token PATTERN
%token <ychar>    NAME
%token <yint>     PATTERN_NUM
%token <ypatType> PATTERN_TYPE

%type <ynames> names
%type <ychar>  nameq

%start start

%%

start
    : initialize header pattern_type pattern_num patterns
    ;

initialize
    : { pat = (PatFile *)(param); }
    ;

header
    : names '|' names '|' names { pat->setPiOrder($1->head);
                                  pat->setPpiOrder($3->head);
                                  pat->setPoOrder($5->head);
                                  pat_filefreeNames($1->head);
                                  pat_filefreeNames($3->head);
                                  pat_filefreeNames($5->head); }
    | names '|' '|' names       { pat->setPiOrder($1->head);
                                  pat->setPoOrder($4->head);
                                  pat_filefreeNames($1->head);
                                  pat_filefreeNames($4->head); }
    ;

pattern_type
    : PATTERN_TYPE { pat->setPatternType($1); }
    ;

pattern_num
    : PATTERN_NUM { pat->setPatternNum($1); }
    ;

patterns
    : patterns pattern
    |
    ;


names
    : names NAME { $$ = new PatNames;
                   strcpy($$->name, $2);
                   $1->next = $$;
                   $$->head = $1->head;
                   $$->next = NULL;                                   }
    | NAME       { $$ = new PatNames;
                   strcpy($$->name, $1);
                   $$->head = $$;
                   $$->next = NULL;                                   }
    ;


pattern
    : PATTERN nameq '|' nameq '|' nameq '|' nameq '|' nameq '|' nameq '|' nameq {
        pat->addPattern($2, $4, $6, $8, $10, $12, $14);
        delete [] $2;
        delete [] $4;
        delete [] $6;
        delete [] $8;
        delete [] $10;
        delete [] $12;
        delete [] $14;
    }
    ;

nameq
    : NAME { $$ = $1;   }
    |      { $$ = NULL; }
    ;

%%

void pat_fileerror(void *a,const char *msg) {
    fprintf(stderr, "**ERROR pat_fileerror(): ");
    fprintf(stderr, "At line %d. Near `%s'. ", pat_fileline, pat_filetext);
    fprintf(stderr, "%s\n", msg);
}

void pat_filefreeNames(PatNames *names) {
    PatNames *name = names->head;
    while (name) {
        PatNames *prev = name;
        name = name->next;
        delete prev;
    }
}

