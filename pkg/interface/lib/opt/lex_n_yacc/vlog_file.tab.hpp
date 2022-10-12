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
     MODULE = 258,
     ENDMODULE = 259,
     PRIMITIVE = 260,
     ENDPRIMITIVE = 261,
     ASSIGN = 262,
     INPUT = 263,
     OUTPUT = 264,
     INOUT = 265,
     WIRE = 266,
     REG = 267,
     SUPPLY_L = 268,
     SUPPLY_H = 269,
     NAME = 270,
     NUMBER = 271,
     STRENGTH = 272
   };
#endif
/* Tokens.  */
#define MODULE 258
#define ENDMODULE 259
#define PRIMITIVE 260
#define ENDPRIMITIVE 261
#define ASSIGN 262
#define INPUT 263
#define OUTPUT 264
#define INOUT 265
#define WIRE 266
#define REG 267
#define SUPPLY_L 268
#define SUPPLY_H 269
#define NAME 270
#define NUMBER 271
#define STRENGTH 272




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 31 "src/vlog_file.y"
{
    char                  ychar[IntfNs::NAME_LEN];
    IntfNs::VlogNetType   ynetType;
    IntfNs::VlogNames     *ynames;
    IntfNs::VlogPortToNet *yportToNet;
}
/* Line 1489 of yacc.c.  */
#line 90 "lib/opt/lex_n_yacc/vlog_file.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE vlog_filelval;

