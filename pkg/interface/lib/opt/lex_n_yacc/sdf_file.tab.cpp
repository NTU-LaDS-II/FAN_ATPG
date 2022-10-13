/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
	 simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
	 infringing on user name space.  This should be done even for local
	 variables, as they might otherwise be expanded by user macros.
	 There are some unavoidable exceptions within include files to
	 define necessary library symbols; they are noted "INFRINGES ON
	 USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse sdf_fileparse
#define yylex sdf_filelex
#define yyerror sdf_fileerror
#define yylval sdf_filelval
#define yychar sdf_filechar
#define yydebug sdf_filedebug
#define yynerrs sdf_filenerrs

/* Tokens.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
	 know about them.  */
enum yytokentype
{
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

/* Copy the first part of user declarations.  */
#line 1 "src/sdf_file.y"

// **************************************************************************
// File       [ sdf_file.y ]
// Author     [ littleshamoo ]
// Synopsis   [ Currently only supports SDF file header and absolute and
//              increment delay information ]
// Date       [ 2010/11/18 created ]
// **************************************************************************

#define YYPARSE_PARAM param

#include <cstring>
#include <cstdio>

#include "sdf_file.h"

using namespace std;
using namespace IntfNs;

extern char *sdf_filetext;
extern int sdf_fileline;
extern int sdf_filelex( void );
void sdf_fileerror( char const *msg );

IntfNs::SdfFile *sdf;

/* Enabling traces.  */
#ifndef YYDEBUG
#define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
#undef YYERROR_VERBOSE
#define YYERROR_VERBOSE 1
#else
#define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
#define YYTOKEN_TABLE 0
#endif

#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "src/sdf_file.y"
{
	float yfloat;
	char ychar[ IntfNs::NAME_LEN ];
	IntfNs::SdfDelayType ydelayType;
	IntfNs::SdfValue yvalue;
	IntfNs::SdfDelayValue ydelayValue;
	IntfNs::SdfDelayValueList ydelayValueList;
	IntfNs::SdfDelayDef *ydelayDef;
	IntfNs::SdfIoPath yioPath;
	IntfNs::SdfPortSpec yportSpec;
}
/* Line 187 of yacc.c.  */
#line 300 "lib/opt/lex_n_yacc/sdf_file.tab.cpp"
YYSTYPE;
#define yystype YYSTYPE /* obsolescent; will be withdrawn */
#define YYSTYPE_IS_DECLARED 1
#define YYSTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */

/* Line 216 of yacc.c.  */
#line 313 "lib/opt/lex_n_yacc/sdf_file.tab.cpp"

#ifdef short
#undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
#ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
#elif defined size_t
#define YYSIZE_T size_t
#elif !defined YYSIZE_T && ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#define YYSIZE_T size_t
#else
#define YYSIZE_T unsigned int
#endif
#endif

#define YYSIZE_MAXIMUM ( (YYSIZE_T)-1 )

#ifndef YY_
#if YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_( msgid ) dgettext( "bison-runtime", msgid )
#endif
#endif
#ifndef YY_
#define YY_( msgid ) msgid
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
#define YYUSE( e ) ( (void)( e ) )
#else
#define YYUSE( e ) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
#define YYID( n ) ( n )
#else
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static int
YYID( int i )
#else
static int
YYID( i )
int i;
#endif
{
	return i;
}
#endif

#if !defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

#ifdef YYSTACK_USE_ALLOCA
#if YYSTACK_USE_ALLOCA
#ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
#elif defined __BUILTIN_VA_ARG_INCR
#include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#elif defined _AIX
#define YYSTACK_ALLOC __alloca
#elif defined _MSC_VER
#include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#define alloca _alloca
#else
#define YYSTACK_ALLOC alloca
#if !defined _ALLOCA_H && !defined _STDLIB_H && ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef _STDLIB_H
#define _STDLIB_H 1
#endif
#endif
#endif
#endif
#endif

#ifdef YYSTACK_ALLOC
/* Pacify GCC's `empty if-body' warning.  */
#define YYSTACK_FREE( Ptr ) \
	do                        \
	{ /* empty */             \
		;                       \
	} while ( YYID( 0 ) )
#ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
	 and a page size can be as small as 4096 bytes.  So we cannot safely
	 invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
	 to allow for a few compiler-allocated temporary stack slots.  */
#define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#endif
#else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
#ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#endif
#if ( defined __cplusplus && !defined _STDLIB_H && !( ( defined YYMALLOC || defined malloc ) && ( defined YYFREE || defined free ) ) )
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef _STDLIB_H
#define _STDLIB_H 1
#endif
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#if !defined malloc && !defined _STDLIB_H && ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
void *malloc( YYSIZE_T ); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#ifndef YYFREE
#define YYFREE free
#if !defined free && !defined _STDLIB_H && ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
void free( void * );			/* INFRINGES ON USER NAME SPACE */
#endif
#endif
#endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */

#if ( !defined yyoverflow && ( !defined __cplusplus || ( defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL ) ) )

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
	yytype_int16 yyss;
	YYSTYPE yyvs;
};

/* The size of the maximum gap between one aligned stack and the next.  */
#define YYSTACK_GAP_MAXIMUM ( sizeof( union yyalloc ) - 1 )

/* The size of an array large to enough to hold all stacks, each with
	 N elements.  */
#define YYSTACK_BYTES( N ) \
	( ( N ) * ( sizeof( yytype_int16 ) + sizeof( YYSTYPE ) ) + YYSTACK_GAP_MAXIMUM )

/* Copy COUNT objects from FROM to TO.  The source and destination do
	 not overlap.  */
#ifndef YYCOPY
#if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY( To, From, Count ) \
	__builtin_memcpy( To, From, ( Count ) * sizeof( *( From ) ) )
#else
#define YYCOPY( To, From, Count )           \
	do                                        \
	{                                         \
		YYSIZE_T yyi;                           \
		for ( yyi = 0; yyi < ( Count ); yyi++ ) \
			( To )[ yyi ] = ( From )[ yyi ];      \
	} while ( YYID( 0 ) )
#endif
#endif

/* Relocate STACK from its old location to the new one.  The
	 local variables YYSIZE and YYSTACKSIZE give the old and new number of
	 elements in the stack, and YYPTR gives the new location of the
	 stack.  Advance YYPTR to a properly aligned location for the next
	 stack.  */
#define YYSTACK_RELOCATE( Stack )                                      \
	do                                                                   \
	{                                                                    \
		YYSIZE_T yynewbytes;                                               \
		YYCOPY( &yyptr->Stack, Stack, yysize );                            \
		Stack = &yyptr->Stack;                                             \
		yynewbytes = yystacksize * sizeof( *Stack ) + YYSTACK_GAP_MAXIMUM; \
		yyptr += yynewbytes / sizeof( *yyptr );                            \
	} while ( YYID( 0 ) )

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 767

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 101
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 81
/* YYNRULES -- Number of rules.  */
#define YYNRULES 233
/* YYNRULES -- Number of states.  */
#define YYNSTATES 550

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK 2
#define YYMAXUTOK 333

#define YYTRANSLATE( YYX ) \
	( (unsigned int)( YYX ) <= YYMAXUTOK ? yytranslate[ YYX ] : YYUNDEFTOK )

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
		{
				0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 89, 2, 2, 2, 88, 81, 2,
				91, 92, 86, 84, 99, 85, 93, 87, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 96, 2,
				82, 2, 83, 100, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 94, 2, 95, 80, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 97, 79, 98, 90, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
				5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
				15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
				25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
				35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
				45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
				55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
				65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
				75, 76, 77, 78 };

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
	 YYRHS.  */
static const yytype_uint16 yyprhs[] =
		{
				0, 0, 3, 6, 7, 13, 16, 19, 22, 25,
				28, 31, 34, 37, 40, 43, 45, 50, 55, 60,
				65, 70, 75, 80, 82, 84, 89, 94, 99, 104,
				109, 114, 117, 120, 122, 127, 133, 136, 141, 145,
				150, 155, 160, 163, 165, 167, 169, 171, 176, 179,
				181, 186, 189, 191, 196, 199, 201, 206, 211, 213,
				215, 217, 219, 221, 224, 227, 231, 234, 236, 239,
				241, 246, 251, 257, 264, 269, 275, 278, 282, 289,
				299, 301, 308, 315, 323, 332, 341, 351, 358, 365,
				373, 382, 391, 401, 408, 414, 420, 428, 430, 436,
				443, 448, 454, 459, 465, 469, 474, 479, 482, 484,
				486, 488, 496, 505, 511, 518, 525, 533, 541, 550,
				557, 565, 571, 574, 576, 579, 581, 590, 600, 609,
				619, 628, 638, 645, 647, 648, 653, 655, 657, 662,
				664, 666, 668, 670, 672, 674, 676, 678, 680, 682,
				687, 692, 699, 706, 708, 710, 713, 715, 718, 720,
				731, 742, 746, 750, 753, 757, 762, 767, 771, 776,
				780, 786, 788, 793, 799, 801, 804, 808, 815, 828,
				830, 834, 840, 842, 848, 852, 857, 859, 862, 864,
				867, 869, 873, 877, 881, 885, 889, 893, 897, 901,
				905, 909, 913, 917, 921, 925, 929, 933, 937, 941,
				945, 949, 951, 954, 958, 960, 962, 964, 966, 968,
				970, 972, 974, 976, 978, 980, 982, 984, 986, 988,
				990, 992, 994, 996 };

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
		{
				102, 0, -1, 103, 104, -1, -1, 91, 3, 105,
				119, 92, -1, 105, 107, -1, 105, 108, -1, 105,
				109, -1, 105, 110, -1, 105, 111, -1, 105, 112,
				-1, 105, 114, -1, 105, 115, -1, 105, 116, -1,
				105, 117, -1, 106, -1, 91, 4, 77, 92, -1,
				91, 5, 77, 92, -1, 91, 6, 77, 92, -1,
				91, 7, 77, 92, -1, 91, 8, 77, 92, -1,
				91, 9, 77, 92, -1, 91, 10, 113, 92, -1,
				87, -1, 93, -1, 91, 11, 169, 92, -1, 91,
				11, 78, 92, -1, 91, 12, 77, 92, -1, 91,
				13, 169, 92, -1, 91, 13, 78, 92, -1, 91,
				14, 118, 92, -1, 78, 76, -1, 119, 120, -1,
				120, -1, 91, 15, 121, 92, -1, 91, 15, 121,
				124, 92, -1, 122, 123, -1, 91, 16, 77, 92,
				-1, 91, 17, 92, -1, 91, 17, 75, 92, -1,
				91, 17, 76, 92, -1, 91, 17, 86, 92, -1,
				124, 125, -1, 125, -1, 126, -1, 128, -1, 130,
				-1, 91, 18, 127, 92, -1, 127, 132, -1, 132,
				-1, 91, 19, 129, 92, -1, 129, 142, -1, 142,
				-1, 91, 20, 131, 92, -1, 131, 149, -1, 149,
				-1, 91, 133, 135, 92, -1, 91, 134, 136, 92,
				-1, 21, -1, 22, -1, 23, -1, 24, -1, 168,
				-1, 137, 168, -1, 168, 168, -1, 137, 168, 168,
				-1, 136, 138, -1, 138, -1, 159, 159, -1, 140,
				-1, 91, 139, 140, 92, -1, 91, 141, 140, 92,
				-1, 91, 31, 159, 171, 92, -1, 91, 32, 159,
				159, 171, 92, -1, 91, 33, 171, 92, -1, 91,
				33, 159, 171, 92, -1, 27, 172, -1, 27, 77,
				172, -1, 91, 25, 156, 159, 171, 92, -1, 91,
				25, 156, 159, 91, 26, 171, 92, 92, -1, 30,
				-1, 91, 34, 143, 143, 168, 92, -1, 91, 35,
				143, 143, 168, 92, -1, 91, 36, 143, 143, 168,
				168, 92, -1, 91, 36, 143, 143, 168, 168, 144,
				92, -1, 91, 36, 143, 143, 168, 168, 145, 92,
				-1, 91, 36, 143, 143, 168, 168, 144, 145, 92,
				-1, 91, 37, 143, 143, 168, 92, -1, 91, 38,
				143, 143, 168, 92, -1, 91, 39, 143, 143, 168,
				168, 92, -1, 91, 39, 143, 143, 168, 168, 144,
				92, -1, 91, 39, 143, 143, 168, 168, 145, 92,
				-1, 91, 39, 143, 143, 168, 168, 144, 145, 92,
				-1, 91, 40, 143, 143, 168, 92, -1, 91, 41,
				143, 168, 92, -1, 91, 42, 143, 168, 92, -1,
				91, 43, 143, 143, 168, 168, 92, -1, 156, -1,
				91, 27, 176, 156, 92, -1, 91, 27, 77, 176,
				156, 92, -1, 91, 28, 176, 92, -1, 91, 28,
				77, 176, 92, -1, 91, 29, 176, 92, -1, 91,
				29, 77, 176, 92, -1, 91, 44, 92, -1, 91,
				44, 77, 92, -1, 91, 45, 148, 92, -1, 148,
				123, -1, 123, -1, 150, -1, 153, -1, 91, 46,
				159, 151, 168, 168, 92, -1, 91, 46, 146, 159,
				151, 168, 168, 92, -1, 91, 47, 159, 168, 92,
				-1, 91, 47, 159, 168, 147, 92, -1, 91, 48,
				155, 155, 168, 92, -1, 91, 48, 155, 155, 168,
				168, 92, -1, 91, 48, 155, 155, 152, 168, 92,
				-1, 91, 48, 155, 155, 152, 168, 168, 92, -1,
				91, 49, 155, 155, 168, 92, -1, 91, 49, 155,
				155, 168, 168, 92, -1, 91, 50, 156, 168, 92,
				-1, 151, 159, -1, 159, -1, 152, 155, -1, 155,
				-1, 91, 51, 159, 168, 168, 168, 168, 92, -1,
				91, 51, 157, 159, 168, 168, 168, 168, 92, -1,
				91, 52, 159, 168, 168, 168, 168, 92, -1, 91,
				52, 157, 159, 168, 168, 168, 168, 92, -1, 91,
				53, 159, 168, 168, 168, 168, 92, -1, 91, 53,
				159, 168, 168, 168, 168, 78, 92, -1, 91, 54,
				159, 78, 163, 92, -1, 78, -1, -1, 91, 159,
				159, 92, -1, 159, -1, 157, -1, 91, 158, 159,
				92, -1, 55, -1, 56, -1, 74, -1, 76, -1,
				78, -1, 160, -1, 161, -1, 162, -1, 76, -1,
				75, -1, 76, 94, 78, 95, -1, 75, 94, 78,
				95, -1, 76, 94, 78, 96, 78, 95, -1, 75,
				94, 78, 96, 78, 95, -1, 164, -1, 165, -1,
				164, 166, -1, 166, -1, 165, 167, -1, 167, -1,
				91, 55, 78, 154, 92, 91, 56, 78, 154, 92,
				-1, 91, 56, 78, 154, 92, 91, 55, 78, 154,
				92, -1, 91, 78, 92, -1, 91, 169, 92, -1,
				91, 92, -1, 78, 96, 96, -1, 78, 96, 78,
				96, -1, 78, 96, 96, 78, -1, 96, 78, 96,
				-1, 96, 78, 96, 78, -1, 96, 96, 78, -1,
				78, 96, 78, 96, 78, -1, 168, -1, 91, 168,
				168, 92, -1, 91, 168, 168, 168, 92, -1, 170,
				-1, 170, 170, -1, 170, 170, 170, -1, 170, 170,
				170, 170, 170, 170, -1, 170, 170, 170, 170, 170,
				170, 170, 170, 170, 170, 170, 170, -1, 173, -1,
				97, 173, 98, -1, 97, 172, 99, 173, 98, -1,
				174, -1, 174, 100, 172, 96, 173, -1, 91, 174,
				92, -1, 179, 91, 174, 92, -1, 160, -1, 179,
				160, -1, 178, -1, 179, 178, -1, 175, -1, 174,
				84, 174, -1, 174, 85, 174, -1, 174, 86, 174,
				-1, 174, 87, 174, -1, 174, 88, 174, -1, 174,
				59, 174, -1, 174, 60, 174, -1, 174, 57, 174,
				-1, 174, 58, 174, -1, 174, 61, 174, -1, 174,
				62, 174, -1, 174, 82, 174, -1, 174, 63, 174,
				-1, 174, 83, 174, -1, 174, 64, 174, -1, 174,
				81, 174, -1, 174, 80, 174, -1, 174, 79, 174,
				-1, 174, 65, 174, -1, 174, 66, 174, -1, 177,
				-1, 180, 177, -1, 177, 181, 178, -1, 161, -1,
				71, -1, 78, -1, 84, -1, 85, -1, 89, -1,
				90, -1, 81, -1, 67, -1, 79, -1, 68, -1,
				80, -1, 69, -1, 70, -1, 89, -1, 90, -1,
				59, -1, 60, -1, 57, -1, 58, -1 };

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
		{
				0, 93, 93, 97, 102, 107, 108, 109, 110, 111,
				112, 113, 114, 115, 116, 117, 122, 127, 132, 137,
				142, 147, 152, 157, 158, 163, 164, 172, 176, 177,
				185, 190, 195, 196, 201, 202, 207, 212, 217, 218,
				219, 220, 225, 226, 231, 232, 233, 238, 243, 244,
				249, 254, 255, 260, 265, 266, 271, 272, 277, 278,
				283, 284, 289, 290, 291, 292, 297, 302, 311, 317,
				320, 323, 326, 332, 339, 345, 355, 356, 361, 368,
				379, 384, 385, 386, 387, 388, 389, 390, 391, 392,
				393, 394, 395, 396, 397, 398, 399, 404, 405, 406,
				411, 412, 417, 418, 422, 423, 427, 432, 433, 438,
				439, 444, 445, 446, 447, 448, 449, 450, 451, 452,
				453, 454, 459, 460, 465, 466, 471, 472, 473, 474,
				475, 476, 477, 482, 483, 488, 493, 495, 501, 507,
				508, 509, 513, 517, 525, 529, 530, 535, 536, 537,
				541, 549, 557, 569, 570, 575, 576, 581, 582, 587,
				592, 597, 599, 603, 608, 612, 616, 620, 624, 628,
				632, 640, 642, 645, 653, 657, 662, 668, 677, 696,
				697, 698, 703, 704, 709, 710, 711, 712, 713, 714,
				715, 720, 721, 722, 723, 724, 725, 726, 727, 728,
				729, 730, 731, 732, 733, 734, 735, 736, 737, 738,
				739, 744, 745, 746, 751, 756, 757, 762, 763, 764,
				765, 766, 767, 768, 769, 770, 771, 772, 777, 778,
				783, 784, 785, 786 };
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
	 First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
		{
				"$end", "error", "$undefined", "DELAYFILE", "SDFVERSION", "DESIGN",
				"DATE", "VENDOR", "PROGRAM", "VERSION", "DIVIDER", "VOLTAGE", "PROCESS",
				"TEMPERATURE", "TIMESCALE", "CELL", "CELLTYPE", "INSTANCE", "DELAY",
				"TIMINGCHECK", "TIMINGENV", "PATHPULSE", "PATHPULSEPERCENT", "ABSOLUTE",
				"INCREMENT", "IOPATH", "RETAIN", "COND", "SCOND", "CCOND", "CONDELSE",
				"PORT", "INTERCONNECT", "DEVICE", "SETUP", "HOLD", "SETUPHOLD",
				"RECOVERY", "REMOVAL", "RECREM", "SKEW", "WIDTH", "PERIOD", "NOCHANGE",
				"NAME", "EXCEPTION", "PATHCONSTRAINT", "PERIODCONSTRAINT", "SUM", "DIFF",
				"SKEWCONSTRAINT", "ARRIVAL", "DEPARTURE", "SLACK", "WAVEFORM", "POSEDGE",
				"NEGEDGE", "CASE_EQU", "CASE_INEQU", "EQU", "INEQU", "L_AND", "L_OR",
				"LESS_OR_EQU", "GREATER_OR_EQU", "RIGHT_SHIFT", "LEFT_SHIFT", "U_NAND",
				"U_NOR", "U_XNOR", "U_XNOR_ALT", "BIT_CONST", "ONE_CONST", "ZERO_CONST",
				"EDGE", "PATH", "IDENTIFIER", "QSTRING", "NUMBER", "'|'", "'^'", "'&'",
				"'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "'('",
				"')'", "'.'", "'['", "']'", "':'", "'{'", "'}'", "','", "'?'", "$accept",
				"start", "initialize", "delay_file", "sdf_header", "sdf_version",
				"design_name", "date", "vendor", "program_name", "program_version",
				"hierarchy_divider", "hchar", "voltage", "process", "temperature",
				"time_scale", "tsvalue", "cells", "cell", "add_cell", "cell_type",
				"cell_instance", "timing_specs", "timing_spec", "del_spec", "deltypes",
				"tc_spec", "tchk_defs", "te_spec", "te_defs", "deltype", "pathtype",
				"valuetype", "del_path", "del_defs", "input_output_path", "del_def",
				"del_cond", "del_def_iopath", "cond_else_type", "tchk_def", "port_tchk",
				"scond", "ccond", "name", "exception", "cell_instances", "te_def",
				"cns_def", "port_instances", "constraint_paths", "tenv_def", "number_q",
				"constraint_path", "port_spec", "port_edge", "edge_identifier",
				"port_instance", "port", "scalar_port", "bus_port", "edge_list",
				"pos_pairs", "neg_pairs", "pos_pair", "neg_pair", "value", "triple",
				"delval", "delval_list", "expression", "conditional_expression",
				"simple_expression", "binary_expression", "timing_check_condition",
				"scalar_node", "scalar_constant", "unary_op", "inv_op", "equ_op", 0 };
#endif

#ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
	 token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
		{
				0, 256, 257, 258, 259, 260, 261, 262, 263, 264,
				265, 266, 267, 268, 269, 270, 271, 272, 273, 274,
				275, 276, 277, 278, 279, 280, 281, 282, 283, 284,
				285, 286, 287, 288, 289, 290, 291, 292, 293, 294,
				295, 296, 297, 298, 299, 300, 301, 302, 303, 304,
				305, 306, 307, 308, 309, 310, 311, 312, 313, 314,
				315, 316, 317, 318, 319, 320, 321, 322, 323, 324,
				325, 326, 327, 328, 329, 330, 331, 332, 333, 124,
				94, 38, 60, 62, 43, 45, 42, 47, 37, 33,
				126, 40, 41, 46, 91, 93, 58, 123, 125, 44,
				63 };
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
		{
				0, 101, 102, 103, 104, 105, 105, 105, 105, 105,
				105, 105, 105, 105, 105, 105, 106, 107, 108, 109,
				110, 111, 112, 113, 113, 114, 114, 115, 116, 116,
				117, 118, 119, 119, 120, 120, 121, 122, 123, 123,
				123, 123, 124, 124, 125, 125, 125, 126, 127, 127,
				128, 129, 129, 130, 131, 131, 132, 132, 133, 133,
				134, 134, 135, 135, 135, 135, 136, 136, 137, 138,
				138, 138, 138, 138, 138, 138, 139, 139, 140, 140,
				141, 142, 142, 142, 142, 142, 142, 142, 142, 142,
				142, 142, 142, 142, 142, 142, 142, 143, 143, 143,
				144, 144, 145, 145, 146, 146, 147, 148, 148, 149,
				149, 150, 150, 150, 150, 150, 150, 150, 150, 150,
				150, 150, 151, 151, 152, 152, 153, 153, 153, 153,
				153, 153, 153, 154, 154, 155, 156, 156, 157, 158,
				158, 158, 158, 158, 159, 160, 160, 161, 161, 161,
				161, 162, 162, 163, 163, 164, 164, 165, 165, 166,
				167, 168, 168, 168, 169, 169, 169, 169, 169, 169,
				169, 170, 170, 170, 171, 171, 171, 171, 171, 172,
				172, 172, 173, 173, 174, 174, 174, 174, 174, 174,
				174, 175, 175, 175, 175, 175, 175, 175, 175, 175,
				175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
				175, 176, 176, 176, 177, 178, 178, 179, 179, 179,
				179, 179, 179, 179, 179, 179, 179, 179, 180, 180,
				181, 181, 181, 181 };

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
		{
				0, 2, 2, 0, 5, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 1, 4, 4, 4, 4,
				4, 4, 4, 1, 1, 4, 4, 4, 4, 4,
				4, 2, 2, 1, 4, 5, 2, 4, 3, 4,
				4, 4, 2, 1, 1, 1, 1, 4, 2, 1,
				4, 2, 1, 4, 2, 1, 4, 4, 1, 1,
				1, 1, 1, 2, 2, 3, 2, 1, 2, 1,
				4, 4, 5, 6, 4, 5, 2, 3, 6, 9,
				1, 6, 6, 7, 8, 8, 9, 6, 6, 7,
				8, 8, 9, 6, 5, 5, 7, 1, 5, 6,
				4, 5, 4, 5, 3, 4, 4, 2, 1, 1,
				1, 7, 8, 5, 6, 6, 7, 7, 8, 6,
				7, 5, 2, 1, 2, 1, 8, 9, 8, 9,
				8, 9, 6, 1, 0, 4, 1, 1, 4, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
				4, 6, 6, 1, 1, 2, 1, 2, 1, 10,
				10, 3, 3, 2, 3, 4, 4, 3, 4, 3,
				5, 1, 4, 5, 1, 2, 3, 6, 12, 1,
				3, 5, 1, 5, 3, 4, 1, 2, 1, 2,
				1, 3, 3, 3, 3, 3, 3, 3, 3, 3,
				3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
				3, 1, 2, 3, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1 };

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
	 STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
	 means the default is an error.  */
static const yytype_uint8 yydefact[] =
		{
				3, 0, 0, 1, 0, 2, 0, 0, 0, 15,
				0, 0, 5, 6, 7, 8, 9, 10, 11, 12,
				13, 14, 0, 33, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 4, 32, 16,
				0, 0, 0, 0, 0, 23, 24, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
				18, 19, 20, 21, 22, 26, 0, 0, 0, 25,
				27, 29, 28, 31, 30, 0, 0, 34, 0, 43,
				44, 45, 46, 0, 36, 0, 164, 167, 169, 0,
				0, 0, 0, 35, 42, 0, 165, 166, 168, 37,
				0, 0, 49, 0, 0, 52, 0, 0, 55, 109,
				110, 0, 0, 0, 38, 170, 58, 59, 60, 61,
				0, 0, 47, 48, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 50, 51, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 53, 54, 39, 40, 41,
				148, 147, 0, 0, 0, 0, 144, 145, 146, 62,
				0, 0, 67, 69, 0, 0, 97, 137, 136, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 163, 0, 56, 63,
				68, 64, 0, 0, 80, 0, 0, 0, 0, 0,
				57, 66, 0, 139, 140, 141, 142, 143, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 123, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 161, 162, 65, 0,
				222, 224, 226, 227, 215, 0, 216, 223, 225, 221,
				217, 218, 219, 220, 0, 0, 186, 76, 179, 182,
				190, 188, 0, 0, 0, 0, 0, 171, 174, 0,
				0, 0, 0, 148, 147, 0, 228, 229, 214, 0,
				211, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				94, 95, 0, 0, 104, 0, 122, 0, 0, 113,
				0, 0, 0, 0, 125, 0, 0, 121, 0, 0,
				0, 0, 0, 0, 0, 153, 154, 156, 158, 150,
				0, 149, 0, 0, 77, 0, 0, 179, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				187, 189, 0, 0, 0, 0, 175, 74, 70, 71,
				0, 0, 0, 0, 232, 233, 230, 231, 0, 212,
				138, 81, 82, 0, 87, 88, 0, 93, 0, 105,
				0, 0, 0, 114, 135, 124, 0, 115, 0, 119,
				0, 0, 0, 0, 0, 0, 0, 0, 132, 0,
				155, 0, 157, 0, 0, 0, 0, 184, 0, 180,
				198, 199, 196, 197, 200, 201, 203, 205, 209, 210,
				208, 207, 206, 202, 204, 191, 192, 193, 194, 195,
				0, 0, 72, 0, 0, 75, 176, 0, 0, 0,
				98, 213, 0, 83, 0, 0, 89, 0, 0, 96,
				0, 111, 108, 0, 117, 0, 116, 120, 0, 0,
				0, 0, 0, 134, 134, 152, 151, 0, 78, 0,
				0, 185, 73, 172, 0, 0, 99, 0, 0, 0,
				84, 0, 85, 90, 0, 91, 112, 106, 107, 118,
				0, 126, 0, 128, 0, 130, 133, 0, 0, 0,
				181, 183, 173, 0, 0, 0, 0, 0, 86, 92,
				127, 129, 131, 0, 0, 0, 177, 0, 100, 0,
				102, 0, 0, 79, 0, 101, 103, 0, 0, 0,
				134, 134, 0, 0, 0, 0, 159, 160, 0, 178 };

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
		{
				-1, 1, 2, 5, 8, 9, 12, 13, 14, 15,
				16, 17, 47, 18, 19, 20, 21, 55, 22, 23,
				57, 58, 84, 78, 79, 80, 101, 81, 104, 82,
				107, 102, 120, 121, 153, 161, 154, 162, 208, 163,
				209, 105, 165, 454, 455, 179, 310, 463, 108, 109,
				231, 313, 110, 507, 183, 166, 167, 218, 168, 156,
				157, 158, 324, 325, 326, 327, 328, 277, 197, 278,
				279, 267, 268, 269, 270, 289, 290, 271, 272, 291,
				378 };

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
	 STATE-NUM.  */
#define YYPACT_NINF -457
static const yytype_int16 yypact[] =
		{
				-457, 4, -76, -457, 9, -457, -34, 61, -11, -457,
				37, 443, -457, -457, -457, -457, -457, -457, -457, -457,
				-457, -457, -68, -457, 33, 53, 58, 80, 91, 109,
				30, 0, 117, 10, 154, 149, 233, -457, -457, -457,
				165, 172, 211, 215, 238, -457, -457, 262, 56, 11,
				269, 293, 157, 298, 175, 310, 239, -56, 354, -457,
				-457, -457, -457, -457, -457, -457, 35, 231, 385, -457,
				-457, -457, -457, -457, -457, 399, 278, -457, -47, -457,
				-457, -457, -457, 460, -457, 386, 403, 405, -457, 414,
				393, 437, 438, -457, -457, 105, 452, -457, -457, -457,
				335, 92, -457, 474, 112, -457, 496, 151, -457, -457,
				-457, 459, 461, 472, -457, -457, -457, -457, -457, -457,
				114, 481, -457, -457, 159, 159, 159, 159, 159, 159,
				159, 159, 159, 159, -457, -457, 161, 193, 482, 482,
				163, 163, 163, 193, 193, -457, -457, -457, -457, -457,
				458, 471, -70, 490, 492, 193, -457, -457, -457, 492,
				304, 180, -457, -457, -14, 159, -457, -457, -457, 159,
				159, 159, 159, 159, 159, 492, 492, 159, 522, 193,
				193, 492, 193, 482, 482, 73, 492, 193, 492, 193,
				492, 492, 510, 512, 523, 167, -457, 497, -457, 492,
				-457, -457, 163, 543, -457, 193, 193, 171, 509, 509,
				-457, -457, 190, -457, -457, -457, -457, -457, 193, 492,
				492, 492, 492, 492, 492, 492, 524, 525, 492, -49,
				193, 114, -457, 209, 193, 534, 492, 537, 492, 492,
				492, 492, 492, 535, 247, 279, -457, -457, -457, 193,
				-457, -457, -457, -457, -457, 568, -457, -457, -457, -457,
				-457, -457, -457, -457, 619, 568, -457, -457, -457, 407,
				-457, -457, 101, 539, 193, -71, 539, -457, 539, 549,
				590, 550, 553, 556, 557, 185, -457, -457, -457, 163,
				313, 301, 562, 563, 564, 492, 572, 574, 492, 575,
				-457, -457, 492, 576, -457, 114, -457, 492, 586, -457,
				577, 578, 78, 534, -457, 290, 292, -457, 492, 492,
				492, 492, 492, 341, 579, 581, 582, -457, -457, -457,
				596, -457, 598, 584, -457, 439, 592, 580, 619, 619,
				619, 619, 619, 619, 619, 619, 619, 619, 619, 619,
				619, 619, 619, 619, 619, 619, 619, 619, 568, 619,
				-457, -457, 585, 539, 492, 600, 539, -457, -457, -457,
				615, 618, 163, 609, -457, -457, -457, -457, -4, -457,
				-457, -457, -457, 307, -457, -457, 309, -457, 610, -457,
				492, 613, 354, -457, -457, -457, 314, -457, 614, -457,
				620, 492, 492, 492, 492, 492, 629, 633, -457, 658,
				-457, 659, -457, 621, 622, -9, 626, -457, 619, -457,
				597, 597, 597, 597, 329, 79, -55, -55, 204, 204,
				511, 521, 281, -55, -55, 235, 235, -457, -457, -457,
				623, 475, -457, 628, 327, -457, 539, 627, 630, 631,
				-457, -457, 392, -457, 332, 632, -457, 368, 634, -457,
				635, -457, -457, 370, -457, 636, -457, -457, 492, 637,
				492, 638, 54, 643, 643, -457, -457, 539, -457, 616,
				619, -457, -457, -457, 639, 539, -457, 236, 243, 703,
				-457, 641, -457, -457, 642, -457, -457, -457, -457, -457,
				644, -457, 645, -457, 646, -457, -457, 647, 648, 649,
				-457, -457, -457, 539, 185, 650, 185, 651, -457, -457,
				-457, -457, -457, 653, 654, 655, 539, 656, -457, 657,
				-457, 679, 691, -457, 539, -457, -457, 672, 673, 539,
				643, 643, 539, 660, 661, 539, -457, -457, 539, -457 };

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
		{
				-457, -457, -457, -457, -457, -457, -457, -457, -457, -457,
				-457, -457, -457, -457, -457, -457, -457, -457, -457, 732,
				-457, -457, -386, -457, 677, -457, -457, -457, -457, -457,
				-457, 662, -457, -457, -457, -457, -457, 595, -457, 266,
				-457, 663, -79, 371, -381, -457, -457, -457, 652, -457,
				528, -457, -457, -456, -125, -139, 338, -457, 173, -131,
				-210, -457, -457, -457, -457, 435, 436, -120, 140, -240,
				-236, -246, -249, 87, -457, -258, 470, -269, -457, -457,
				-457 };

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
	 positive, shift that token.  If negative, reduce the rule which
	 number is the opposite.  If zero, do what YYDEFACT says.
	 If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
		{
				159, 186, 288, 361, 3, 458, 462, 195, 195, 334,
				346, 347, 6, 212, 184, 4, 337, 477, 508, 336,
				152, 196, 196, 36, 37, 49, 49, 372, 303, 353,
				354, 355, 356, 357, 199, 76, 77, 362, 366, 201,
				365, 213, 214, 304, 76, 93, 169, 170, 171, 172,
				173, 174, 175, 176, 177, 226, 227, 7, 235, 236,
				215, 233, 216, 249, 217, 10, 237, 254, 239, 195,
				241, 242, 266, 491, 256, 288, 494, 498, 48, 248,
				11, 288, 152, 196, 543, 544, 219, 49, 52, 67,
				220, 221, 222, 223, 224, 225, 49, 416, 228, 293,
				294, 295, 296, 297, 298, 299, 49, 68, 302, 451,
				314, 307, 440, 85, 24, 315, 316, 45, 318, 319,
				320, 321, 322, 46, 266, 39, 446, 443, 213, 214,
				40, 86, 504, 266, 266, 41, 338, 339, 340, 341,
				342, 360, 344, 345, 346, 347, 505, 215, 65, 216,
				373, 217, 66, 150, 151, 364, 195, 42, 348, 349,
				350, 351, 352, 353, 354, 355, 356, 357, 43, 479,
				196, 50, 254, 53, 49, 383, 150, 151, 386, 256,
				111, 112, 388, 100, 122, 390, 44, 391, 395, 150,
				151, 113, 359, 396, 51, 398, 400, 114, 401, 402,
				403, 404, 405, 103, 134, 152, 485, 266, 266, 266,
				266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
				266, 266, 266, 266, 266, 266, 266, 266, 266, 515,
				517, 511, 54, 449, 150, 151, 150, 151, 150, 151,
				56, 509, 106, 145, 444, 513, 150, 151, 35, 71,
				164, 73, 178, 66, 185, 75, 527, 59, 529, 246,
				283, 284, 275, 66, 60, 283, 284, 285, 150, 151,
				460, 160, 210, 526, 286, 287, 465, 288, 288, 286,
				287, 468, 469, 470, 471, 472, 534, 266, 353, 354,
				355, 356, 357, 155, 539, 364, 90, 91, 92, 542,
				308, 309, 545, 61, 288, 548, 288, 62, 549, 180,
				181, 283, 284, 514, 188, 190, 191, 192, 283, 284,
				516, 355, 356, 357, 484, 286, 287, 87, 200, 202,
				63, 203, 286, 287, 204, 205, 206, 207, 338, 339,
				340, 341, 329, 330, 344, 345, 346, 347, 500, 266,
				502, 335, 230, 232, 64, 234, 116, 117, 118, 119,
				238, 69, 240, 351, 352, 353, 354, 355, 356, 357,
				374, 375, 376, 377, 331, 332, 283, 284, 273, 274,
				276, 152, 397, 152, 399, 70, 338, 339, 340, 341,
				72, 292, 344, 345, 346, 347, 406, 407, 452, 453,
				452, 456, 74, 232, 306, 152, 464, 311, 348, 349,
				350, 351, 352, 353, 354, 355, 356, 357, 152, 483,
				487, 488, 333, 489, 490, 420, 421, 422, 423, 424,
				425, 426, 427, 428, 429, 430, 431, 432, 433, 434,
				435, 436, 437, 438, 439, 83, 441, 363, 25, 26,
				27, 28, 29, 30, 31, 32, 33, 34, 35, 489,
				493, 83, 497, 88, 338, 339, 340, 341, 342, 343,
				344, 345, 346, 347, 281, 282, 89, 95, 306, 187,
				189, 97, 96, 98, 100, 234, 348, 349, 350, 351,
				352, 353, 354, 355, 356, 357, 338, 339, 340, 341,
				342, 343, 344, 345, 346, 347, 99, 358, 124, 125,
				126, 127, 128, 129, 130, 131, 132, 133, 348, 349,
				350, 351, 352, 353, 354, 355, 356, 357, 103, 106,
				115, 417, 338, 339, 340, 341, 342, 343, 344, 345,
				346, 347, 136, 137, 138, 139, 140, 141, 142, 143,
				144, 147, 193, 148, 348, 349, 350, 351, 352, 353,
				354, 355, 356, 357, 149, 194, 229, 481, 338, 339,
				340, 341, 160, 182, 344, 345, 346, 347, 338, 339,
				340, 341, 198, 152, 344, 345, 346, 347, 243, 247,
				244, 349, 350, 351, 352, 353, 354, 355, 356, 357,
				280, 245, 350, 351, 352, 353, 354, 355, 356, 357,
				250, 251, 252, 253, 254, 202, 300, 301, 150, 151,
				255, 256, 257, 258, 259, 312, 323, 260, 261, 317,
				275, 392, 262, 263, 264, 250, 251, 252, 253, 254,
				265, 367, 368, 150, 151, 369, 256, 257, 258, 259,
				370, 371, 260, 261, 380, 381, 382, 262, 263, 264,
				344, 345, 346, 347, 384, 265, 385, 387, 389, 393,
				394, 408, 409, 411, 413, 415, 414, 442, 419, 351,
				352, 353, 354, 355, 356, 357, 250, 251, 252, 253,
				254, 418, 445, 447, 150, 151, 448, 256, 257, 258,
				259, 450, 459, 260, 261, 461, 466, 473, 262, 263,
				264, 474, 467, 406, 510, 407, 475, 476, 478, 480,
				482, 506, 329, 486, 492, 331, 495, 496, 499, 501,
				503, 512, 488, 518, 519, 537, 520, 521, 522, 523,
				524, 525, 528, 530, 531, 532, 538, 533, 535, 536,
				540, 541, 546, 547, 38, 94, 211, 457, 305, 146,
				410, 379, 412, 123, 0, 0, 0, 135 };

static const yytype_int16 yycheck[] =
		{
				120, 140, 212, 272, 0, 386, 392, 78, 78, 255,
				65, 66, 3, 27, 139, 91, 265, 26, 474, 265,
				91, 92, 92, 91, 92, 96, 96, 285, 77, 84,
				85, 86, 87, 88, 154, 91, 92, 273, 278, 159,
				276, 55, 56, 92, 91, 92, 125, 126, 127, 128,
				129, 130, 131, 132, 133, 175, 176, 91, 183, 184,
				74, 181, 76, 202, 78, 4, 186, 71, 188, 78,
				190, 191, 203, 454, 78, 285, 457, 463, 78, 199,
				91, 291, 91, 92, 540, 541, 165, 96, 78, 78,
				169, 170, 171, 172, 173, 174, 96, 333, 177, 219,
				220, 221, 222, 223, 224, 225, 96, 96, 228, 378,
				235, 231, 358, 78, 77, 235, 236, 87, 238, 239,
				240, 241, 242, 93, 255, 92, 366, 363, 55, 56,
				77, 96, 78, 264, 265, 77, 57, 58, 59, 60,
				61, 272, 63, 64, 65, 66, 92, 74, 92, 76,
				289, 78, 96, 75, 76, 275, 78, 77, 79, 80,
				81, 82, 83, 84, 85, 86, 87, 88, 77, 418,
				92, 31, 71, 33, 96, 295, 75, 76, 298, 78,
				75, 76, 302, 91, 92, 305, 77, 307, 313, 75,
				76, 86, 91, 313, 77, 315, 316, 92, 318, 319,
				320, 321, 322, 91, 92, 91, 446, 338, 339, 340,
				341, 342, 343, 344, 345, 346, 347, 348, 349, 350,
				351, 352, 353, 354, 355, 356, 357, 358, 359, 487,
				488, 480, 78, 372, 75, 76, 75, 76, 75, 76,
				91, 477, 91, 92, 364, 485, 75, 76, 15, 92,
				91, 76, 91, 96, 91, 16, 514, 92, 516, 92,
				75, 76, 91, 96, 92, 75, 76, 77, 75, 76,
				390, 91, 92, 513, 89, 90, 396, 487, 488, 89,
				90, 401, 402, 403, 404, 405, 526, 418, 84, 85,
				86, 87, 88, 120, 534, 415, 18, 19, 20, 539,
				91, 92, 542, 92, 514, 545, 516, 92, 548, 136,
				137, 75, 76, 77, 141, 142, 143, 144, 75, 76,
				77, 86, 87, 88, 444, 89, 90, 96, 155, 25,
				92, 27, 89, 90, 30, 31, 32, 33, 57, 58,
				59, 60, 95, 96, 63, 64, 65, 66, 468, 480,
				470, 264, 179, 180, 92, 182, 21, 22, 23, 24,
				187, 92, 189, 82, 83, 84, 85, 86, 87, 88,
				57, 58, 59, 60, 95, 96, 75, 76, 205, 206,
				207, 91, 92, 91, 92, 92, 57, 58, 59, 60,
				92, 218, 63, 64, 65, 66, 55, 56, 91, 92,
				91, 92, 92, 230, 231, 91, 92, 234, 79, 80,
				81, 82, 83, 84, 85, 86, 87, 88, 91, 92,
				28, 29, 249, 91, 92, 338, 339, 340, 341, 342,
				343, 344, 345, 346, 347, 348, 349, 350, 351, 352,
				353, 354, 355, 356, 357, 91, 359, 274, 5, 6,
				7, 8, 9, 10, 11, 12, 13, 14, 15, 91,
				92, 91, 92, 78, 57, 58, 59, 60, 61, 62,
				63, 64, 65, 66, 208, 209, 77, 17, 305, 141,
				142, 78, 96, 78, 91, 312, 79, 80, 81, 82,
				83, 84, 85, 86, 87, 88, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 92, 100, 34, 35,
				36, 37, 38, 39, 40, 41, 42, 43, 79, 80,
				81, 82, 83, 84, 85, 86, 87, 88, 91, 91,
				78, 92, 57, 58, 59, 60, 61, 62, 63, 64,
				65, 66, 46, 47, 48, 49, 50, 51, 52, 53,
				54, 92, 94, 92, 79, 80, 81, 82, 83, 84,
				85, 86, 87, 88, 92, 94, 44, 92, 57, 58,
				59, 60, 91, 91, 63, 64, 65, 66, 57, 58,
				59, 60, 92, 91, 63, 64, 65, 66, 78, 92,
				78, 80, 81, 82, 83, 84, 85, 86, 87, 88,
				91, 78, 81, 82, 83, 84, 85, 86, 87, 88,
				67, 68, 69, 70, 71, 25, 92, 92, 75, 76,
				77, 78, 79, 80, 81, 91, 91, 84, 85, 92,
				91, 45, 89, 90, 91, 67, 68, 69, 70, 71,
				97, 92, 92, 75, 76, 92, 78, 79, 80, 81,
				94, 94, 84, 85, 92, 92, 92, 89, 90, 91,
				63, 64, 65, 66, 92, 97, 92, 92, 92, 92,
				92, 92, 91, 91, 78, 91, 78, 92, 98, 82,
				83, 84, 85, 86, 87, 88, 67, 68, 69, 70,
				71, 99, 92, 78, 75, 76, 78, 78, 79, 80,
				81, 92, 92, 84, 85, 92, 92, 78, 89, 90,
				91, 78, 92, 55, 98, 56, 95, 95, 92, 96,
				92, 78, 95, 92, 92, 95, 92, 92, 92, 92,
				92, 92, 29, 92, 92, 56, 92, 92, 92, 92,
				92, 92, 92, 92, 91, 91, 55, 92, 92, 92,
				78, 78, 92, 92, 22, 78, 161, 386, 230, 107,
				325, 291, 326, 101, -1, -1, -1, 104 };

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
	 symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
		{
				0, 102, 103, 0, 91, 104, 3, 91, 105, 106,
				4, 91, 107, 108, 109, 110, 111, 112, 114, 115,
				116, 117, 119, 120, 77, 5, 6, 7, 8, 9,
				10, 11, 12, 13, 14, 15, 91, 92, 120, 92,
				77, 77, 77, 77, 77, 87, 93, 113, 78, 96,
				169, 77, 78, 169, 78, 118, 91, 121, 122, 92,
				92, 92, 92, 92, 92, 92, 96, 78, 96, 92,
				92, 92, 92, 76, 92, 16, 91, 92, 124, 125,
				126, 128, 130, 91, 123, 78, 96, 96, 78, 77,
				18, 19, 20, 92, 125, 17, 96, 78, 78, 92,
				91, 127, 132, 91, 129, 142, 91, 131, 149, 150,
				153, 75, 76, 86, 92, 78, 21, 22, 23, 24,
				133, 134, 92, 132, 34, 35, 36, 37, 38, 39,
				40, 41, 42, 43, 92, 142, 46, 47, 48, 49,
				50, 51, 52, 53, 54, 92, 149, 92, 92, 92,
				75, 76, 91, 135, 137, 159, 160, 161, 162, 168,
				91, 136, 138, 140, 91, 143, 156, 157, 159, 143,
				143, 143, 143, 143, 143, 143, 143, 143, 91, 146,
				159, 159, 91, 155, 155, 91, 156, 157, 159, 157,
				159, 159, 159, 94, 94, 78, 92, 169, 92, 168,
				159, 168, 25, 27, 30, 31, 32, 33, 139, 141,
				92, 138, 27, 55, 56, 74, 76, 78, 158, 143,
				143, 143, 143, 143, 143, 143, 168, 168, 143, 44,
				159, 151, 159, 168, 159, 155, 155, 168, 159, 168,
				159, 168, 168, 78, 78, 78, 92, 92, 168, 156,
				67, 68, 69, 70, 71, 77, 78, 79, 80, 81,
				84, 85, 89, 90, 91, 97, 160, 172, 173, 174,
				175, 178, 179, 159, 159, 91, 159, 168, 170, 171,
				91, 140, 140, 75, 76, 77, 89, 90, 161, 176,
				177, 180, 159, 168, 168, 168, 168, 168, 168, 168,
				92, 92, 168, 77, 92, 151, 159, 168, 91, 92,
				147, 159, 91, 152, 155, 168, 168, 92, 168, 168,
				168, 168, 168, 91, 163, 164, 165, 166, 167, 95,
				96, 95, 96, 159, 172, 174, 172, 173, 57, 58,
				59, 60, 61, 62, 63, 64, 65, 66, 79, 80,
				81, 82, 83, 84, 85, 86, 87, 88, 100, 91,
				160, 178, 171, 159, 168, 171, 170, 92, 92, 92,
				94, 94, 176, 156, 57, 58, 59, 60, 181, 177,
				92, 92, 92, 168, 92, 92, 168, 92, 168, 92,
				168, 168, 45, 92, 92, 155, 168, 92, 168, 92,
				168, 168, 168, 168, 168, 168, 55, 56, 92, 91,
				166, 91, 167, 78, 78, 91, 171, 92, 99, 98,
				174, 174, 174, 174, 174, 174, 174, 174, 174, 174,
				174, 174, 174, 174, 174, 174, 174, 174, 174, 174,
				172, 174, 92, 171, 168, 92, 170, 78, 78, 156,
				92, 178, 91, 92, 144, 145, 92, 144, 145, 92,
				168, 92, 123, 148, 92, 168, 92, 92, 168, 168,
				168, 168, 168, 78, 78, 95, 95, 26, 92, 173,
				96, 92, 92, 92, 168, 170, 92, 28, 29, 91,
				92, 145, 92, 92, 145, 92, 92, 92, 123, 92,
				168, 92, 168, 92, 78, 92, 78, 154, 154, 171,
				98, 173, 92, 170, 77, 176, 77, 176, 92, 92,
				92, 92, 92, 92, 92, 92, 170, 176, 92, 176,
				92, 91, 91, 92, 170, 92, 92, 56, 55, 170,
				78, 78, 170, 154, 154, 170, 92, 92, 170, 170 };

#define yyerrok ( yyerrstatus = 0 )
#define yyclearin ( yychar = YYEMPTY )
#define YYEMPTY ( -2 )
#define YYEOF 0

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab

/* Like YYERROR except do call yyerror.  This remains here temporarily
	 to ease the transition to the new meaning of YYERROR, for GCC.
	 Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL goto yyerrlab

#define YYRECOVERING() ( !!yyerrstatus )

#define YYBACKUP( Token, Value )                        \
	do                                                    \
		if ( yychar == YYEMPTY && yylen == 1 )              \
		{                                                   \
			yychar = ( Token );                               \
			yylval = ( Value );                               \
			yytoken = YYTRANSLATE( yychar );                  \
			YYPOPSTACK( 1 );                                  \
			goto yybackup;                                    \
		}                                                   \
		else                                                \
		{                                                   \
			yyerror( YY_( "syntax error: cannot back up" ) ); \
			YYERROR;                                          \
		}                                                   \
	while ( YYID( 0 ) )

#define YYTERROR 1
#define YYERRCODE 256

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
	 If N is 0, then set CURRENT to the empty location which ends
	 the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC( Rhs, K ) ( ( Rhs )[ K ] )
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT( Current, Rhs, N )                         \
	do                                                              \
		if ( YYID( N ) )                                              \
		{                                                             \
			( Current ).first_line = YYRHSLOC( Rhs, 1 ).first_line;     \
			( Current ).first_column = YYRHSLOC( Rhs, 1 ).first_column; \
			( Current ).last_line = YYRHSLOC( Rhs, N ).last_line;       \
			( Current ).last_column = YYRHSLOC( Rhs, N ).last_column;   \
		}                                                             \
		else                                                          \
		{                                                             \
			( Current ).first_line = ( Current ).last_line =            \
					YYRHSLOC( Rhs, 0 ).last_line;                           \
			( Current ).first_column = ( Current ).last_column =        \
					YYRHSLOC( Rhs, 0 ).last_column;                         \
		}                                                             \
	while ( YYID( 0 ) )
#endif

/* YY_LOCATION_PRINT -- Print the location on the stream.
	 This macro was not mandated originally: define only if we know
	 we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
#if YYLTYPE_IS_TRIVIAL
#define YY_LOCATION_PRINT( File, Loc )               \
	fprintf( File, "%d.%d-%d.%d",                      \
					 ( Loc ).first_line, ( Loc ).first_column, \
					 ( Loc ).last_line, ( Loc ).last_column )
#else
#define YY_LOCATION_PRINT( File, Loc ) ( (void)0 )
#endif
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
#define YYLEX yylex( YYLEX_PARAM )
#else
#define YYLEX yylex()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

#ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#define YYFPRINTF fprintf
#endif

#define YYDPRINTF( Args ) \
	do                      \
	{                       \
		if ( yydebug )        \
			YYFPRINTF Args;     \
	} while ( YYID( 0 ) )

#define YY_SYMBOL_PRINT( Title, Type, Value, Location ) \
	do                                                    \
	{                                                     \
		if ( yydebug )                                      \
		{                                                   \
			YYFPRINTF( stderr, "%s ", Title );                \
			yy_symbol_print( stderr,                          \
											 Type, Value );                   \
			YYFPRINTF( stderr, "\n" );                        \
		}                                                   \
	} while ( YYID( 0 ) )

/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static void
yy_symbol_value_print( FILE *yyoutput, int yytype, YYSTYPE const *const yyvaluep )
#else
static void
		yy_symbol_value_print( yyoutput, yytype, yyvaluep )
				FILE *yyoutput;
int yytype;
YYSTYPE const *const yyvaluep;
#endif
{
	if ( !yyvaluep )
		return;
#ifdef YYPRINT
	if ( yytype < YYNTOKENS )
		YYPRINT( yyoutput, yytoknum[ yytype ], *yyvaluep );
#else
	YYUSE( yyoutput );
#endif
	switch ( yytype )
	{
		default:
			break;
	}
}

/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static void
yy_symbol_print( FILE *yyoutput, int yytype, YYSTYPE const *const yyvaluep )
#else
static void
		yy_symbol_print( yyoutput, yytype, yyvaluep )
				FILE *yyoutput;
int yytype;
YYSTYPE const *const yyvaluep;
#endif
{
	if ( yytype < YYNTOKENS )
		YYFPRINTF( yyoutput, "token %s (", yytname[ yytype ] );
	else
		YYFPRINTF( yyoutput, "nterm %s (", yytname[ yytype ] );

	yy_symbol_value_print( yyoutput, yytype, yyvaluep );
	YYFPRINTF( yyoutput, ")" );
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static void
yy_stack_print( yytype_int16 *bottom, yytype_int16 *top )
#else
static void
		yy_stack_print( bottom, top )
				yytype_int16 *bottom;
yytype_int16 *top;
#endif
{
	YYFPRINTF( stderr, "Stack now" );
	for ( ; bottom <= top; ++bottom )
		YYFPRINTF( stderr, " %d", *bottom );
	YYFPRINTF( stderr, "\n" );
}

#define YY_STACK_PRINT( Bottom, Top )        \
	do                                         \
	{                                          \
		if ( yydebug )                           \
			yy_stack_print( ( Bottom ), ( Top ) ); \
	} while ( YYID( 0 ) )

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static void
yy_reduce_print( YYSTYPE *yyvsp, int yyrule )
#else
static void
		yy_reduce_print( yyvsp, yyrule )
				YYSTYPE *yyvsp;
int yyrule;
#endif
{
	int yynrhs = yyr2[ yyrule ];
	int yyi;
	unsigned long int yylno = yyrline[ yyrule ];
	YYFPRINTF( stderr, "Reducing stack by rule %d (line %lu):\n",
						 yyrule - 1, yylno );
	/* The symbols being reduced.  */
	for ( yyi = 0; yyi < yynrhs; yyi++ )
	{
		fprintf( stderr, "   $%d = ", yyi + 1 );
		yy_symbol_print( stderr, yyrhs[ yyprhs[ yyrule ] + yyi ],
										 &( yyvsp[ ( yyi + 1 ) - ( yynrhs ) ] ) );
		fprintf( stderr, "\n" );
	}
}

#define YY_REDUCE_PRINT( Rule )       \
	do                                  \
	{                                   \
		if ( yydebug )                    \
			yy_reduce_print( yyvsp, Rule ); \
	} while ( YYID( 0 ) )

/* Nonzero means print parse trace.  It is left uninitialized so that
	 multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
#define YYDPRINTF( Args )
#define YY_SYMBOL_PRINT( Title, Type, Value, Location )
#define YY_STACK_PRINT( Bottom, Top )
#define YY_REDUCE_PRINT( Rule )
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
	 if the built-in stack extension method is used).

	 Do not make this value too large; the results are undefined if
	 YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
	 evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#if YYERROR_VERBOSE

#ifndef yystrlen
#if defined __GLIBC__ && defined _STRING_H
#define yystrlen strlen
#else
/* Return the length of YYSTR.  */
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static YYSIZE_T
yystrlen( const char *yystr )
#else
static YYSIZE_T
		yystrlen( yystr )
				const char *yystr;
#endif
{
	YYSIZE_T yylen;
	for ( yylen = 0; yystr[ yylen ]; yylen++ )
		continue;
	return yylen;
}
#endif
#endif

#ifndef yystpcpy
#if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
#else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
	 YYDEST.  */
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static char *
yystpcpy( char *yydest, const char *yysrc )
#else
static char *
yystpcpy( yydest, yysrc )
char *yydest;
const char *yysrc;
#endif
{
	char *yyd = yydest;
	const char *yys = yysrc;

	while ( ( *yyd++ = *yys++ ) != '\0' )
		continue;

	return yyd - 1;
}
#endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
	 quotes and backslashes, so that it's suitable for yyerror.  The
	 heuristic is that double-quoting is unnecessary unless the string
	 contains an apostrophe, a comma, or backslash (other than
	 backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
	 null, do not copy; instead, return the length of what the result
	 would have been.  */
static YYSIZE_T
yytnamerr( char *yyres, const char *yystr )
{
	if ( *yystr == '"' )
	{
		YYSIZE_T yyn = 0;
		char const *yyp = yystr;

		for ( ;; )
			switch ( *++yyp )
			{
				case '\'':
				case ',':
					goto do_not_strip_quotes;

				case '\\':
					if ( *++yyp != '\\' )
						goto do_not_strip_quotes;
					/* Fall through.  */
				default:
					if ( yyres )
						yyres[ yyn ] = *yyp;
					yyn++;
					break;

				case '"':
					if ( yyres )
						yyres[ yyn ] = '\0';
					return yyn;
			}
	do_not_strip_quotes:;
	}

	if ( !yyres )
		return yystrlen( yystr );

	return yystpcpy( yyres, yystr ) - yyres;
}
#endif

/* Copy into YYRESULT an error message about the unexpected token
	 YYCHAR while in state YYSTATE.  Return the number of bytes copied,
	 including the terminating null byte.  If YYRESULT is null, do not
	 copy anything; just return the number of bytes that would be
	 copied.  As a special case, return 0 if an ordinary "syntax error"
	 message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
	 size calculation.  */
static YYSIZE_T
yysyntax_error( char *yyresult, int yystate, int yychar )
{
	int yyn = yypact[ yystate ];

	if ( !( YYPACT_NINF < yyn && yyn <= YYLAST ) )
		return 0;
	else
	{
		int yytype = YYTRANSLATE( yychar );
		YYSIZE_T yysize0 = yytnamerr( 0, yytname[ yytype ] );
		YYSIZE_T yysize = yysize0;
		YYSIZE_T yysize1;
		int yysize_overflow = 0;
		enum
		{
			YYERROR_VERBOSE_ARGS_MAXIMUM = 5
		};
		char const *yyarg[ YYERROR_VERBOSE_ARGS_MAXIMUM ];
		int yyx;

#if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
		char *yyfmt;
		char const *yyf;
		static char const yyunexpected[] = "syntax error, unexpected %s";
		static char const yyexpecting[] = ", expecting %s";
		static char const yyor[] = " or %s";
		char yyformat[ sizeof yyunexpected + sizeof yyexpecting - 1 + ( ( YYERROR_VERBOSE_ARGS_MAXIMUM - 2 ) * ( sizeof yyor - 1 ) ) ];
		char const *yyprefix = yyexpecting;

		/* Start YYX at -YYN if negative to avoid negative indexes in
 YYCHECK.  */
		int yyxbegin = yyn < 0 ? -yyn : 0;

		/* Stay within bounds of both yycheck and yytname.  */
		int yychecklim = YYLAST - yyn + 1;
		int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
		int yycount = 1;

		yyarg[ 0 ] = yytname[ yytype ];
		yyfmt = yystpcpy( yyformat, yyunexpected );

		for ( yyx = yyxbegin; yyx < yyxend; ++yyx )
			if ( yycheck[ yyx + yyn ] == yyx && yyx != YYTERROR )
			{
				if ( yycount == YYERROR_VERBOSE_ARGS_MAXIMUM )
				{
					yycount = 1;
					yysize = yysize0;
					yyformat[ sizeof yyunexpected - 1 ] = '\0';
					break;
				}
				yyarg[ yycount++ ] = yytname[ yyx ];
				yysize1 = yysize + yytnamerr( 0, yytname[ yyx ] );
				yysize_overflow |= ( yysize1 < yysize );
				yysize = yysize1;
				yyfmt = yystpcpy( yyfmt, yyprefix );
				yyprefix = yyor;
			}

		yyf = YY_( yyformat );
		yysize1 = yysize + yystrlen( yyf );
		yysize_overflow |= ( yysize1 < yysize );
		yysize = yysize1;

		if ( yysize_overflow )
			return YYSIZE_MAXIMUM;

		if ( yyresult )
		{
			/* Avoid sprintf, as that infringes on the user's name space.
				 Don't have undefined behavior even if the translation
				 produced a string with the wrong number of "%s"s.  */
			char *yyp = yyresult;
			int yyi = 0;
			while ( ( *yyp = *yyf ) != '\0' )
			{
				if ( *yyp == '%' && yyf[ 1 ] == 's' && yyi < yycount )
				{
					yyp += yytnamerr( yyp, yyarg[ yyi++ ] );
					yyf += 2;
				}
				else
				{
					yyp++;
					yyf++;
				}
			}
		}
		return yysize;
	}
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
static void
yydestruct( const char *yymsg, int yytype, YYSTYPE *yyvaluep )
#else
static void
		yydestruct( yymsg, yytype, yyvaluep )
				const char *yymsg;
int yytype;
YYSTYPE *yyvaluep;
#endif
{
	YYUSE( yyvaluep );

	if ( !yymsg )
		yymsg = "Deleting";
	YY_SYMBOL_PRINT( yymsg, yytype, yyvaluep, yylocationp );

	switch ( yytype )
	{

		default:
			break;
	}
}

/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse( void *YYPARSE_PARAM );
#else
int yyparse();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse( void );
#else
int yyparse();
#endif
#endif /* ! YYPARSE_PARAM */

/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
int yyparse( void *YYPARSE_PARAM )
#else
int
		yyparse( YYPARSE_PARAM ) void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if ( defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER )
int yyparse( void )
#else
int yyparse()

#endif
#endif
{

	int yystate;
	int yyn;
	int yyresult;
	/* Number of tokens to shift before error messages enabled.  */
	int yyerrstatus;
	/* Look-ahead token as an internal (translated) token number.  */
	int yytoken = 0;
#if YYERROR_VERBOSE
	/* Buffer for error messages, and its allocated size.  */
	char yymsgbuf[ 128 ];
	char *yymsg = yymsgbuf;
	YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

	/* Three stacks and their tools:
		 `yyss': related to states,
		 `yyvs': related to semantic values,
		 `yyls': related to locations.

		 Refer to the stacks thru separate pointers, to allow yyoverflow
		 to reallocate them elsewhere.  */

	/* The state stack.  */
	yytype_int16 yyssa[ YYINITDEPTH ];
	yytype_int16 *yyss = yyssa;
	yytype_int16 *yyssp;

	/* The semantic value stack.  */
	YYSTYPE yyvsa[ YYINITDEPTH ];
	YYSTYPE *yyvs = yyvsa;
	YYSTYPE *yyvsp;

#define YYPOPSTACK( N ) ( yyvsp -= ( N ), yyssp -= ( N ) )

	YYSIZE_T yystacksize = YYINITDEPTH;

	/* The variables used to return semantic value and location from the
		 action routines.  */
	YYSTYPE yyval;

	/* The number of symbols on the RHS of the reduced rule.
		 Keep to zero when no symbol should be popped.  */
	int yylen = 0;

	YYDPRINTF( ( stderr, "Starting parse\n" ) );

	yystate = 0;
	yyerrstatus = 0;
	yynerrs = 0;
	yychar = YYEMPTY; /* Cause a token to be read.  */

	/* Initialize stack pointers.
		 Waste one element of value and location stack
		 so that they stay on the same level as the state stack.
		 The wasted elements are never initialized.  */

	yyssp = yyss;
	yyvsp = yyvs;

	goto yysetstate;

	/*------------------------------------------------------------.
	| yynewstate -- Push a new state, which is found in yystate.  |
	`------------------------------------------------------------*/
yynewstate:
	/* In all cases, when you get here, the value and location stacks
		 have just been pushed.  So pushing a state here evens the stacks.  */
	yyssp++;

yysetstate:
	*yyssp = yystate;

	if ( yyss + yystacksize - 1 <= yyssp )
	{
		/* Get the current used size of the three stacks, in elements.  */
		YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
		{
			/* Give user a chance to reallocate the stack.  Use copies of
				 these so that the &'s don't force the real ones into
				 memory.  */
			YYSTYPE *yyvs1 = yyvs;
			yytype_int16 *yyss1 = yyss;

			/* Each stack pointer address is followed by the size of the
				 data in use in that stack, in bytes.  This used to be a
				 conditional around just the two extra args, but that might
				 be undefined if yyoverflow is a macro.  */
			yyoverflow( YY_( "memory exhausted" ),
									&yyss1, yysize * sizeof( *yyssp ),
									&yyvs1, yysize * sizeof( *yyvsp ),

									&yystacksize );

			yyss = yyss1;
			yyvs = yyvs1;
		}
#else /* no yyoverflow */
#ifndef YYSTACK_RELOCATE
		goto yyexhaustedlab;
#else
		/* Extend the stack our own way.  */
		if ( YYMAXDEPTH <= yystacksize )
			goto yyexhaustedlab;
		yystacksize *= 2;
		if ( YYMAXDEPTH < yystacksize )
			yystacksize = YYMAXDEPTH;

		{
			yytype_int16 *yyss1 = yyss;
			union yyalloc *yyptr =
					(union yyalloc *)YYSTACK_ALLOC( YYSTACK_BYTES( yystacksize ) );
			if ( !yyptr )
				goto yyexhaustedlab;
			YYSTACK_RELOCATE( yyss );
			YYSTACK_RELOCATE( yyvs );

#undef YYSTACK_RELOCATE
			if ( yyss1 != yyssa )
				YYSTACK_FREE( yyss1 );
		}
#endif
#endif /* no yyoverflow */

		yyssp = yyss + yysize - 1;
		yyvsp = yyvs + yysize - 1;

		YYDPRINTF( ( stderr, "Stack size increased to %lu\n",
								 (unsigned long int)yystacksize ) );

		if ( yyss + yystacksize - 1 <= yyssp )
			YYABORT;
	}

	YYDPRINTF( ( stderr, "Entering state %d\n", yystate ) );

	goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

	/* Do appropriate processing given the current state.  Read a
		 look-ahead token if we need one and don't already have one.  */

	/* First try to decide what to do without reference to look-ahead token.  */
	yyn = yypact[ yystate ];
	if ( yyn == YYPACT_NINF )
		goto yydefault;

	/* Not known => get a look-ahead token if don't already have one.  */

	/* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
	if ( yychar == YYEMPTY )
	{
		YYDPRINTF( ( stderr, "Reading a token: " ) );
		yychar = YYLEX;
	}

	if ( yychar <= YYEOF )
	{
		yychar = yytoken = YYEOF;
		YYDPRINTF( ( stderr, "Now at end of input.\n" ) );
	}
	else
	{
		yytoken = YYTRANSLATE( yychar );
		YY_SYMBOL_PRINT( "Next token is", yytoken, &yylval, &yylloc );
	}

	/* If the proper action on seeing token YYTOKEN is to reduce or to
		 detect an error, take that action.  */
	yyn += yytoken;
	if ( yyn < 0 || YYLAST < yyn || yycheck[ yyn ] != yytoken )
		goto yydefault;
	yyn = yytable[ yyn ];
	if ( yyn <= 0 )
	{
		if ( yyn == 0 || yyn == YYTABLE_NINF )
			goto yyerrlab;
		yyn = -yyn;
		goto yyreduce;
	}

	if ( yyn == YYFINAL )
		YYACCEPT;

	/* Count tokens shifted since error; after three, turn off error
		 status.  */
	if ( yyerrstatus )
		yyerrstatus--;

	/* Shift the look-ahead token.  */
	YY_SYMBOL_PRINT( "Shifting", yytoken, &yylval, &yylloc );

	/* Discard the shifted token unless it is eof.  */
	if ( yychar != YYEOF )
		yychar = YYEMPTY;

	yystate = yyn;
	*++yyvsp = yylval;

	goto yynewstate;

/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
	yyn = yydefact[ yystate ];
	if ( yyn == 0 )
		goto yyerrlab;
	goto yyreduce;

/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
	/* yyn is the number of a rule to reduce with.  */
	yylen = yyr2[ yyn ];

	/* If YYLEN is nonzero, implement the default value of the action:
		 `$$ = $1'.

		 Otherwise, the following line sets YYVAL to garbage.
		 This behavior is undocumented and Bison
		 users should not rely upon it.  Assigning to YYVAL
		 unconditionally makes the parser a bit smaller, and it avoids a
		 GCC warning that YYVAL may be used uninitialized.  */
	yyval = yyvsp[ 1 - yylen ];

	YY_REDUCE_PRINT( yyn );
	switch ( yyn )
	{
		case 3:
#line 97 "src/sdf_file.y"
		{
			sdf = (SdfFile *)( param );
			;
		}
		break;

		case 16:
#line 122 "src/sdf_file.y"
		{
			sdf->addVersion( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 17:
#line 127 "src/sdf_file.y"
		{
			sdf->addDesign( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 18:
#line 132 "src/sdf_file.y"
		{
			sdf->addDate( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 19:
#line 137 "src/sdf_file.y"
		{
			sdf->addVendor( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 20:
#line 142 "src/sdf_file.y"
		{
			sdf->addProgName( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 21:
#line 147 "src/sdf_file.y"
		{
			sdf->addProgVersion( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 22:
#line 152 "src/sdf_file.y"
		{
			sdf->addHierChar( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar )[ 0 ] );
			;
		}
		break;

		case 23:
#line 157 "src/sdf_file.y"
		{
			( yyval.ychar )[ 0 ] = '/';
			;
		}
		break;

		case 24:
#line 158 "src/sdf_file.y"
		{
			( yyval.ychar )[ 0 ] = '.';
			;
		}
		break;

		case 25:
#line 163 "src/sdf_file.y"
		{
			sdf->addVoltage( ( yyvsp[ ( 3 ) - ( 4 ) ].yvalue ) );
			;
		}
		break;

		case 26:
#line 164 "src/sdf_file.y"
		{
			SdfValue v;
			v.n = 1;
			v.v[ 0 ] = ( yyvsp[ ( 3 ) - ( 4 ) ].yfloat );
			sdf->addVoltage( v );
			;
		}
		break;

		case 27:
#line 172 "src/sdf_file.y"
		{
			sdf->addProcess( ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 28:
#line 176 "src/sdf_file.y"
		{
			sdf->addTemperature( ( yyvsp[ ( 3 ) - ( 4 ) ].yvalue ) );
			;
		}
		break;

		case 29:
#line 177 "src/sdf_file.y"
		{
			SdfValue v;
			v.n = 1;
			v.v[ 0 ] = ( yyvsp[ ( 3 ) - ( 4 ) ].yfloat );
			sdf->addTemperature( v );
			;
		}
		break;

		case 31:
#line 190 "src/sdf_file.y"
		{
			sdf->addTimeScale( ( yyvsp[ ( 1 ) - ( 2 ) ].yfloat ), ( yyvsp[ ( 2 ) - ( 2 ) ].ychar ) );
			;
		}
		break;

		case 36:
#line 207 "src/sdf_file.y"
		{
			sdf->addCell( ( yyvsp[ ( 1 ) - ( 2 ) ].ychar ), ( yyvsp[ ( 2 ) - ( 2 ) ].ychar ) );
			;
		}
		break;

		case 37:
#line 212 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 38:
#line 217 "src/sdf_file.y"
		{
			( yyval.ychar )[ 0 ] = '\0';
			;
		}
		break;

		case 39:
#line 218 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 40:
#line 219 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 41:
#line 220 "src/sdf_file.y"
		{
			( yyval.ychar )[ 0 ] = '*';
			( yyval.ychar )[ 1 ] = '\0';
			;
		}
		break;

		case 60:
#line 283 "src/sdf_file.y"
		{
			( yyval.ydelayType ) = DELAY_ABSOLUTE;
			;
		}
		break;

		case 61:
#line 284 "src/sdf_file.y"
		{
			( yyval.ydelayType ) = DELAY_INCREMENT;
			;
		}
		break;

		case 66:
#line 297 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = ( yyvsp[ ( 1 ) - ( 2 ) ].ydelayDef );
			( yyvsp[ ( 1 ) - ( 2 ) ].ydelayDef )->next = ( yyvsp[ ( 2 ) - ( 2 ) ].ydelayDef );
			( yyvsp[ ( 2 ) - ( 2 ) ].ydelayDef )->head = ( yyvsp[ ( 1 ) - ( 2 ) ].ydelayDef )->head;
			;
		}
		break;

		case 67:
#line 302 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = ( yyvsp[ ( 1 ) - ( 1 ) ].ydelayDef );
			( yyval.ydelayDef )->head = ( yyval.ydelayDef );
			( yyval.ydelayDef )->next = NULL;
			;
		}
		break;

		case 68:
#line 311 "src/sdf_file.y"
		{
			strcpy( ( yyval.yioPath ).in, ( yyvsp[ ( 1 ) - ( 2 ) ].ychar ) );
			strcpy( ( yyval.yioPath ).out, ( yyvsp[ ( 2 ) - ( 2 ) ].ychar ) );
			;
		}
		break;

		case 69:
#line 317 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = ( yyvsp[ ( 1 ) - ( 1 ) ].ydelayDef );
			;
		}
		break;

		case 70:
#line 320 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = ( yyvsp[ ( 3 ) - ( 4 ) ].ydelayDef );
			;
		}
		break;

		case 71:
#line 323 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = ( yyvsp[ ( 3 ) - ( 4 ) ].ydelayDef );
			;
		}
		break;

		case 72:
#line 326 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::PORT_DELAY;
			strcpy( ( yyval.ydelayDef )->p1, ( yyvsp[ ( 3 ) - ( 5 ) ].ychar ) );
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 4 ) - ( 5 ) ].ydelayValueList );
			;
		}
		break;

		case 73:
#line 332 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::INTER_DELAY;
			strcpy( ( yyval.ydelayDef )->p1, ( yyvsp[ ( 3 ) - ( 6 ) ].ychar ) );
			strcpy( ( yyval.ydelayDef )->p2, ( yyvsp[ ( 4 ) - ( 6 ) ].ychar ) );
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 5 ) - ( 6 ) ].ydelayValueList );
			;
		}
		break;

		case 74:
#line 339 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::DEVICE_DELAY;
			( yyval.ydelayDef )->p1[ 0 ] = '\0';
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 3 ) - ( 4 ) ].ydelayValueList );
			;
		}
		break;

		case 75:
#line 345 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::DEVICE_DELAY;
			strcpy( ( yyval.ydelayDef )->p1, ( yyvsp[ ( 3 ) - ( 5 ) ].ychar ) );
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 4 ) - ( 5 ) ].ydelayValueList );
			;
		}
		break;

		case 78:
#line 361 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::IO_DELAY;
			( yyval.ydelayDef )->pspec = ( yyvsp[ ( 3 ) - ( 6 ) ].yportSpec );
			strcpy( ( yyval.ydelayDef )->p1, ( yyvsp[ ( 4 ) - ( 6 ) ].ychar ) );
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 5 ) - ( 6 ) ].ydelayValueList );
			;
		}
		break;

		case 79:
#line 368 "src/sdf_file.y"
		{
			( yyval.ydelayDef ) = new SdfDelayDef;
			( yyval.ydelayDef )->type = SdfDelayDef::IO_RETAIN;
			( yyval.ydelayDef )->pspec = ( yyvsp[ ( 3 ) - ( 9 ) ].yportSpec );
			strcpy( ( yyval.ydelayDef )->p1, ( yyvsp[ ( 4 ) - ( 9 ) ].ychar ) );
			( yyval.ydelayDef )->vlist = ( yyvsp[ ( 7 ) - ( 9 ) ].ydelayValueList );
			;
		}
		break;

		case 136:
#line 493 "src/sdf_file.y"
		{
			( yyval.yportSpec ).type = SdfPortSpec::EDGE_NA;
			strcpy( ( yyval.yportSpec ).port, ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 137:
#line 495 "src/sdf_file.y"
		{
			( yyval.yportSpec ).type = ( yyvsp[ ( 1 ) - ( 1 ) ].yportSpec ).type;
			strcpy( ( yyval.yportSpec ).port, ( yyval.yportSpec ).port );
			;
		}
		break;

		case 138:
#line 501 "src/sdf_file.y"
		{
			( yyval.yportSpec ).type = ( yyvsp[ ( 2 ) - ( 4 ) ].yportSpec ).type;
			strcpy( ( yyval.yportSpec ).port, ( yyvsp[ ( 3 ) - ( 4 ) ].ychar ) );
			;
		}
		break;

		case 139:
#line 507 "src/sdf_file.y"
		{
			( yyval.yportSpec ).type = SdfPortSpec::EDGE_01;
			;
		}
		break;

		case 140:
#line 508 "src/sdf_file.y"
		{
			( yyval.yportSpec ).type = SdfPortSpec::EDGE_10;
			;
		}
		break;

		case 141:
#line 509 "src/sdf_file.y"
		{
			if ( !strcmp( "0z", ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) ) )
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_0Z;
			else
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_1Z;
			;
		}
		break;

		case 142:
#line 513 "src/sdf_file.y"
		{
			if ( !strcmp( "z0", ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) ) )
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_Z0;
			else
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_Z1;
			;
		}
		break;

		case 143:
#line 517 "src/sdf_file.y"
		{
			if ( ( yyvsp[ ( 1 ) - ( 1 ) ].yfloat ) < 10.0 )
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_01;
			else
				( yyval.yportSpec ).type = SdfPortSpec::EDGE_10;
			;
		}
		break;

		case 144:
#line 525 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 145:
#line 529 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 146:
#line 530 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 147:
#line 535 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 148:
#line 536 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 1 ) ].ychar ) );
			;
		}
		break;

		case 149:
#line 537 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 4 ) ].ychar ) );
			strcat( ( yyval.ychar ), "[" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 3 ) - ( 4 ) ].yfloat ) );
			strcat( ( yyval.ychar ), "]" );
			;
		}
		break;

		case 150:
#line 541 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 4 ) ].ychar ) );
			strcat( ( yyval.ychar ), "[" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 3 ) - ( 4 ) ].yfloat ) );
			strcat( ( yyval.ychar ), "]" );
			;
		}
		break;

		case 151:
#line 549 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 6 ) ].ychar ) );
			strcat( ( yyval.ychar ), "[" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 3 ) - ( 6 ) ].yfloat ) );
			strcat( ( yyval.ychar ), ":" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 5 ) - ( 6 ) ].yfloat ) );
			strcat( ( yyval.ychar ), "]" );
			;
		}
		break;

		case 152:
#line 557 "src/sdf_file.y"
		{
			strcpy( ( yyval.ychar ), ( yyvsp[ ( 1 ) - ( 6 ) ].ychar ) );
			strcat( ( yyval.ychar ), "[" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 3 ) - ( 6 ) ].yfloat ) );
			strcat( ( yyval.ychar ), ":" );
			sprintf( &( yyval.ychar )[ strlen( ( yyval.ychar ) ) ], "%d", (int)( yyvsp[ ( 5 ) - ( 6 ) ].yfloat ) );
			strcat( ( yyval.ychar ), "]" );
			;
		}
		break;

		case 161:
#line 597 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 1;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 2 ) - ( 3 ) ].yfloat );
			;
		}
		break;

		case 162:
#line 599 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = ( yyvsp[ ( 2 ) - ( 3 ) ].yvalue ).n;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 2 ) - ( 3 ) ].yvalue ).v[ 0 ];
			( yyval.yvalue ).v[ 1 ] = ( yyvsp[ ( 2 ) - ( 3 ) ].yvalue ).v[ 1 ];
			( yyval.yvalue ).v[ 2 ] = ( yyvsp[ ( 2 ) - ( 3 ) ].yvalue ).v[ 2 ];
			;
		}
		break;

		case 163:
#line 603 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 0;
			;
		}
		break;

		case 164:
#line 608 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 1 ) - ( 3 ) ].yfloat );
			( yyval.yvalue ).v[ 1 ] = 0;
			( yyval.yvalue ).v[ 2 ] = 0;
			;
		}
		break;

		case 165:
#line 612 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 1 ) - ( 4 ) ].yfloat );
			( yyval.yvalue ).v[ 1 ] = ( yyvsp[ ( 3 ) - ( 4 ) ].yfloat );
			( yyval.yvalue ).v[ 2 ] = 0;
			;
		}
		break;

		case 166:
#line 616 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 1 ) - ( 4 ) ].yfloat );
			( yyval.yvalue ).v[ 1 ] = 0;
			( yyval.yvalue ).v[ 2 ] = ( yyvsp[ ( 4 ) - ( 4 ) ].yfloat );
			;
		}
		break;

		case 167:
#line 620 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = 0;
			( yyval.yvalue ).v[ 1 ] = ( yyvsp[ ( 2 ) - ( 3 ) ].yfloat );
			( yyval.yvalue ).v[ 2 ] = 0;
			;
		}
		break;

		case 168:
#line 624 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = 0;
			( yyval.yvalue ).v[ 1 ] = ( yyvsp[ ( 2 ) - ( 4 ) ].yfloat );
			( yyval.yvalue ).v[ 2 ] = ( yyvsp[ ( 4 ) - ( 4 ) ].yfloat );
			;
		}
		break;

		case 169:
#line 628 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = 0;
			( yyval.yvalue ).v[ 1 ] = 0;
			( yyval.yvalue ).v[ 2 ] = ( yyvsp[ ( 3 ) - ( 3 ) ].yfloat );
			;
		}
		break;

		case 170:
#line 632 "src/sdf_file.y"
		{
			( yyval.yvalue ).n = 3;
			( yyval.yvalue ).v[ 0 ] = ( yyvsp[ ( 1 ) - ( 5 ) ].yfloat );
			( yyval.yvalue ).v[ 1 ] = ( yyvsp[ ( 3 ) - ( 5 ) ].yfloat );
			( yyval.yvalue ).v[ 2 ] = ( yyvsp[ ( 5 ) - ( 5 ) ].yfloat );
			;
		}
		break;

		case 171:
#line 640 "src/sdf_file.y"
		{
			( yyval.ydelayValue ).n = 1;
			sdfValueCopy( ( yyval.ydelayValue ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 1 ) ].yvalue ) );
			;
		}
		break;

		case 172:
#line 642 "src/sdf_file.y"
		{
			( yyval.ydelayValue ).n = 2;
			sdfValueCopy( ( yyval.ydelayValue ).v[ 0 ], ( yyvsp[ ( 2 ) - ( 4 ) ].yvalue ) );
			sdfValueCopy( ( yyval.ydelayValue ).v[ 1 ], ( yyvsp[ ( 3 ) - ( 4 ) ].yvalue ) );
			;
		}
		break;

		case 173:
#line 645 "src/sdf_file.y"
		{
			( yyval.ydelayValue ).n = 3;
			sdfValueCopy( ( yyval.ydelayValue ).v[ 0 ], ( yyvsp[ ( 2 ) - ( 5 ) ].yvalue ) );
			sdfValueCopy( ( yyval.ydelayValue ).v[ 0 ], ( yyvsp[ ( 3 ) - ( 5 ) ].yvalue ) );
			sdfValueCopy( ( yyval.ydelayValue ).v[ 1 ], ( yyvsp[ ( 4 ) - ( 5 ) ].yvalue ) );
			;
		}
		break;

		case 174:
#line 653 "src/sdf_file.y"
		{
			( yyval.ydelayValueList ).n = 1;
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 1 ) ].ydelayValue ) );
			;
		}
		break;

		case 175:
#line 657 "src/sdf_file.y"
		{
			( yyval.ydelayValueList ).n = 2;
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 2 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 1 ], ( yyvsp[ ( 1 ) - ( 2 ) ].ydelayValue ) );
			;
		}
		break;

		case 176:
#line 662 "src/sdf_file.y"
		{
			( yyval.ydelayValueList ).n = 3;
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 3 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 1 ], ( yyvsp[ ( 2 ) - ( 3 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 2 ], ( yyvsp[ ( 3 ) - ( 3 ) ].ydelayValue ) );
			;
		}
		break;

		case 177:
#line 668 "src/sdf_file.y"
		{
			( yyval.ydelayValueList ).n = 6;
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 6 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 1 ], ( yyvsp[ ( 2 ) - ( 6 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 2 ], ( yyvsp[ ( 3 ) - ( 6 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 3 ], ( yyvsp[ ( 4 ) - ( 6 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 4 ], ( yyvsp[ ( 5 ) - ( 6 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 5 ], ( yyvsp[ ( 6 ) - ( 6 ) ].ydelayValue ) );
			;
		}
		break;

		case 178:
#line 677 "src/sdf_file.y"
		{
			( yyval.ydelayValueList ).n = 12;
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 0 ], ( yyvsp[ ( 1 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 1 ], ( yyvsp[ ( 2 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 2 ], ( yyvsp[ ( 3 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 3 ], ( yyvsp[ ( 4 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 4 ], ( yyvsp[ ( 5 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 5 ], ( yyvsp[ ( 6 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 6 ], ( yyvsp[ ( 7 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 7 ], ( yyvsp[ ( 8 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 8 ], ( yyvsp[ ( 9 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 9 ], ( yyvsp[ ( 10 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 10 ], ( yyvsp[ ( 11 ) - ( 12 ) ].ydelayValue ) );
			sdfDelayValueCopy( ( yyval.ydelayValueList ).v[ 11 ], ( yyvsp[ ( 12 ) - ( 12 ) ].ydelayValue ) );
			;
		}
		break;

/* Line 1267 of yacc.c.  */
#line 2567 "lib/opt/lex_n_yacc/sdf_file.tab.cpp"
		default:
			break;
	}
	YY_SYMBOL_PRINT( "-> $$ =", yyr1[ yyn ], &yyval, &yyloc );

	YYPOPSTACK( yylen );
	yylen = 0;
	YY_STACK_PRINT( yyss, yyssp );

	*++yyvsp = yyval;

	/* Now `shift' the result of the reduction.  Determine what state
		 that goes to, based on the state we popped back to and the rule
		 number reduced by.  */

	yyn = yyr1[ yyn ];

	yystate = yypgoto[ yyn - YYNTOKENS ] + *yyssp;
	if ( 0 <= yystate && yystate <= YYLAST && yycheck[ yystate ] == *yyssp )
		yystate = yytable[ yystate ];
	else
		yystate = yydefgoto[ yyn - YYNTOKENS ];

	goto yynewstate;

/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
	/* If not already recovering from an error, report this error.  */
	if ( !yyerrstatus )
	{
		++yynerrs;
#if !YYERROR_VERBOSE
		yyerror( YY_( "syntax error" ) );
#else
		{
			YYSIZE_T yysize = yysyntax_error( 0, yystate, yychar );
			if ( yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM )
			{
				YYSIZE_T yyalloc = 2 * yysize;
				if ( !( yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM ) )
					yyalloc = YYSTACK_ALLOC_MAXIMUM;
				if ( yymsg != yymsgbuf )
					YYSTACK_FREE( yymsg );
				yymsg = (char *)YYSTACK_ALLOC( yyalloc );
				if ( yymsg )
					yymsg_alloc = yyalloc;
				else
				{
					yymsg = yymsgbuf;
					yymsg_alloc = sizeof yymsgbuf;
				}
			}

			if ( 0 < yysize && yysize <= yymsg_alloc )
			{
				(void)yysyntax_error( yymsg, yystate, yychar );
				yyerror( yymsg );
			}
			else
			{
				yyerror( YY_( "syntax error" ) );
				if ( yysize != 0 )
					goto yyexhaustedlab;
			}
		}
#endif
	}

	if ( yyerrstatus == 3 )
	{
		/* If just tried and failed to reuse look-ahead token after an
 error, discard it.  */

		if ( yychar <= YYEOF )
		{
			/* Return failure if at end of input.  */
			if ( yychar == YYEOF )
				YYABORT;
		}
		else
		{
			yydestruct( "Error: discarding",
									yytoken, &yylval );
			yychar = YYEMPTY;
		}
	}

	/* Else will try to reuse look-ahead token after shifting the error
		 token.  */
	goto yyerrlab1;

/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

	/* Pacify compilers like GCC when the user code never invokes
		 YYERROR and the label yyerrorlab therefore never appears in user
		 code.  */
	if ( /*CONSTCOND*/ 0 )
		goto yyerrorlab;

	/* Do not reclaim the symbols of the rule which action triggered
		 this YYERROR.  */
	YYPOPSTACK( yylen );
	yylen = 0;
	YY_STACK_PRINT( yyss, yyssp );
	yystate = *yyssp;
	goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
	yyerrstatus = 3; /* Each real token shifted decrements this.  */

	for ( ;; )
	{
		yyn = yypact[ yystate ];
		if ( yyn != YYPACT_NINF )
		{
			yyn += YYTERROR;
			if ( 0 <= yyn && yyn <= YYLAST && yycheck[ yyn ] == YYTERROR )
			{
				yyn = yytable[ yyn ];
				if ( 0 < yyn )
					break;
			}
		}

		/* Pop the current state because it cannot handle the error token.  */
		if ( yyssp == yyss )
			YYABORT;

		yydestruct( "Error: popping",
								yystos[ yystate ], yyvsp );
		YYPOPSTACK( 1 );
		yystate = *yyssp;
		YY_STACK_PRINT( yyss, yyssp );
	}

	if ( yyn == YYFINAL )
		YYACCEPT;

	*++yyvsp = yylval;

	/* Shift the error token.  */
	YY_SYMBOL_PRINT( "Shifting", yystos[ yyn ], yyvsp, yylsp );

	yystate = yyn;
	goto yynewstate;

/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
	yyresult = 0;
	goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
	yyresult = 1;
	goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
	yyerror( YY_( "memory exhausted" ) );
	yyresult = 2;
	/* Fall through.  */
#endif

yyreturn:
	if ( yychar != YYEOF && yychar != YYEMPTY )
		yydestruct( "Cleanup: discarding lookahead",
								yytoken, &yylval );
	/* Do not reclaim the symbols of the rule which action triggered
		 this YYABORT or YYACCEPT.  */
	YYPOPSTACK( yylen );
	YY_STACK_PRINT( yyss, yyssp );
	while ( yyssp != yyss )
	{
		yydestruct( "Cleanup: popping",
								yystos[ *yyssp ], yyvsp );
		YYPOPSTACK( 1 );
	}
#ifndef yyoverflow
	if ( yyss != yyssa )
		YYSTACK_FREE( yyss );
#endif
#if YYERROR_VERBOSE
	if ( yymsg != yymsgbuf )
		YYSTACK_FREE( yymsg );
#endif
	/* Make sure YYID is used.  */
	return YYID( yyresult );
}

#line 790 "src/sdf_file.y"

void sdf_fileerror( const char *msg )
{
	fprintf( stderr, "**ERROR sdf_fileerror(): " );
	fprintf( stderr, "At line %d. Near `%s'. ", sdf_fileline, sdf_filetext );
	fprintf( stderr, "%s\n", msg );
}
