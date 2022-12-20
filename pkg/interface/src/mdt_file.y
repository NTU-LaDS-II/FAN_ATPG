%{
// **************************************************************************
// File       [ mdt_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/02/23 created ]
// **************************************************************************

// #define YYPARSE_PARAM param

#include <cstdio>
#include <cstring>

#include "mdt_file.h"

using namespace IntfNs;

extern char *mdt_filetext;
extern int  mdt_fileline;
extern int  mdt_filelex(void);
void mdt_fileerror(void* a,char const *msg);

void mdt_filefreeNames(MdtNames *names);
void mdt_filefreePortToNet(MdtPortToNet *p2ns);

IntfNs::MdtFile *mdt;

%}

%parse-param {void* param}


%union {
    char                 ychar[IntfNs::NAME_LEN];
    IntfNs::MdtPortType  yportType;
    IntfNs::MdtNames     *ynames;
    IntfNs::MdtPortToNet *yportToNet;
}

%token MODEL MODEL_SOURCE INPUT OUTPUT INOUT INTERN PRIMITIVE INSTANCE
%token <ychar> NAME

%type <yportType>  body_type
%type <ynames>     list
%type <yportToNet> mapping_list

%start start

%%

start
    : initialize models
    ;


initialize
    : { mdt = (MdtFile *)(param); }
    ;


models
    : models model
    | model
    ;


model
    : MODEL_SOURCE '=' NAME
    | MODEL add_model '(' model_port ')' '(' model_bodies ')'
    ;


add_model
    : NAME { mdt->addModel($1); }
    ;


model_port
    : list { mdt->addPorts($1->head); }
    ;


model_bodies
    : model_bodies model_body
    | model_body
    ;


model_body
    : body_net '(' attributes ')'
    | body_net '(' ')'
    ;

body_net
    : body_type '(' list ')' { switch ($1) {
                                   case MDT_PORT_INPUT:
                                       mdt->setInputNets($3->head);
                                       break;
                                   case MDT_PORT_OUTPUT:
                                       mdt->setOutputNets($3->head);
                                       break;
                                   case MDT_PORT_INOUT:
                                       mdt->setInoutNets($3->head);
                                       break;
                                   case MDT_PORT_INTERN:
                                       mdt->setInternNets($3->head);
                                       break;
                               }
                               mdt_filefreeNames($3->head);          }
    ;

body_type
    : INPUT  { $$ = MDT_PORT_INPUT;  }
    | OUTPUT { $$ = MDT_PORT_OUTPUT; }
    | INOUT  { $$ = MDT_PORT_INOUT;  }
    | INTERN { $$ = MDT_PORT_INTERN; }
    ;


attributes
    : attributes attribute
    | attribute
    ;


attribute
    : primitive
    | instance
    ;


primitive
    : PRIMITIVE '=' NAME NAME '(' list ')' ';' {
        mdt->addPrimitive($3, $4, $6->head);
        mdt_filefreeNames($6->head);
    }
    | PRIMITIVE '=' NAME '(' list ')' ';' {
        mdt->addPrimitive($3, "", $5->head);
        mdt_filefreeNames($5->head);
    }
    ;


instance
    : INSTANCE '=' NAME NAME '(' list ')' ';' {
        mdt->addInstance($3, $4, $6->head);
        mdt_filefreeNames($6->head);
    }
    | INSTANCE '=' NAME '(' list ')' ';' {
        mdt->addInstance($3, "", $5->head);
        mdt_filefreeNames($5->head);
    }
    | INSTANCE '=' NAME NAME '(' mapping_list ')' ';' {
        mdt->addInstance($3, $4, $6->head);
        mdt_filefreePortToNet($6->head);
    }
    | INSTANCE '=' NAME '(' mapping_list ')' ';' {
        mdt->addInstance($3, "", $5->head);
        mdt_filefreePortToNet($5->head);
    }
    ;


list
    : list ',' NAME { $$ = new MdtNames;
                      strcpy($$->name, $3);
                      $1->next = $$;
                      $$->head = $1->head;
                      $$->next = NULL;                                   }
    | list ','      { $$ = new MdtNames;
                      strcpy($$->name, "");
                      $1->next = $$;
                      $$->head = $1->head;
                      $$->next = NULL;                                   }
    | NAME          { $$ = new MdtNames;
                      strcpy($$->name, $1);
                      $$->head = $$;
                      $$->next = NULL;                                   }
    |               { $$ = new MdtNames;
                      strcpy($$->name, "");
                      $$->head = $$;
                      $$->next = NULL;                                   }
    ;


mapping_list
    : mapping_list ',' '.' NAME '(' NAME ')' {
        $$ = new MdtPortToNet;
        strcpy($$->port, $4);
        strcpy($$->net, $6);
        $1->next = $$;
        $$->head = $1->head;
        $$->next = NULL;
    }
    | '.' NAME '(' NAME ')' {
        $$ = new MdtPortToNet;
        strcpy($$->port, $2);
        strcpy($$->net, $4);
        $$->head = $$;
        $$->next = NULL;
    }
    ;


%%

void mdt_fileerror(void* a, const char *msg) {
    fprintf(stderr, "**ERROR mdt_fileerror(): ");
    fprintf(stderr, "At line %d. Near `%s'. ", mdt_fileline, mdt_filetext);
    fprintf(stderr, "%s\n", msg);
}

void mdt_filefreeNames(MdtNames *names) {
    MdtNames *name = names->head;
    while (name) {
        MdtNames *prev = name;
        name = name->next;
        delete prev;
        prev = NULL;
    }
}

void mdt_filefreePortToNet(MdtPortToNet *p2ns) {
    MdtPortToNet *p2n = p2ns->head;
    while (p2n) {
        MdtPortToNet *prev = p2n;
        p2n = p2n->next;
        delete prev;
        prev = NULL;
    }
}

