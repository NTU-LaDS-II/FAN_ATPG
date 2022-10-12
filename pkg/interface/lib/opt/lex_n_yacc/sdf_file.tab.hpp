/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DELAYFILE = 258,
     SDFVERSION = 259,
     DESIGN = 260,
     DATE = 261,
     VENDOR = 262,
     PROGRAM = 263,
     VERSION = 264,
     DIVIDER = 265,
     VOLTAGE = 266,
     PROCESS = 267,
     TEMPERATURE = 268,
     TIMESCALE = 269,
     CELL = 270,
     CELLTYPE = 271,
     INSTANCE = 272,
     DELAY = 273,
     TIMINGCHECK = 274,
     TIMINGENV = 275,
     PATHPULSE = 276,
     PATHPULSEPERCENT = 277,
     ABSOLUTE = 278,
     INCREMENT = 279,
     IOPATH = 280,
     RETAIN = 281,
     COND = 282,
     SCOND = 283,
     CCOND = 284,
     CONDELSE = 285,
     PORT = 286,
     INTERCONNECT = 287,
     DEVICE = 288,
     SETUP = 289,
     HOLD = 290,
     SETUPHOLD = 291,
     RECOVERY = 292,
     REMOVAL = 293,
     RECREM = 294,
     SKEW = 295,
     WIDTH = 296,
     PERIOD = 297,
     NOCHANGE = 298,
     NAME = 299,
     EXCEPTION = 300,
     PATHCONSTRAINT = 301,
     PERIODCONSTRAINT = 302,
     SUM = 303,
     DIFF = 304,
     SKEWCONSTRAINT = 305,
     ARRIVAL = 306,
     DEPARTURE = 307,
     SLACK = 308,
     WAVEFORM = 309,
     POSEDGE = 310,
     NEGEDGE = 311,
     CASE_EQU = 312,
     CASE_INEQU = 313,
     EQU = 314,
     INEQU = 315,
     L_AND = 316,
     L_OR = 317,
     LESS_OR_EQU = 318,
     GREATER_OR_EQU = 319,
     RIGHT_SHIFT = 320,
     LEFT_SHIFT = 321,
     U_NAND = 322,
     U_NOR = 323,
     U_XNOR = 324,
     U_XNOR_ALT = 325,
     BIT_CONST = 326,
     ONE_CONST = 327,
     ZERO_CONST = 328,
     EDGE = 329,
     PATH = 330,
     IDENTIFIER = 331,
     QSTRING = 332,
     NUMBER = 333
   };
#endif
/* Tokens.  */
#define DELAYFILE 258
#define SDFVERSION 259
#define DESIGN 260
#define DATE 261
#define VENDOR 262
#define PROGRAM 263
#define VERSION 264
#define DIVIDER 265
#define VOLTAGE 266
#define PROCESS 267
#define TEMPERATURE 268
#define TIMESCALE 269
#define CELL 270
#define CELLTYPE 271
#define INSTANCE 272
#define DELAY 273
#define TIMINGCHECK 274
#define TIMINGENV 275
#define PATHPULSE 276
#define PATHPULSEPERCENT 277
#define ABSOLUTE 278
#define INCREMENT 279
#define IOPATH 280
#define RETAIN 281
#define COND 282
#define SCOND 283
#define CCOND 284
#define CONDELSE 285
#define PORT 286
#define INTERCONNECT 287
#define DEVICE 288
#define SETUP 289
#define HOLD 290
#define SETUPHOLD 291
#define RECOVERY 292
#define REMOVAL 293
#define RECREM 294
#define SKEW 295
#define WIDTH 296
#define PERIOD 297
#define NOCHANGE 298
#define NAME 299
#define EXCEPTION 300
#define PATHCONSTRAINT 301
#define PERIODCONSTRAINT 302
#define SUM 303
#define DIFF 304
#define SKEWCONSTRAINT 305
#define ARRIVAL 306
#define DEPARTURE 307
#define SLACK 308
#define WAVEFORM 309
#define POSEDGE 310
#define NEGEDGE 311
#define CASE_EQU 312
#define CASE_INEQU 313
#define EQU 314
#define INEQU 315
#define L_AND 316
#define L_OR 317
#define LESS_OR_EQU 318
#define GREATER_OR_EQU 319
#define RIGHT_SHIFT 320
#define LEFT_SHIFT 321
#define U_NAND 322
#define U_NOR 323
#define U_XNOR 324
#define U_XNOR_ALT 325
#define BIT_CONST 326
#define ONE_CONST 327
#define ZERO_CONST 328
#define EDGE 329
#define PATH 330
#define IDENTIFIER 331
#define QSTRING 332
#define NUMBER 333




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "src/sdf_file.y"
{
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
/* Line 1489 of yacc.c.  */
#line 217 "lib/opt/lex_n_yacc/sdf_file.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE sdf_filelval;

