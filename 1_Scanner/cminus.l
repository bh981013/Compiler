%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
newline     \n
whitespace  [ \t]+

%%
"/*"             { char c;
                    int ready = 0;
                  do
                  { c = input();
                    if (c == '\n') lineno++;
                    if (c == '*') ready = 1;
                    else if(ready==1 && c != '/') ready = 0;
                  } while (ready!=1 || c!='/');
                }
"int"            {return INT;}
"void"          {return VOID;}
"if"          {return IF;}
"else"           {return ELSE;}
"while"        {return WHILE;}
"return"         {return RETURN;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"<="             {return LE;}
"<"             {return LT;}
">="             {return GE;}
">"             {return GT;}
"=="            {return EQ;}
"="             {return ASSIGN;}
"!="             {return NE;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"{"             {return LCURLY;}
"}"             {return RCURLY;}
"["             {return LBRACE;}
"]"             {return RBRACE;}
";"             {return SEMI;}
","             {return COMMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}

.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

