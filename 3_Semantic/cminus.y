/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *

#define STACK_SIZE 100
static int top = -1;

static char * savedName[STACK_SIZE]; /* for use in assignments */
static int num;
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 zcvxx to ensure no conflict with lex
static int yyerror(char* message);

%}

%token INT VOID WHILE RETURN
%token ID NUM
%token SEMI COMMA ASSIGN
%token LE LT GE GT EQ NE
%left PLUS MINUS
%left TIMES OVER
%token LBRACE RBRACE LCURLY RCURLY LPAREN RPAREN
%token ERROR
%token IF
%nonassoc then
%nonassoc ELSE

%% /* Grammar for C-MINUS */

program: declaration_list
                 { savedTree = $1;} 
            ;

declaration_list: declaration_list declaration
                    { YYSTYPE t = $1;
                      if(t != NULL)
                      {
                        while (t->sibling != NULL)
                            t = t->sibling;
                        t->sibling = $2;
                        $$ = $1; 
                      }
                        else $$ = $2;
                    } 
                | declaration{$$ = $1;}
                ;

declaration: var_declaration{$$ = $1;}
            | fun_declaration{$$ = $1;}
            ;
GETID: ID
        {
        savedName[++top] = copyString(tokenString);
        savedLineNo = lineno;
        }
        ;

GETNUM: NUM{ 
            num = atoi(tokenString);
            savedLineNo = lineno;
            }
            ;
var_declaration: type_specifier GETID SEMI
                  {
                    $$ = newNode(VarDK);
                    $$->type = $1->type;
                    $$->name = savedName[top--];
                    $$->lineno = lineno;
                  }
                  |
                  type_specifier GETID LBRACE GETNUM RBRACE SEMI{
                    $$ = newNode(VarDK);
                    $$->type = ($1->type) + 2;
                    $$->name = savedName[top--];
                    YYSTYPE newN = newNode(ConstK);
                    newN -> num = num;
                    $$->child[0] = newN;
                  }
                  ;
type_specifier: INT{
                  $$ = newNode(TypeK);
                  $$->type = Integer;
                  }
                  |
                VOID{
                  $$ = newNode(TypeK);
                  $$-> type = Void;
                  }
                  ;
fun_declaration: type_specifier GETID
                  {
                    $$ = newNode(FunK);
                    $$->type = $1->type;
                    $$->name = savedName[top--];
                    $$->lineno = lineno;
                  } LPAREN params RPAREN compound_stmt{
                    $$ = $3;
                    $$->child[0] = $5;
                    $$->child[1] = $7;
                  }
                  ;

params: params_list{
          $$ = $1;
        }
        |
        VOID{
          $$ = newNode(paramsK);
          $$->type = Void;
        }
        ;
params_list: params_list COMMA param{ 
                      YYSTYPE t = $1;
                      if(t != NULL)
                      {
                        while (t->sibling != NULL)
                        t = t->sibling;
                        t->sibling = $3;
                        $$ = $1; 
                      }
                        else $$ = $3;
                    } 
              |
              param{
                  $$ = $1;
              }
              ;
param: type_specifier GETID{
          $$ = newNode(paraK);
          $$->type = $1->type;
          $$->name = savedName[top--];
        }
        |
        type_specifier GETID LBRACE RBRACE{
          $$ = newNode(paraK);
          $$->type = ($1->type) + 2;
          $$ ->name = savedName[top--];
        }
        ;
compound_stmt: LCURLY local_declarations statement_list RCURLY
              {
                $$ = newNode(CompK);
                $$->child[0] = $2;
                $$->child[1] = $3;
              }
              ;
local_declarations: local_declarations var_declaration{ 
                      YYSTYPE t = $1;
                      if(t != NULL)
                      {
                        while (t->sibling != NULL)
                          t = t->sibling;
                        t->sibling = $2;
                        $$ = $1; 
                      }
                        else $$ = $2;
                    }
                  |{$$ = NULL;}
                  ;
statement_list: statement_list statement{ 
                      YYSTYPE t = $1;
                      if(t != NULL)
                      {
                        while (t->sibling != NULL)
                        t = t->sibling;
                        t->sibling = $2;
                        $$ = $1; 
                      }
                        else $$ = $2;
                    }
                    |{$$ = NULL;}
                    ;
statement:expression_stmt{$$ = $1;}
          | compound_stmt{$$ = $1;}
          | selection_stmt{$$ = $1;}
          | iteration_stmt{$$ = $1;}
          | return_stmt{$$ = $1;}
          ;
expression_stmt: expression SEMI{$$ = $1;}
                | SEMI{$$ = NULL;}
                ;
selection_stmt: IF LPAREN expression RPAREN statement %prec then{
                  $$ = newNode(IfK);
                  $$->child[0] = $3;
                  $$->child[1] = $5;
                }                
                |
                IF LPAREN expression RPAREN statement ELSE statement{
                  $$ = newNode(IfelseK);
                  $$->child[0] = $3;
                  $$->child[1] = $5;
                  $$->child[2] = $7;
                }
                ;
iteration_stmt: WHILE LPAREN expression RPAREN statement{
                  $$ = newNode(WhileK);
                  $$->child[0] = $3;
                  $$->child[1] = $5;
              }
              ;
return_stmt: RETURN SEMI{
                $$ = newNode(RetNK);
                $$->type = Void;
              }
              |
              RETURN expression SEMI{
                $$ = newNode(RetK);
                $$ ->child[0] = $2;
              }
              ;
expression: var ASSIGN expression
            {
              $$ = newNode(ExpK);
              $$->child[0] = $1;
              $$->child[1] = $3;
            }
            |
            simple_expression
            {
              $$ = $1;
            }
            ;
var: GETID{
      $$ = newNode(VarK);
      $$->name = savedName[top--];
      }
    |
    GETID{
    } LBRACE expression RBRACE{ 
      $$ = newNode(VarAK);
      $$->name = savedName[top--];
      $$->child[0] = $4;
    }
    ;
simple_expression: addictive_expr relop  addictive_expr{
                    $$ = newNode(OpK);
                    $$->op = $2->op;
                    $$->child[0] = $1;
                    $$->child[1] = $3;
                  }
                  |
                   addictive_expr{$$ = $1;}
                  ;
relop:  LE{$$ = newNode(OpK);
           $$->op = LE;}
      | LT{$$ = newNode(OpK);
           $$->op = LT;}
      | GT{$$ = newNode(OpK);
           $$->op = GT;}
      | GE{$$ = newNode(OpK);
           $$->op = GE;}
      | EQ{$$ = newNode(OpK); 
            $$->op = EQ;}
      | NE{$$ = newNode(OpK); 
          $$->op = NE;}
      ;
addictive_expr: addictive_expr addop term{
                $$ = newNode(OpK);
                $$->op = $2->op;
                $$->child[0] = $1;
                $$->child[1] = $3;
                }
                |
                term{$$ = $1;}
                ;
addop: PLUS{$$ = newNode(OpK); $$->op = PLUS;}
      | MINUS{$$ = newNode(OpK); $$->op = MINUS;}
      ;
term: term mulop factor{
        $$ = newNode(OpK);
        $$->op = $2->op;
        $$->child[0] = $1;
        $$->child[1] = $3;
      }
      |
      factor{$$ = $1;}
      ;
mulop:  TIMES{$$ = newNode(OpK); $$->op = TIMES;}
      | OVER{$$ = newNode(OpK); $$->op = OVER;}
      ;
factor: LPAREN expression RPAREN{$$ = $2;}
      |
      var{$$ = $1;}
      |
      call{$$ = $1;}
      |
      GETNUM{$$ = newNode(ConstK);
          $$->num =num;}
      ;
call: GETID{
        $$ = newNode(CallK);
        $$->name = savedName[top--];
  } LPAREN args RPAREN{
      $$ = $2;
      $$->child[0] = $4;
      }
      ;
args: arg_list{
        $$ = $1;
        } 
        | 
        {$$= NULL;}
        ; 

arg_list: arg_list COMMA expression{
              YYSTYPE t = $1;
              if(t != NULL)
              {
                while (t->sibling != NULL)
                  t = t->sibling;
                t->sibling = $3;
                $$ = $1; 
              }
                else $$ = $3;
          }
          |
          expression{$$ = $1;}
          ;
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
*/

static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

