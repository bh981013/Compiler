/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT = 258,
    VOID = 259,
    WHILE = 260,
    RETURN = 261,
    ID = 262,
    NUM = 263,
    SEMI = 264,
    COMMA = 265,
    ASSIGN = 266,
    LE = 267,
    LT = 268,
    GE = 269,
    GT = 270,
    EQ = 271,
    NE = 272,
    PLUS = 273,
    MINUS = 274,
    TIMES = 275,
    OVER = 276,
    LBRACE = 277,
    RBRACE = 278,
    LCURLY = 279,
    RCURLY = 280,
    LPAREN = 281,
    RPAREN = 282,
    ERROR = 283,
    IF = 284,
    then = 285,
    ELSE = 286
  };
#endif
/* Tokens.  */
#define INT 258
#define VOID 259
#define WHILE 260
#define RETURN 261
#define ID 262
#define NUM 263
#define SEMI 264
#define COMMA 265
#define ASSIGN 266
#define LE 267
#define LT 268
#define GE 269
#define GT 270
#define EQ 271
#define NE 272
#define PLUS 273
#define MINUS 274
#define TIMES 275
#define OVER 276
#define LBRACE 277
#define RBRACE 278
#define LCURLY 279
#define RCURLY 280
#define LPAREN 281
#define RPAREN 282
#define ERROR 283
#define IF 284
#define then 285
#define ELSE 286

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
