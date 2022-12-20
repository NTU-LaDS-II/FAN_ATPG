// **************************************************************************
// File       [ vt100.h ]
// Author     [ littleshamoo ]
// Synopsis   [ VT100 configuration keys.
//              References:
//                1. http://ascii-table.com/ansi-escape-sequences-vt-100.php
//                2. http://www.termsys.demon.co.uk/vtansi.htm
//            ]
// Date       [ Ver. 1.0 started 2010/01/29 ]
// **************************************************************************


#ifndef _COMMON_VT100_H_
#define _COMMON_VT100_H_

namespace CommonNs {

// ANSI colors
const char *const ANSI_RESET  = "\033[0m";
const char *const ANSI_BLACK  = "\033[30m";
const char *const ANSI_RED    = "\033[31m";
const char *const ANSI_GREEN  = "\033[32m";
const char *const ANSI_YELLOW = "\033[33m";
const char *const ANSI_BLUE   = "\033[34m";
const char *const ANSI_PURPLE = "\033[35m";
const char *const ANSI_CYAN   = "\033[36m";
const char *const ANSI_WHITE  = "\033[37m";

// setting and resetting terminal modes
const char *const VT100_LM_NL    = "\033[20h"; // set new line mode
const char *const VT100_CKM_APP  = "\033[?1h"; // set cursor key to app
                                               // set ANSI
const char *const VT100_COLM_132 = "\033[?3h"; // set column to 132
const char *const VT100_SCLM_SM  = "\033[?4h"; // set smooth scrolling
const char *const VT100_SCNM_REV = "\033[?5h"; // set screen reverse
const char *const VT100_OM_REL   = "\033[?6h"; // set origin to relative
const char *const VT100_AWM_ON   = "\033[?7h"; // set auto-wrap mode
const char *const VT100_ARM_ON   = "\033[?8h"; // set auto-repeat mode
const char *const VT100_INLM_ON  = "\033[?9h"; // set interlacing mode

const char *const VT100_LM_LF    = "\033[20h"; // set line feed mode
const char *const VT100_CKM_CSR  = "\033[?1h"; // set cursor key to cursor
const char *const VT100_VT52M    = "\033[?1h"; // set VT52
const char *const VT100_COLM_80  = "\033[?3h"; // set column to 80
const char *const VT100_SCLM_JP  = "\033[?4h"; // set jump scrolling
const char *const VT100_SCNM_NOR = "\033[?5h"; // set screen normal
const char *const VT100_OM_ABS   = "\033[?6h"; // set origin to absolute
const char *const VT100_AWM_OFF  = "\033[?7h"; // unset auto-wrap mode
const char *const VT100_ARM_OFF  = "\033[?8h"; // unset auto-repeat mode
const char *const VT100_INLM_OFF = "\033[?9h"; // unset interlacing mode

// corsor control
const char *const VT100_CSRU     = "\033[1A";  // move cursor up 1 row
const char *const VT100_CSRD     = "\033[1B";  // move cursor down 1 row
const char *const VT100_CSRF     = "\033[1C";  // move cursor right 1 col
const char *const VT100_CSRB     = "\033[1D";  // move cursor back 1 col
const char *const VT100_CSRL     = "\033E";    // move to next line
const char *const VT100_CSRS     = "\0337";    // save cursor position
const char *const VT100_CSRR     = "\0338";    // restore cursor position

// scrolling
const char *const VT100_SCRU     = "\033D";    // scroll window up 1 line
const char *const VT100_SCRD     = "\033M";    // scroll window dn 1 line
const char *const VT100_SCRE     = "\033[r";   // scroll entire window

// tab control
const char *const VT100_TABSET   = "\033H";    // set tab
const char *const VT100_TABCLR   = "\033[g";   // clear tab
const char *const VT100_TABCLRA  = "\033[3g";  // clear all tab

// erasing text
const char *const VT100_ERSEOL   = "\033[K";   // erase end of line
const char *const VT100_ERSSOL   = "\033[1K";  // erase start of line
const char *const VT100_ERSL     = "\033[2K";  // erase entire line
const char *const VT100_ERSD     = "\033[J";   // erase down
const char *const VT100_ERSU     = "\033[1J";  // erase up
const char *const VT100_ERSS     = "\033[2J";  // erase screen

};

#endif