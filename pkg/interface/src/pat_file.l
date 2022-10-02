%{
// **************************************************************************
// File       [ pat_file.l ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ ]
// **************************************************************************


#include <cstdlib>

#include "pat_file.h"
#include "pat_file.tab.hpp"

using namespace std;

int pat_fileline = 1;


%}

D      [0-9]
NUMBER {D}{D}*
L      [a-zA-Z_]
LTRAIL [0-9a-zA-Z_./]
ESC    [~`!#$%^&*()+=|{}:;"',<>?/.\-\[\]]
NAME   ({LTRAIL}|("\\"{ESC}))*("["{NUMBER}(":"{NUMBER})?"]")?

%option nounput noyywrap

%%

"BASIC_SCAN" {
    pat_filelval.ypatType = IntfNs::BASIC_SCAN;
    return PATTERN_TYPE;
}

"LAUNCH_ON_CAPTURE" {
    pat_filelval.ypatType = IntfNs::LAUNCH_CAPTURE;
    return PATTERN_TYPE;
}

"LAUNCH_ON_SHIFT" {
    pat_filelval.ypatType = IntfNs::LAUNCH_SHIFT;
    return PATTERN_TYPE;
}

"_num_of_pattern_"{NUMBER} {
    pat_filelval.yint = atoi(&pat_filetext[16]);
    return PATTERN_NUM;
}

"_pattern_"{NUMBER}     {
    return PATTERN;
}

{NAME}                  {
    pat_filelval.ychar = strdup(pat_filetext);
    return NAME;
}

"|"                     { return pat_filetext[0]; }
\n                      { pat_fileline++;         }
[ \t\v\f]               ;
.                       ;

%%

