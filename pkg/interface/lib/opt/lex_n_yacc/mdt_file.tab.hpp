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
     MODEL = 258,
     MODEL_SOURCE = 259,
     INPUT = 260,
     OUTPUT = 261,
     INOUT = 262,
     INTERN = 263,
     PRIMITIVE = 264,
     INSTANCE = 265,
     NAME = 266
   };
#endif
/* Tokens.  */
#define MODEL 258
#define MODEL_SOURCE 259
#define INPUT 260
#define OUTPUT 261
#define INOUT 262
#define INTERN 263
#define PRIMITIVE 264
#define INSTANCE 265
#define NAME 266




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 31 "src/mdt_file.y"
{
    char                 ychar[IntfNs::NAME_LEN];
    IntfNs::MdtPortType  yportType;
    IntfNs::MdtNames     *ynames;
    IntfNs::MdtPortToNet *yportToNet;
}
/* Line 1489 of yacc.c.  */
#line 78 "lib/opt/lex_n_yacc/mdt_file.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mdt_filelval;

