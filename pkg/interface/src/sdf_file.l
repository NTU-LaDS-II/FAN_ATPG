%{
// **************************************************************************
// File       [ sdf_file.l ]
// Author     [ littleshamoo ]
// Synopsis   [ Lex for reading SDF file 3.0 ]
// Date       [ 2010/11/18 created ]
// **************************************************************************

#include "sdf_file.h"
#include "sdf_file.tab.hpp"

int sdf_fileline = 1;

%}

EDGE           0z|1z
BIT_CONST      (1'b0)|(1'b1)|(1'B0)|(1'B1)|('b0)|('b1)|('B0)|('B1)
D              [0-9]
E              [Ee][+-]?{D}+
L              [a-zA-Z_]
ESC_CHAR       [~`!#$%^&*()+=|{}:;"',<>?/.^\-\[\]]
ID             {L}({L}|{D}|("\\"{ESC_CHAR}))*
PATH           {ID}([./]{ID})+

%option nounput noyywrap
%x COMMENT

%%

"//".*                      ;
"/*" BEGIN COMMENT          ;
<COMMENT>"*/" BEGIN INITIAL ;
<COMMENT>\n                 { ++sdf_fileline;          }
<COMMENT>.

"DELAYFILE"                 { return DELAYFILE;        }
"SDFVERSION"                { return SDFVERSION;       }
"DESIGN"                    { return DESIGN;           }
"DATE"                      { return DATE;             }
"VENDOR"                    { return VENDOR;           }
"PROGRAM"                   { return PROGRAM;          }
"VERSION"                   { return VERSION;          }
"DIVIDER"                   { return DIVIDER;          }
"VOLTAGE"                   { return VOLTAGE;          }
"PROCESS"                   { return PROCESS;          }
"TEMPERATURE"               { return TEMPERATURE;      }
"TIMESCALE"                 { return TIMESCALE;        }
"CELL"                      { return CELL;             }
"CELLTYPE"                  { return CELLTYPE;         }
"INSTANCE"                  { return INSTANCE;         }
"DELAY"                     { return DELAY;            }
"TIMINGCHECK"               { return TIMINGCHECK;      }
"TIMINGENV"                 { return TIMINGENV;        }
"PATHPULSE"                 { return PATHPULSE;        }
"PATHPULSEPERCENT"          { return PATHPULSEPERCENT; }
"ABSOLUTE"                  { return ABSOLUTE;         }
"INCREMENT"                 { return INCREMENT;        }
"IOPATH"                    { return IOPATH;           }
"RETAIN"                    { return RETAIN;           }
"COND"                      { return COND;             }
"SCOND"                     { return SCOND;            }
"CCOND"                     { return CCOND;            }
"CONDELSE"                  { return CONDELSE;         }
"PORT"                      { return PORT;             }
"INTERCONNECT"              { return INTERCONNECT;     }
"DEVICE"                    { return DEVICE;           }
"SETUP"                     { return SETUP;            }
"HOLD"                      { return HOLD;             }
"SETUPHOLD"                 { return SETUPHOLD;        }
"RECOVERY"                  { return RECOVERY;         }
"REMOVAL"                   { return REMOVAL;          }
"RECREM"                    { return RECREM;           }
"SKEW"                      { return SKEW;             }
"WIDTH"                     { return WIDTH;            }
"PERIOD"                    { return PERIOD;           }
"NOCHANGE"                  { return NOCHANGE;         }
"NAME"                      { return NAME;             }
"EXCEPTION"                 { return EXCEPTION;        }
"PATHCONSTRAINT"            { return PATHCONSTRAINT;   }
"PERIODCONSTRAINT"          { return PERIODCONSTRAINT; }
"SUM"                       { return SUM;              }
"DIFF"                      { return DIFF;             }
"SKEWCONSTRAINT"            { return SKEWCONSTRAINT;   }
"ARRIVAL"                   { return ARRIVAL;          }
"DEPARTURE"                 { return DEPARTURE;        }
"SLACK"                     { return SLACK;            }
"WAVEFORM"                  { return WAVEFORM;         }
"posedge"                   { return POSEDGE;          }
"negedge"                   { return NEGEDGE;          }

\"(\\.|[^\\"])*\"           {
    strcpy(sdf_filelval.ychar, sdf_filetext);
    return QSTRING;
}

{PATH}                      {
    strcpy(sdf_filelval.ychar, sdf_filetext);
    return PATH;
}

{BIT_CONST}                 {
    strcpy(sdf_filelval.ychar, sdf_filetext);
    return BIT_CONST;
}

{EDGE}                      {
    strcpy(sdf_filelval.ychar, sdf_filetext);
    return EDGE;
}

{ID}                        {
    strcpy(sdf_filelval.ychar, sdf_filetext);
    return IDENTIFIER;
}

[+\-]?{D}+                   {
    sdf_filelval.yfloat = atof(sdf_filetext);
    return NUMBER;
}

[+\-]?{D}+"."{D}*({E})?      {
    sdf_filelval.yfloat = atof(sdf_filetext);
    return NUMBER;
}

[+\-]?{D}*"."{D}+({E})?      {
    sdf_filelval.yfloat = atof(sdf_filetext);
    return NUMBER;
}

"==="                        { return CASE_EQU;        }
"!=="                        { return CASE_INEQU;      }
"=="                         { return EQU;             }
"!="                         { return INEQU;           }
"&&"                         { return L_AND;           }
"||"                         { return L_OR;            }
"<="                         { return LESS_OR_EQU;     }
">="                         { return GREATER_OR_EQU;  }
">>"                         { return RIGHT_SHIFT;     }
"<<"                         { return LEFT_SHIFT;      }
"~&"                         { return U_NAND;          }
"~|"                         { return U_NOR;           }
"~^"                         { return U_XNOR;          }
"^~"                         { return U_XNOR_ALT;      }

"%"                          { return sdf_filetext[0]; }
"<"                          { return sdf_filetext[0]; }
">"                          { return sdf_filetext[0]; }
"."                          { return sdf_filetext[0]; }
"/"                          { return sdf_filetext[0]; }
"!"                          { return sdf_filetext[0]; }
"~"                          { return sdf_filetext[0]; }
"+"                          { return sdf_filetext[0]; }
"-"                          { return sdf_filetext[0]; }
"|"                          { return sdf_filetext[0]; }
"&"                          { return sdf_filetext[0]; }
"^"                          { return sdf_filetext[0]; }
"*"                          { return sdf_filetext[0]; }
"("                          { return sdf_filetext[0]; }
")"                          { return sdf_filetext[0]; }
"{"                          { return sdf_filetext[0]; }
"}"                          { return sdf_filetext[0]; }
"?"                          { return sdf_filetext[0]; }
":"                          { return sdf_filetext[0]; }
","                          { return sdf_filetext[0]; }

\n                           { ++sdf_fileline;         }
[ \t\v\f]                    ;
.                            ;

%%

