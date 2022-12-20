%{
// **************************************************************************
// File       [ vlog_file.l ]
// Author     [ littleshamoo ]
// Synopsis   [ lex for reading verilog file ]
// Date       [ Ver. 1.0 started 2010/07/07 ]
// **************************************************************************

#include "vlog_file.h"
#include "vlog_file.tab.hpp"

// extern char *vlog_filetext;
// extern int  vlog_fileline;
bool vlog_filewarning = 1;
int vlog_fileline = 1;

void vlog_fileskipSpecify();
void vlog_fileskipTable();
void vlog_fileskipTimeScale();
void vlog_fileskipCellDef();
void vlog_fileskipEndCellDef();

%}

D        [0-9]
NUMBER   {D}{D}*
L        [a-zA-Z_]
LTRAIL   [0-9a-zA-Z_./]
ESC      [~`!#$%^&*()+=|{}:;"',<>?/.\-\[\]]
VALUE    {NUMBER}'b(0|1|X|x|Z|z)
NAME     ({L}({LTRAIL}|("\\"{ESC}))*("["{NUMBER}(":"{NUMBER})?"]")?)|{VALUE}

%option nounput noyywrap
%x COMMENT
%x SPECIFY
%x TABLE

%%


"//".*                              ;
"/*" BEGIN COMMENT                  ;
<COMMENT>"*/" BEGIN INITIAL         ;
<COMMENT>\n                         { ++vlog_fileline;           }
<COMMENT>.

"specify" BEGIN SPECIFY             ;
<SPECIFY>"endspecify"               { vlog_fileskipSpecify();
                                      BEGIN(INITIAL);            }
<SPECIFY>\n                         { ++vlog_fileline;           }
<SPECIFY>.

"table" BEGIN TABLE                 ;
<TABLE>"endtable"                   { vlog_fileskipTable();
                                      BEGIN(INITIAL);            }
<TABLE>\n                           { ++vlog_fileline;           }
<TABLE>.

"`timescale".*                      { vlog_fileskipTimeScale();  }
"`celldefine".*                     { vlog_fileskipCellDef();    }
"`endcelldefine".*                  { vlog_fileskipEndCellDef(); }

"module"                            { return MODULE;             }
"endmodule"                         { return ENDMODULE;          }
"primitive"                         { return PRIMITIVE;          }
"endprimitive"                      { return ENDPRIMITIVE;       }
"input"                             { return INPUT;              }
"output"                            { return OUTPUT;             }
"inout"                             { return INOUT;              }
"assign"                            { return ASSIGN;             }
"wire"                              { return WIRE;               }
"reg"                               { return REG;                }
"supply0"                           { return SUPPLY_L;           }
"supply1"                           { return SUPPLY_H;           }

"weak0"|"weak1"|"strong0"|"strong1" {
    strcpy(vlog_filelval.ychar, vlog_filetext);
    return STRENGTH;
}

{NUMBER}                            {
    strcpy(vlog_filelval.ychar, vlog_filetext);
    return NUMBER;
}

{NAME}                              {
    strcpy(vlog_filelval.ychar, vlog_filetext);
    return NAME;
}

"["                                 { return vlog_filetext[0];   }
"]"                                 { return vlog_filetext[0];   }
"("                                 { return vlog_filetext[0];   }
")"                                 { return vlog_filetext[0];   }
"."                                 { return vlog_filetext[0];   }
","                                 { return vlog_filetext[0];   }
";"                                 { return vlog_filetext[0];   }
":"                                 { return vlog_filetext[0];   }
"="                                 { return vlog_filetext[0];   }

\n                                  { ++vlog_fileline;           }
[ \t\v\f]                           ;
.                                   ;

%%

void vlog_fileskipSpecify() { }

void vlog_fileskipTable() { }

void vlog_fileskipTimeScale() { }

void vlog_fileskipCellDef() { }

void vlog_fileskipEndCellDef() { }