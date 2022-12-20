%{
// **************************************************************************
// File       [ vlog_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ yacc for parsing verilog file ]
// Date       [ 2010/07/07 created ]
// **************************************************************************

// #define YYPARSE_PARAM param

#include <cstdio>
#include <cstring>

#include "vlog_file.h"

using namespace IntfNs;

extern char *vlog_filetext;
extern int vlog_fileline;
extern int  vlog_filelex(void);

int vlog_fileparse(void*);
void vlog_fileerror(void*, char const *msg);
void vlog_filefreeNames(VlogNames *names);
void vlog_filefreePortToNet(VlogPortToNet *p2ns);

IntfNs::VlogFile *vlog;

%}

%parse-param {void* param}

%union {
    char                  ychar[IntfNs::NAME_LEN];
    IntfNs::VlogNetType   ynetType;
    IntfNs::VlogNames     *ynames;
    IntfNs::VlogPortToNet *yportToNet;
}

%token MODULE ENDMODULE PRIMITIVE ENDPRIMITIVE ASSIGN INPUT OUTPUT INOUT
%token WIRE REG SUPPLY_L SUPPLY_H
%token <ychar> NAME NUMBER STRENGTH

%type <ynetType>   net_type
%type <ynames>     list strength_list
%type <yportToNet> mapping_list

%start start


%%

start
    : initialize modules
    ;


initialize
    : { vlog = (VlogFile *)(param); }
    ;


modules
    : modules module
    | module
    ;


module
    : MODULE add_module ports body ENDMODULE
    | PRIMITIVE add_module ports body ENDPRIMITIVE
    ;


add_module
    : NAME { vlog->addModule($1); }
    ;


ports
    : '(' list ')' ';' { vlog->addPorts($2->head);
                         vlog_filefreeNames($2->head);          }
    | ';'
    ;


body
    : body net
    | body assign
    | body cell
    |
    ;


net
    : net_type list ';' {
        switch ($1) {
            case VLOG_NET_INPUT:
                vlog->setInputNets($2->head);
                break;
            case VLOG_NET_OUTPUT:
                vlog->setOutputNets($2->head);
                break;
            case VLOG_NET_INOUT:
                vlog->setInoutNets($2->head);
                break;
            case VLOG_NET_WIRE:
                vlog->setWireNets($2->head);
                break;
            case VLOG_NET_REG:
                vlog->setRegNets($2->head);
                break;
            case VLOG_NET_SUPPLY_L:
                vlog->setSupplyLNets($2->head);
                break;
            case VLOG_NET_SUPPLY_H:
                vlog->setSupplyHNets($2->head);
                break;
        }
        vlog_filefreeNames($2->head);
    }
    ;


net_type
    : INPUT    { $$ = VLOG_NET_INPUT;    }
    | OUTPUT   { $$ = VLOG_NET_OUTPUT;   }
    | INOUT    { $$ = VLOG_NET_INOUT;    }
    | WIRE     { $$ = VLOG_NET_WIRE;     }
    | REG      { $$ = VLOG_NET_REG;      }
    | SUPPLY_L { $$ = VLOG_NET_SUPPLY_L; }
    | SUPPLY_H { $$ = VLOG_NET_SUPPLY_H; }
    ;


assign
    : ASSIGN NAME '=' NAME ';' { vlog->addAssign($2, $4); }
    ;


cell
    : NAME NAME '(' list ')' ';' {
        vlog->addCell($1, $2, $4->head);
        vlog_filefreeNames($4->head);
    }
    | NAME '(' list ')' ';' {
        vlog->addCell($1, "", $3->head);
        vlog_filefreeNames($3->head);
    }
    | NAME NAME '(' mapping_list ')' ';' {
        vlog->addCell($1, $2, $4->head);
        vlog_filefreePortToNet($4->head);
    }
    | NAME '(' strength_list ')' NAME '(' list ')' ';' {
        vlog->addCell($1, $3->head, $5, $7->head);
        vlog_filefreeNames($3->head);
        vlog_filefreeNames($7->head);
    }
    ;


list
    : list ',' NAME { $$ = new VlogNames;
                      strcpy($$->name, $3);
                      $1->next = $$;
                      $$->head = $1->head;
                      $$->next = NULL;                                     }
    | list ','      { $$ = new VlogNames;
                      strcpy($$->name, "");
                      $1->next = $$;
                      $$->head = $1->head;
                      $$->next = NULL;                                     }
    | NAME          { $$ = new VlogNames;
                      strcpy($$->name, $1);
                      $$->head = $$;
                      $$->next = NULL;                                     }
    |               { $$ = new VlogNames;
                      strcpy($$->name, "");
                      $$->head = $$;
                      $$->next = NULL;                                     }
    ;



strength_list
    : strength_list ',' STRENGTH {
        $$ = new VlogNames;
        strcpy($$->name, $3);
        $1->next = $$;
        $$->head = $1->head;
        $$->next = NULL;
    }
    | STRENGTH {
        $$ = new VlogNames;
        strcpy($$->name, $1);
        $$->head = $$;
        $$->next = NULL;
    }
    ;


mapping_list
    : mapping_list ',' '.' NAME '(' NAME ')' {
        $$ = new VlogPortToNet;
        strcpy($$->port, $4);
        strcpy($$->net, $6);
        $1->next = $$;
        $$->head = $1->head;
        $$->next = NULL;
    }
    | '.' NAME '(' NAME ')' {
        $$ = new VlogPortToNet;
        strcpy($$->port, $2);
        strcpy($$->net, $4);
        $$->head = $$;
        $$->next = NULL;
    }
    ;


%%

void vlog_fileerror(void *a,const char *msg) {
    fprintf(stderr, "**ERROR vlog_fileerror(): ");
    fprintf(stderr, "At line %d. Near `%s'. ", vlog_fileline, vlog_filetext);
    fprintf(stderr, "%s\n", msg);
}

void vlog_filefreeNames(VlogNames *names) {
    VlogNames *name = names->head;
    while (name) {
        VlogNames *prev = name;
        name = name->next;
        delete prev;
        prev = NULL;
    }
}

void vlog_filefreePortToNet(VlogPortToNet *p2ns) {
    VlogPortToNet *p2n = p2ns->head;
    while (p2n) {
        VlogPortToNet *prev = p2n;
        p2n = p2n->next;
        delete prev;
        prev = NULL;
    }
}