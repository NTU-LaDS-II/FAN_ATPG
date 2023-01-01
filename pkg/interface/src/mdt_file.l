%{
// **************************************************************************
// File       [ mdt_file.l ]
// Author     [ littleshamoo ]
// Synopsis   [ lex for reading mentor mdt library file ]
// Date       [ 2011/02/23 created ]
// **************************************************************************

#include "mdt_file.h"
#include "mdt_file.tab.hpp"


int  mdt_fileline    = 1;
int mdt_filewarning = 1;
int  mdt_fileparen   = 0;

// skip scan definition
void mdt_fileskipScanDef();

// skip attributes
void mdt_fileskipCellType();
void mdt_fileskipUsed();
void mdt_fileskipClock();
void mdt_fileskipNoFault();
void mdt_fileskipFunction();
void mdt_fileskipFault();
void mdt_fileskipSetClk();
void mdt_fileskipResetClk();
void mdt_fileskipBusKeeper();
void mdt_fileskipArrayDel();
void mdt_fileskipArray();
void mdt_fileskipPulse();
void mdt_fileskipPinNoFault();

// skip memory elements
void mdt_fileskipDataSize();
void mdt_fileskipAddrSize();
void mdt_fileskipRom();
void mdt_fileskipRam();
void mdt_fileskipCram();
void mdt_fileskipMinAddr();
void mdt_fileskipMaxAddr();
void mdt_fileskipReadOff();
void mdt_fileskipInitFile();
void mdt_fileskipEdgeTrigger();
void mdt_fileskipAddrType();
void mdt_fileskipRrConflict();
void mdt_fileskipRwConflict();
void mdt_fileskipWriteContention();
void mdt_fileskipOverwrite();

// skip complex file supports
void mdt_fileskipMacro();
void mdt_fileskipAlias();
void mdt_fileskipInclude();

%}

D   [0-9]
L   [a-zA-Z_]
WHT [ \t\v\f\n]

%option nounput noyywrap
%x SCAN_DEF
%x CELL_TYPE USED CLOCK NO_FAULT FUNCTION FAULT SET_CLK RESET_CLK BUS_KEEPER
%x ARRAY_DEL ARRAY PULSE
%x DATA_SIZE ADDR_SIZE ROM RAM CRAM MIN_ADDR MAX_ADDR READ_OFF INIT_FILE
%x EDGE_TRIGGER ADDR_TYPE RR_CONFLICT RW_CONFLICT WRT_CONTENTION OVERWRITE
%x MACRO ALIAS INCLUDE


%%

"//".*                                         ;

"scan_definition"{WHT}*"("                     { ++mdt_fileparen;
                                                 BEGIN SCAN_DEF;          }
<SCAN_DEF>")"                                  { --mdt_fileparen;
                                                 if (mdt_fileparen == 0) {
                                                     mdt_fileskipScanDef();
                                                     BEGIN(INITIAL);
                                                 }
                                               }
<SCAN_DEF>"("                                  { ++mdt_fileparen;         }
<SCAN_DEF>\n                                   { ++mdt_fileline;          }
<SCAN_DEF>.


"cell_type"{WHT}*"=" BEGIN CELL_TYPE           ;
<CELL_TYPE>";"                                 { mdt_fileskipCellType();
                                                 BEGIN(INITIAL);          }
<CELL_TYPE>\n                                  { ++mdt_fileline;          }
<CELL_TYPE>.


"used"{WHT}*"=" BEGIN USED                     ;
<USED>";"                                      { mdt_fileskipUsed();
                                                 BEGIN(INITIAL);          }
<USED>\n                                       { ++mdt_fileline;          }
<USED>.


"clock"{WHT}*"=" BEGIN CLOCK                   ;
<CLOCK>";"                                     { mdt_fileskipClock();
                                                 BEGIN(INITIAL);          }
<CLOCK>\n                                      { ++mdt_fileline;          }
<CLOCK>.


"no_fault"{WHT}*"=" BEGIN NO_FAULT             ;
<NO_FAULT>";"                                  { mdt_fileskipNoFault();
                                                 BEGIN(INITIAL);          }
<NO_FAULT>\n                                   { ++mdt_fileline;          }
<NO_FAULT>.


"function"{WHT}*"=" BEGIN FUNCTION             ;
<FUNCTION>";"                                  { mdt_fileskipFunction();
                                                 BEGIN(INITIAL);          }
<FUNCTION>\n                                   { ++mdt_fileline;          }
<FUNCTION>.


"fault"{WHT}*"=" BEGIN FAULT                   ;
<FAULT>";"                                     { mdt_fileskipFault();
                                                 BEGIN(INITIAL);          }
<FAULT>\n                                      { ++mdt_fileline;          }
<FAULT>.


"set_clock_conflict"{WHT}*"=" BEGIN SET_CLK    ;
<SET_CLK>";"                                   { mdt_fileskipSetClk();
                                                 BEGIN(INITIAL);          }
<SET_CLK>\n                                    { ++mdt_fileline;          }
<SET_CLK>.


"reset_clock_conflict"{WHT}*"=" BEGIN RESET_CLK;
<RESET_CLK>";"                                 { mdt_fileskipResetClk();
                                                 BEGIN(INITIAL);          }
<RESET_CLK>\n                                  { ++mdt_fileline;          }
<RESET_CLK>.


"bus_keeper"{WHT}*"=" BEGIN BUS_KEEPER         ;
<BUS_KEEPER>";"                                { mdt_fileskipBusKeeper();
                                                 BEGIN(INITIAL);          }
<BUS_KEEPER>\n                                 { ++mdt_fileline;          }
<BUS_KEEPER>.


"array_delimiter"{WHT}*"=" BEGIN ARRAY_DEL     ;
<ARRAY_DEL>";"                                 { mdt_fileskipArrayDel();
                                                 BEGIN(INITIAL);          }
<ARRAY_DEL>\n                                  { ++mdt_fileline;          }
<ARRAY_DEL>.


"array"{WHT}*"=" BEGIN ARRAY                   ;
<ARRAY>";"                                     { mdt_fileskipArray();
                                                 BEGIN(INITIAL);          }
<ARRAY>\n                                      { ++mdt_fileline;          }
<ARRAY>.


"primitive"{WHT}*"="{WHT}*"_pulse_generator"   BEGIN PULSE;
<PULSE>";"                                     { mdt_fileskipPulse();
                                                 BEGIN(INITIAL);          }
<PULSE>\n                                      { ++mdt_fileline;          }
<PULSE>.


"primitive"{WHT}*"="{WHT}*"_rom" BEGIN ROM     ;
<ROM>";"                                       { mdt_fileskipRom();
                                                 BEGIN(INITIAL);          }
<ROM>\n                                        { ++mdt_fileline;          }
<ROM>.


"primitive"{WHT}*"="{WHT}*"_ram" BEGIN RAM     ;
<RAM>";"                                       { mdt_fileskipRam();
                                                 BEGIN(INITIAL);          }
<RAM>\n                                        { ++mdt_fileline;          }
<RAM>.


"primitive"{WHT}*"="{WHT}*"_cram" BEGIN CRAM   ;
<CRAM>";"                                      { mdt_fileskipCram();
                                                 BEGIN(INITIAL);          }
<CRAM>\n                                       { ++mdt_fileline;          }
<CRAM>.


"data_size"{WHT}*"=" BEGIN DATA_SIZE           ;
<DATA_SIZE>";"                                 { mdt_fileskipDataSize();
                                                 BEGIN(INITIAL);          }
<DATA_SIZE>\n                                  { ++mdt_fileline;          }
<DATA_SIZE>.


"address_size"{WHT}*"=" BEGIN ADDR_SIZE        ;
<ADDR_SIZE>";"                                 { mdt_fileskipAddrSize();
                                                 BEGIN(INITIAL);          }
<ADDR_SIZE>\n                                  { ++mdt_fileline;          }
<ADDR_SIZE>.


"min_address"{WHT}*"=" BEGIN MIN_ADDR          ;
<MIN_ADDR>";"                                  { mdt_fileskipMinAddr();
                                                 BEGIN(INITIAL);          }
<MIN_ADDR>\n                                   { ++mdt_fileline;          }
<MIN_ADDR>.


"max_address"{WHT}*"=" BEGIN MAX_ADDR          ;
<MAX_ADDR>";"                                  { mdt_fileskipMaxAddr();
                                                 BEGIN(INITIAL);          }
<MAX_ADDR>\n                                   { ++mdt_fileline;          }
<MAX_ADDR>.


"read_off"{WHT}*"=" BEGIN READ_OFF             ;
<READ_OFF>";"                                  { mdt_fileskipReadOff();
                                                 BEGIN(INITIAL);          }
<READ_OFF>\n                                   { ++mdt_fileline;          }
<READ_OFF>.


"init_file"{WHT}*"=" BEGIN INIT_FILE           ;
<INIT_FILE>";"                                 { mdt_fileskipInitFile();
                                                 BEGIN(INITIAL);          }
<INIT_FILE>\n                                  { ++mdt_fileline;          }
<INIT_FILE>.


"edge_trigger"{WHT}*"=" BEGIN EDGE_TRIGGER     ;
<EDGE_TRIGGER>";"                              { mdt_fileskipEdgeTrigger();
                                                 BEGIN(INITIAL);          }
<EDGE_TRIGGER>\n                               { ++mdt_fileline;          }
<EDGE_TRIGGER>.


"address_type"{WHT}*"=" BEGIN ADDR_TYPE        ;
<ADDR_TYPE>";"                                 { mdt_fileskipAddrType();
                                                 BEGIN(INITIAL);          }
<ADDR_TYPE>\n                                  { ++mdt_fileline;          }
<ADDR_TYPE>.


"read_read_conflict"{WHT}*"=" BEGIN RR_CONFLICT;
<RR_CONFLICT>";"                               { mdt_fileskipRrConflict();
                                                 BEGIN(INITIAL);          }
<RR_CONFLICT>\n                                { ++mdt_fileline;          }
<RR_CONFLICT>.


"read_write_conflict"{WHT}*"="                 BEGIN RW_CONFLICT;
<RW_CONFLICT>";"                               { mdt_fileskipRwConflict();
                                                 BEGIN(INITIAL);          }
<RW_CONFLICT>\n                                { ++mdt_fileline;          }
<RW_CONFLICT>.


"write_contention"{WHT}*"="                    BEGIN WRT_CONTENTION;
<WRT_CONTENTION>";"                            { mdt_fileskipWriteContention();
                                                 BEGIN(INITIAL);          }
<WRT_CONTENTION>\n                             { ++mdt_fileline;          }
<WRT_CONTENTION>.


"overwrite"{WHT}*"=" BEGIN OVERWRITE;
<OVERWRITE>";"                                 { mdt_fileskipOverwrite();
                                                 BEGIN(INITIAL);          }
<OVERWRITE>\n                                  { ++mdt_fileline;          }
<OVERWRITE>.


"macro"{WHT} BEGIN MACRO                       ;
<MACRO>")"                                     { --mdt_fileparen;
                                                 if (mdt_fileparen == 0) {
                                                     mdt_fileskipMacro();
                                                     BEGIN(INITIAL);
                                                 }
                                               }
<MACRO>"("                                     { ++mdt_fileparen;         }
<MACRO>\n                                      { ++mdt_fileline;          }
<MACRO>.


"alias"{WHT} BEGIN ALIAS                       ;
<ALIAS>\n                                      { ++mdt_fileline;
                                                 mdt_fileskipAlias();
                                                 BEGIN(INITIAL);          }
<ALIAS>.


"#include"{WHT} BEGIN INCLUDE                  ;
<INCLUDE>\n                                    { ++mdt_fileline;
                                                 mdt_fileskipInclude();
                                                 BEGIN(INITIAL);          }
<INCLUDE>.


"model"                                        { return MODEL;            }
"model_source"                                 { return MODEL_SOURCE;     }
"input"                                        { return INPUT;            }
"output"                                       { return OUTPUT;           }
"inout"                                        { return INOUT;            }
"intern"                                       { return INTERN;           }
"primitive"                                    { return PRIMITIVE;        }
"instance"                                     { return INSTANCE;         }

{L}({L}|{D})*(":nf0"|":nf1"|":nf")             {
    char *pch = strrchr(mdt_filetext, ':');
    int len = pch - mdt_filetext;
    strncpy(mdt_filelval.ychar, mdt_filetext, len);
    mdt_filelval.ychar[len + 1] = '\0';
    mdt_fileskipPinNoFault();
    return NAME;
}

{L}({L}|{D})*                                  {
    strcpy(mdt_filelval.ychar, mdt_filetext);
    return NAME;
}

"("                                            { return mdt_filetext[0]; }
")"                                            { return mdt_filetext[0]; }
"="                                            { return mdt_filetext[0]; }
","                                            { return mdt_filetext[0]; }
";"                                            { return mdt_filetext[0]; }

\n                                             { ++mdt_fileline;       }
[ \t\v\f]                                      ;
.                                              ;

%%

void mdt_fileskipScanDef() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `scan_definition' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipCellType() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `cell_type' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipUsed() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `used' attribute ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipClock() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `clock' attribute ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipNoFault() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `no_fault' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipFunction() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `function' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipFault() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `fault' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipSetClk() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `set_clock_conflict' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipResetClk() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `reset_clock_conflict' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipBusKeeper() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `bus_keeper' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipArrayDel() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `array_delimiter' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipArray() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `array' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipPulse() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `_pulse_generator' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipPinNoFault() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `pin no fault' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipDataSize() {
    if (mdt_filewarning) {
        fprintf(stderr,  "**WARN mdt_filelex(): `data_size' ");
        fprintf(stderr,  "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipAddrSize() {
    if (mdt_filewarning) {
        fprintf(stderr,  "**WARN mdt_filelex(): `address_size' ");
        fprintf(stderr,  "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipRom() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `ROM' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipRam() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `RAM' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipCram() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `CRAM' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipMinAddr() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `min_address' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipMaxAddr() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `max_address' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipReadOff() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `read_off' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipInitFile() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `init_file' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipEdgeTrigger() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `edge_trigger' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipAddrType() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `address_type' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipRrConflict() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `read_read_conflict' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipRwConflict() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `read_write_conflict' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipWriteContention() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `write_contention' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipOverwrite() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `overwrite' ");
        fprintf(stderr, "attribute skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipMacro() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `macro' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipAlias() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `alias' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

void mdt_fileskipInclude() {
    if (mdt_filewarning) {
        fprintf(stderr, "**WARN mdt_filelex(): `include' ");
        fprintf(stderr, "skipped near line %d\n", mdt_fileline);
    }
}

