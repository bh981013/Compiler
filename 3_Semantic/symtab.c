/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
char* types[] = {"void", "int", "void[]", "int[]"};
/* SIZE is the size of the hash table */


/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */

ScopeList makeScope(char* name, ScopeList parent){
  ScopeList scope = malloc(sizeof(ScopeList));
  scope->name = malloc(sizeof(name));
  strcpy(scope->name,name);
  scope->parent = parent;
  scope->bucketNum = 0;
  scopeList[scopeNum++] = scope;
  return scope;
}

ScopeList find_scope(char* scope){
  for(int i = 0; i<scopeNum; i++){
    if(strcmp(scopeList[i]->name,scope) == 0){
      return scopeList[i];
    }
  }
}
FuncList find_func(char* name){
  for(int i = 0; i<funcNum; i++){
    if(strcmp(funcList[i]->name,name) == 0){
      return funcList[i];
    }
  }
  return NULL;
}

void add_func(char* name, Type type){
  FuncList f = malloc(sizeof(FuncList));
  f->name = name;
  f->type = type;
  f->numParameter = 0;
  f->voidParameter = 0;
  funcList[funcNum++] = f;
}

void add_parameter(char* funcName, char* name, Type type){
  FuncList f = find_func(funcName);
  int n= f->numParameter;
  f->parameter[n] = malloc(sizeof(Parameter));
  f->parameter[n]->name = name;
  f->parameter[n]->type = type;
  f->numParameter++;
}



/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

void st_var_insert( char* scope, char* name,int lineno){

  BucketList l = st_lookup(scope, name);
  if(l){
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
}

void st_insert( char * scope, char* name, Type type, int lineno, int loc )
{ 
  ScopeList s = find_scope(scope);
  //if scope name does not exist
  if (s==NULL){
    printf("invalid scope name\n");
  }
  BucketList l = st_lookup(scope, name);

  if (!l) /* variable not yet in table */
  {
     l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->type = type;
    l->size = 0;
    l->lines->next = NULL;
    s->bucket[s->bucketNum] = l;
    s->bucketNum = s->bucketNum + 1;
  }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * scope, char* name ){
  ScopeList s = find_scope(scope);
  BucketList l;
  while(TRUE){
    for(int i = 0; i< s->bucketNum; i++){
      l = s->bucket[i];
      if(strcmp(name, l->name) == 0){
        return l;
      }
    }
    if(strcmp(s->name, "global") == 0){
       break;
    }
    s = s->parent;
  }
  return NULL;
}

BucketList st_lookup_excluding_parent(char* scope, char* name){
  ScopeList s = find_scope(scope);
  BucketList l;
  for(int i = 0; i<s->bucketNum; i++){
    l = s->bucket[i];
    if(strcmp(l->name, name)== 0){
      return l;
    }
  }
  return NULL;
}
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"\nSymbol table:\n\n");
  fprintf(listing,"Variable Name\tVariable Type\tScope Name\tLocation\tLine Numbers\n");
  fprintf(listing,"-------------\t-------------\t----------\t--------\t------------\n");
  for(int j=0;j<scopeNum; j++){
    ScopeList sc = scopeList[j];
    for (i=0;i<sc->bucketNum;i++)
    {   
      BucketList l = sc->bucket[i];
      LineList t = l->lines;
      fprintf(listing,"%-14s",l->name);
      fprintf(listing,"\t%-14s",types[l->type]);
      fprintf(listing,"\t%-10s",sc->name);
      fprintf(listing,"\t%-8d\t",l->memloc);
      while (t != NULL)
      { fprintf(listing,"%d ",t->lineno);
        t = t->next;
      }
      fprintf(listing,"\n");
    }
  }
  fprintf(listing, "\n\nfunction table:\n\n");
  fprintf(listing,"Function Name\tReturn Type\tParameter Name\tParameter Type\n");
  fprintf(listing,"-------------\t-----------\t--------------\t--------------\n");
  for(int k = 0; k<funcNum; k++){
    FuncList f = funcList[k];
    fprintf(listing, "%-14s",f->name);
    fprintf(listing, "\t%-11s", types[f->type]);
    if(f->numParameter == 0){
      fprintf(listing, "\t\t\tvoid");
    }
    for(int h = 0; h<f->numParameter; h++){
      fprintf(listing, "\n\t\t\t\t%s",f->parameter[h]->name);
      fprintf(listing, "\t\t%-15s",types[f->parameter[h]->type]);
    }
    fprintf(listing, "\n");

  }
} /* printSymTab */
