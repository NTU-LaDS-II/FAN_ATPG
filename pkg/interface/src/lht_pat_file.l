%{
// **************************************************************************
// File       [ lht_pat_file.l ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/10/14 created ]
// **************************************************************************


#include "lht_pat_file.h"
#include "lht_pat_file.tab.hpp"

using namespace std;

int lht_pat_fileline = 1;

%}

D      [0-9]
NUMBER {D}{D}*
L      [01X]
VALUE  {L}+

%option nounput noyywrap

%%

{NUMBER}":" {
    return PATTERN;
}

{VALUE} {
    lht_pat_filelval.ychar = strdup(lht_pat_filetext);
    return VALUE;
}

"_"         { return lht_pat_filetext[0]; }
"|"         { return lht_pat_filetext[0]; }
"@"         { return lht_pat_filetext[0]; }
"-"         { return lht_pat_filetext[0]; }
">"         { return lht_pat_filetext[0]; }
\n          { lht_pat_fileline++;         }
[ \t\v\f]   ;
.           ;

%%

