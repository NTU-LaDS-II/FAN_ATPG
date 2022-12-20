%{
// **************************************************************************
// File       [ sdf_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ Currently only supports SDF file header and absolute and
//              increment delay information ]
// Date       [ 2010/11/18 created ]
// **************************************************************************

// #define YYPARSE_PARAM param

#include <cstring>
#include <cstdio>

#include "sdf_file.h"

using namespace IntfNs;

extern char *sdf_filetext;
extern int  sdf_fileline;
int  sdf_filelex(void);
void sdf_fileerror(void* a, char const *msg);

IntfNs::SdfFile *sdf;

%}

%parse-param {void* param}

%union {
    float                     yfloat;
    char                      ychar[IntfNs::NAME_LEN];
    IntfNs::SdfDelayType      ydelayType;
    IntfNs::SdfValue          yvalue;
    IntfNs::SdfDelayValue     ydelayValue;
    IntfNs::SdfDelayValueList ydelayValueList;
    IntfNs::SdfDelayDef       *ydelayDef;
    IntfNs::SdfIoPath         yioPath;
    IntfNs::SdfPortSpec       yportSpec;
}


%token DELAYFILE

%token SDFVERSION DESIGN DATE VENDOR PROGRAM VERSION DIVIDER VOLTAGE PROCESS
%token TEMPERATURE TIMESCALE

%token CELL CELLTYPE INSTANCE

%token DELAY TIMINGCHECK TIMINGENV PATHPULSE PATHPULSEPERCENT ABSOLUTE
%token INCREMENT IOPATH RETAIN COND SCOND CCOND CONDELSE PORT INTERCONNECT
%token DEVICE SETUP HOLD SETUPHOLD RECOVERY REMOVAL RECREM SKEW WIDTH PERIOD
%token NOCHANGE NAME EXCEPTION PATHCONSTRAINT PERIODCONSTRAINT SUM DIFF
%token SKEWCONSTRAINT ARRIVAL DEPARTURE SLACK WAVEFORM POSEDGE NEGEDGE

%token CASE_EQU CASE_INEQU EQU INEQU L_AND L_OR
%token LESS_OR_EQU GREATER_OR_EQU RIGHT_SHIFT LEFT_SHIFT
%token U_NAND U_NOR U_XNOR U_XNOR_ALT

%token <ychar>  BIT_CONST ONE_CONST ZERO_CONST EDGE
%token <ychar>  PATH IDENTIFIER
%token <ychar>  QSTRING
%token <yfloat> NUMBER

%type <ydelayType>      valuetype
%type <ychar>           hchar
%type <ychar>           cell_type cell_instance
%type <ychar>           port_instance port scalar_port bus_port
%type <yvalue>          value triple
%type <ydelayValue>     delval
%type <ydelayValueList> delval_list
%type <ydelayDef>       del_defs del_def_iopath del_def
%type <yioPath>         input_output_path
%type <yportSpec>       port_spec port_edge edge_identifier

%left L_OR
%left L_AND
%left '|'
%left '^' U_XNOR U_XNOR_ALT
%left '&'
%left EQU INEQU CASE_EQU CASE_INEQU
%left '<' LESS_OR_EQU '>' GREATER_OR_EQU
%left RIGHT_SHIFT LEFT_SHIFT
%left '+' '-'
%left '*' '/' '%'
%left '!' '~'

%start start


%%

start
    : initialize delay_file;


initialize
    : { sdf = (SdfFile *)(param); }
    ;


delay_file
    : '(' DELAYFILE sdf_header cells ')'
    ;


sdf_header
    : sdf_header design_name
    | sdf_header date
    | sdf_header vendor
    | sdf_header program_name
    | sdf_header program_version
    | sdf_header hierarchy_divider
    | sdf_header voltage
    | sdf_header process
    | sdf_header temperature
    | sdf_header time_scale
    | sdf_version
    ;


sdf_version
    : '(' SDFVERSION QSTRING ')' { sdf->addVersion($3); }
    ;


design_name
    : '(' DESIGN QSTRING ')' { sdf->addDesign($3); }
    ;


date
    : '(' DATE QSTRING ')' { sdf->addDate($3); }
    ;


vendor
    : '(' VENDOR QSTRING ')' { sdf->addVendor($3); }
    ;


program_name
    : '(' PROGRAM QSTRING ')' { sdf->addProgName($3); }
    ;


program_version
    : '(' VERSION QSTRING ')' { sdf->addProgVersion($3); }
    ;


hierarchy_divider
    : '(' DIVIDER hchar ')' { sdf->addHierChar($3[0]); }
    ;


hchar
    : '/' { $$[0] = '/'; }
    | '.' { $$[0] = '.'; }
    ;


voltage
    : '(' VOLTAGE triple ')' { sdf->addVoltage($3); }
    | '(' VOLTAGE NUMBER ')' { SdfValue v;
                               v.n = 1;
                               v.v[0] = $3;
                               sdf->addVoltage(v);  }
    ;


process
    : '(' PROCESS QSTRING ')' { sdf->addProcess($3); }
    ;

temperature
    : '(' TEMPERATURE triple ')' { sdf->addTemperature($3); }
    | '(' TEMPERATURE NUMBER ')' { SdfValue v;
                                   v.n = 1;
                                   v.v[0] = $3;
                                   sdf->addTemperature(v);  }
    ;


time_scale
    : '(' TIMESCALE tsvalue ')'
    ;


tsvalue
    : NUMBER IDENTIFIER { sdf->addTimeScale($1, $2); }
    ;


cells
    : cells cell
    | cell
    ;


cell
    : '(' CELL add_cell ')'
    | '(' CELL add_cell timing_specs ')'
    ;


add_cell
    : cell_type cell_instance { sdf->addCell($1, $2); }
    ;


cell_type
    : '(' CELLTYPE QSTRING ')' { strcpy($$, $3); }
    ;


cell_instance
    : '(' INSTANCE ')'            { $$[0] = '\0';   }
    | '(' INSTANCE PATH ')'       { strcpy($$, $3); }
    | '(' INSTANCE IDENTIFIER ')' { strcpy($$, $3); }
    | '(' INSTANCE '*' ')'        { $$[0] = '*';
                                    $$[1] = '\0';   }
    ;

timing_specs
    : timing_specs timing_spec
    | timing_spec
    ;


timing_spec
    : del_spec
    | tc_spec
    | te_spec
    ;


del_spec
    : '(' DELAY deltypes ')'
    ;


deltypes
    : deltypes deltype
    | deltype
    ;


tc_spec
    : '(' TIMINGCHECK tchk_defs ')'
    ;


tchk_defs
    : tchk_defs tchk_def
    | tchk_def
    ;


te_spec
    : '(' TIMINGENV te_defs ')'
    ;


te_defs
    : te_defs te_def
    | te_def
    ;


deltype
    : '(' pathtype del_path ')'
    | '(' valuetype del_defs ')'
    ;


pathtype
    : PATHPULSE
    | PATHPULSEPERCENT
    ;


valuetype
    : ABSOLUTE  { $$ = DELAY_ABSOLUTE;  }
    | INCREMENT { $$ = DELAY_INCREMENT; }
    ;


del_path
    : value
    | input_output_path value
    | value value
    | input_output_path value value
    ;


del_defs
    : del_defs del_def {
        $$ = $1;
        $1->next = $2;
        $2->head = $1->head;
    }
    | del_def {
        $$ = $1;
        $$->head = $$;
        $$->next = NULL;
    }
    ;


input_output_path
    : port_instance port_instance { strcpy($$.in, $1);
                                    strcpy($$.out, $2); }
    ;


del_def
    : del_def_iopath {
        $$ = $1;
    }
    | '(' del_cond del_def_iopath ')' {
        $$ = $3;
    }
    | '(' cond_else_type del_def_iopath ')' {
        $$ = $3;
    }
    | '(' PORT port_instance delval_list ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::PORT_DELAY;
        strcpy($$->p1, $3);
        $$->vlist = $4;
    }
    | '(' INTERCONNECT port_instance port_instance delval_list ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::INTER_DELAY;
        strcpy($$->p1, $3);
        strcpy($$->p2, $4);
        $$->vlist = $5;
    }
    | '(' DEVICE delval_list ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::DEVICE_DELAY;
        $$->p1[0] = '\0';
        $$->vlist = $3;
    }
    | '(' DEVICE port_instance delval_list ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::DEVICE_DELAY;
        strcpy($$->p1, $3);
        $$->vlist = $4;
    }
    ;


del_cond
    : COND expression
    | COND QSTRING expression
    ;


del_def_iopath
    : '(' IOPATH port_spec port_instance delval_list ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::IO_DELAY;
        $$->pspec = $3;
        strcpy($$->p1, $4);
        $$->vlist = $5;
    }
    | '(' IOPATH port_spec port_instance '(' RETAIN delval_list ')' ')' {
        $$ = new SdfDelayDef;
        $$->type = SdfDelayDef::IO_RETAIN;
        $$->pspec = $3;
        strcpy($$->p1, $4);
        $$->vlist = $7;
    }
    ;


cond_else_type
    : CONDELSE
    ;


tchk_def
    : '(' SETUP port_tchk port_tchk value ')'
    | '(' HOLD port_tchk port_tchk value ')'
    | '(' SETUPHOLD port_tchk port_tchk value value ')'
    | '(' SETUPHOLD port_tchk port_tchk value value scond ')'
    | '(' SETUPHOLD port_tchk port_tchk value value ccond ')'
    | '(' SETUPHOLD port_tchk port_tchk value value scond ccond ')'
    | '(' RECOVERY port_tchk port_tchk value ')'
    | '(' REMOVAL port_tchk port_tchk value ')'
    | '(' RECREM port_tchk port_tchk value value ')'
    | '(' RECREM port_tchk port_tchk value value scond ')'
    | '(' RECREM port_tchk port_tchk value value ccond ')'
    | '(' RECREM port_tchk port_tchk value value scond ccond ')'
    | '(' SKEW port_tchk port_tchk value ')'
    | '(' WIDTH port_tchk value ')'
    | '(' PERIOD port_tchk value ')'
    | '(' NOCHANGE port_tchk port_tchk value value ')'
    ;


port_tchk
    : port_spec
    | '(' COND timing_check_condition port_spec ')'
    | '(' COND QSTRING timing_check_condition port_spec ')'
    ;


scond
    : '(' SCOND timing_check_condition ')'
    | '(' SCOND QSTRING timing_check_condition ')'
    ;


ccond
    : '(' CCOND timing_check_condition ')'
    | '(' CCOND QSTRING timing_check_condition ')'
    ;

name
    : '(' NAME ')'
    | '(' NAME QSTRING ')'
    ;

exception
    : '(' EXCEPTION cell_instances ')'
    ;


cell_instances
    : cell_instances cell_instance
    | cell_instance
    ;


te_def
    : cns_def
    | tenv_def
    ;


cns_def
    : '(' PATHCONSTRAINT port_instance port_instances value value ')'
    | '(' PATHCONSTRAINT name port_instance port_instances value value ')'
    | '(' PERIODCONSTRAINT port_instance value ')'
    | '(' PERIODCONSTRAINT port_instance value exception ')'
    | '(' SUM constraint_path constraint_path value ')'
    | '(' SUM constraint_path constraint_path value value ')'
    | '(' SUM constraint_path constraint_path constraint_paths value ')'
    | '(' SUM constraint_path constraint_path constraint_paths value value ')'
    | '(' DIFF constraint_path constraint_path value ')'
    | '(' DIFF constraint_path constraint_path value value ')'
    | '(' SKEWCONSTRAINT port_spec value ')'
    ;


port_instances
    : port_instances port_instance
    | port_instance
    ;


constraint_paths
    : constraint_paths constraint_path
    | constraint_path
    ;


tenv_def
    : '(' ARRIVAL port_instance value value value value ')'
    | '(' ARRIVAL port_edge port_instance value value value value ')'
    | '(' DEPARTURE port_instance value value value value ')'
    | '(' DEPARTURE port_edge port_instance value value value value ')'
    | '(' SLACK port_instance value value value value ')'
    | '(' SLACK port_instance value value value value NUMBER ')'
    | '(' WAVEFORM port_instance NUMBER edge_list ')'
    ;


number_q
    : NUMBER
    |
    ;


constraint_path
    : '(' port_instance port_instance ')'
    ;


port_spec
    : port_instance { $$.type = SdfPortSpec::EDGE_NA;
                      strcpy($$.port, $1);            }
    | port_edge     { $$.type = $1.type;
                      strcpy($$.port, $$.port);       }
    ;


port_edge
    : '(' edge_identifier port_instance ')' { $$.type = $2.type;
                                              strcpy($$.port, $3); }
    ;


edge_identifier
    : POSEDGE    { $$.type = SdfPortSpec::EDGE_01;     }
    | NEGEDGE    { $$.type = SdfPortSpec::EDGE_10;     }
    | EDGE       { if (!strcmp("0z", $1))
                       $$.type = SdfPortSpec::EDGE_0Z;
                   else
                       $$.type = SdfPortSpec::EDGE_1Z; }
    | IDENTIFIER { if (!strcmp("z0", $1))
                       $$.type = SdfPortSpec::EDGE_Z0;
                   else
                       $$.type = SdfPortSpec::EDGE_Z1; }
    | NUMBER     { if ($1 < 10.0)
                       $$.type = SdfPortSpec::EDGE_01;
                   else
                       $$.type = SdfPortSpec::EDGE_10; }
    ;


port_instance
    : port { strcpy($$, $1); }
    ;

port
    : scalar_port { strcpy($$, $1); }
    | bus_port    { strcpy($$, $1); }
    ;


scalar_port
    : IDENTIFIER                { strcpy($$, $1);                          }
    | PATH                      { strcpy($$, $1);                          }
    | IDENTIFIER '[' NUMBER ']' { strcpy($$, $1);
                                  strcat($$, "[");
                                  sprintf(&$$[strlen($$)], "%d", (int)$3);
                                  strcat($$, "]");                         }
    | PATH '[' NUMBER ']'       { strcpy($$, $1);
                                  strcat($$, "[");
                                  sprintf(&$$[strlen($$)], "%d", (int)$3);
                                  strcat($$, "]");                         }
    ;


bus_port
    : IDENTIFIER '[' NUMBER ':' NUMBER ']' {
        strcpy($$, $1);
        strcat($$, "[");
        sprintf(&$$[strlen($$)], "%d", (int)$3);
        strcat($$, ":");
        sprintf(&$$[strlen($$)], "%d", (int)$5);
        strcat($$, "]");
    }
    | PATH '[' NUMBER ':' NUMBER ']' {
        strcpy($$, $1);
        strcat($$, "[");
        sprintf(&$$[strlen($$)], "%d", (int)$3);
        strcat($$, ":");
        sprintf(&$$[strlen($$)], "%d", (int)$5);
        strcat($$, "]");
    }
    ;


edge_list
    : pos_pairs
    | neg_pairs
    ;


pos_pairs
    : pos_pairs pos_pair
    | pos_pair
    ;


neg_pairs
    : neg_pairs neg_pair
    | neg_pair
    ;


pos_pair
    : '(' POSEDGE NUMBER number_q ')' '(' NEGEDGE NUMBER number_q ')'
    ;


neg_pair
    : '(' NEGEDGE NUMBER number_q ')' '(' POSEDGE NUMBER number_q ')'
    ;


value
    : '(' NUMBER ')' { $$.n = 1;
                       $$.v[0] = $2;      }
    | '(' triple ')' { $$.n = $2.n;
                       $$.v[0] = $2.v[0];
                       $$.v[1] = $2.v[1];
                       $$.v[2] = $2.v[2]; }
    | '(' ')'        { $$.n = 0;          }
    ;


triple
    : NUMBER ':' ':'               { $$.n = 3;
                                     $$.v[0] = $1;
                                     $$.v[1] = 0;
                                     $$.v[2] = 0;  }
    | NUMBER ':' NUMBER ':'        { $$.n = 3;
                                     $$.v[0] = $1;
                                     $$.v[1] = $3;
                                     $$.v[2] = 0;  }
    | NUMBER ':' ':' NUMBER        { $$.n = 3;
                                     $$.v[0] = $1;
                                     $$.v[1] = 0;
                                     $$.v[2] = $4; }
    | ':' NUMBER ':'               { $$.n = 3;
                                     $$.v[0] = 0;
                                     $$.v[1] = $2;
                                     $$.v[2] = 0;  }
    | ':' NUMBER ':' NUMBER        { $$.n = 3;
                                     $$.v[0] = 0;
                                     $$.v[1] = $2;
                                     $$.v[2] = $4; }
    | ':' ':' NUMBER               { $$.n = 3;
                                     $$.v[0] = 0;
                                     $$.v[1] = 0;
                                     $$.v[2] = $3; }
    | NUMBER ':' NUMBER ':' NUMBER { $$.n = 3;
                                     $$.v[0] = $1;
                                     $$.v[1] = $3;
                                     $$.v[2] = $5; }
    ;


delval
    : value                     { $$.n = 1;
                                  sdfValueCopy($$.v[0], $1); }
    | '(' value value ')'       { $$.n = 2;
                                  sdfValueCopy($$.v[0], $2);
                                  sdfValueCopy($$.v[1], $3); }
    | '(' value value value ')' { $$.n = 3;
                                  sdfValueCopy($$.v[0], $2);
                                  sdfValueCopy($$.v[0], $3);
                                  sdfValueCopy($$.v[1], $4); }
    ;


delval_list
    : delval {
        $$.n = 1;
        sdfDelayValueCopy($$.v[0], $1);
    }
    | delval delval {
        $$.n = 2;
        sdfDelayValueCopy($$.v[0], $1);
        sdfDelayValueCopy($$.v[1], $1);
    }
    | delval delval delval {
        $$.n = 3;
        sdfDelayValueCopy($$.v[0], $1);
        sdfDelayValueCopy($$.v[1], $2);
        sdfDelayValueCopy($$.v[2], $3);
    }
    | delval delval delval delval delval delval {
        $$.n = 6;
        sdfDelayValueCopy($$.v[0], $1);
        sdfDelayValueCopy($$.v[1], $2);
        sdfDelayValueCopy($$.v[2], $3);
        sdfDelayValueCopy($$.v[3], $4);
        sdfDelayValueCopy($$.v[4], $5);
        sdfDelayValueCopy($$.v[5], $6);
    }
    | delval delval delval delval delval delval delval delval delval delval delval delval {
        $$.n = 12;
        sdfDelayValueCopy($$.v[0], $1);
        sdfDelayValueCopy($$.v[1], $2);
        sdfDelayValueCopy($$.v[2], $3);
        sdfDelayValueCopy($$.v[3], $4);
        sdfDelayValueCopy($$.v[4], $5);
        sdfDelayValueCopy($$.v[5], $6);
        sdfDelayValueCopy($$.v[6], $7);
        sdfDelayValueCopy($$.v[7], $8);
        sdfDelayValueCopy($$.v[8], $9);
        sdfDelayValueCopy($$.v[9], $10);
        sdfDelayValueCopy($$.v[10], $11);
        sdfDelayValueCopy($$.v[11], $12);
    }
    ;


expression
    : conditional_expression
    | '{' conditional_expression '}'
    | '{' expression ',' conditional_expression '}'
    ;


conditional_expression
    : simple_expression
    | simple_expression '?' expression ':' conditional_expression
    ;


simple_expression
    : '(' simple_expression ')'
    | unary_op '(' simple_expression ')'
    | port
    | unary_op port
    | scalar_constant
    | unary_op scalar_constant
    | binary_expression
    ;


binary_expression
    : simple_expression '+' simple_expression
    | simple_expression '-' simple_expression
    | simple_expression '*' simple_expression
    | simple_expression '/' simple_expression
    | simple_expression '%' simple_expression
    | simple_expression EQU simple_expression
    | simple_expression INEQU simple_expression
    | simple_expression CASE_EQU simple_expression
    | simple_expression CASE_INEQU simple_expression
    | simple_expression L_AND simple_expression
    | simple_expression L_OR simple_expression
    | simple_expression '<' simple_expression
    | simple_expression LESS_OR_EQU simple_expression
    | simple_expression '>' simple_expression
    | simple_expression GREATER_OR_EQU simple_expression
    | simple_expression '&' simple_expression
    | simple_expression '^' simple_expression
    | simple_expression '|' simple_expression
    | simple_expression RIGHT_SHIFT simple_expression
    | simple_expression LEFT_SHIFT simple_expression
    ;


timing_check_condition
    : scalar_node
    | inv_op scalar_node
    | scalar_node equ_op scalar_constant
    ;


scalar_node
    : scalar_port
    ;


scalar_constant
    : BIT_CONST
    | NUMBER
    ;


unary_op
    : '+'
    | '-'
    | '!'
    | '~'
    | '&'
    | U_NAND
    | '|'
    | U_NOR
    | '^'
    | U_XNOR
    | U_XNOR_ALT
    ;


inv_op
    : '!'
    | '~'
    ;


equ_op
    : EQU
    | INEQU
    | CASE_EQU
    | CASE_INEQU
    ;


%%

void sdf_fileerror(void *a, char const *msg) {
    fprintf(stderr, "**ERROR sdf_fileerror(): ");
    fprintf(stderr, "At line %d. Near `%s'. ", sdf_fileline, sdf_filetext);
    fprintf(stderr, "%s\n", msg);
}


