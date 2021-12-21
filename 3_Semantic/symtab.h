/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
#define SIZE 1000


static int scopeNum = 0;
static int scopeTop = 1;
static int funcNum = 0;


typedef struct LineListRec
{ int lineno;
    struct LineListRec * next;
} * LineList;

typedef struct BucketListRec
{ char * name;
  Type type;
  LineList lines;
  int memloc ; /* memory location for variable */
  struct BucketListRec * next;
  int size;
} * BucketList;

typedef struct ScopeListRec{
  char* name;
  BucketList bucket[SIZE];
  int bucketNum;
  struct ScopeListRec* parent;
} * ScopeList;



typedef struct ParameterRec{
    char* name;
    Type type;
}* Parameter;

typedef struct FuncListRec{
    char* name;
    Type type;
    Parameter parameter[SIZE];
    int numParameter;
    int voidParameter;
}* FuncList;

static ScopeList scopeList[SIZE];
static FuncList funcList[SIZE];

void st_var_insert( char * scope, char* name, int lineno);
ScopeList makeScope(char* name, ScopeList parent);
ScopeList find_scope(char* scope);
FuncList find_func(char* name);
void st_insert( char * scope, char* name, Type type, int lineno, int loc );
void add_func(char* name, Type Type);
void add_parameter(char* funcName, char* name, Type type);
/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * scope, char* name );
BucketList st_lookup_excluding_parent(char* scope, char* name);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
