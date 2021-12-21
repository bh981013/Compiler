/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;
static int error = 0;
static char* glob = "global";
static char* inputfunc = "input";
static char* outputfunc = "output";
static ScopeList curScope;
static char* curFunc = "global";
char* typename[] = {"void", "int", "void[]", "int[]", "None"};
/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ 
  if (t != NULL)
  { 
    preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
int madeScope = 0;
int compNum = 0;
static void insertNode( TreeNode * t)
{  
  BucketList b;
  if(error) return;
  switch (t->kind){
    case VarDK:
      if(st_lookup_excluding_parent(curScope->name, t->name)){
        fprintf(listing,"Semantic Error: redefined variable '%s' at line %d\n", t->name, t->lineno);
      }
      else{
        if(st_lookup(curScope->name, t->name) == NULL){
          //this variable does not exist
          st_insert(curScope->name, t->name, t->type, t->lineno, curScope->bucketNum);
        }
        else{
          st_insert(curScope->name, t->name, t->type, t->lineno, 0);
        }
      }
      break;

    case FunK:
      if(find_func(t->name) == NULL){    
        st_insert(curScope->name, t->name, t->type, t->lineno, curScope->bucketNum);
        curScope = makeScope(t->name, curScope);
        curFunc = t->name;
        madeScope = 1;
        add_func(t->name, t->type);
      }
      else{
        fprintf(listing,"Semantic Error: redefined function '%s' at line %d\n", t->name, t->lineno);
        error = 1;
        madeScope = 1;
      }
      break;
    case paraK:
      b = st_lookup_excluding_parent(curScope->name, t->name);
      if(b != NULL){
        break;
      }
      else{
         st_insert(curScope->name, t->name, t->type, t->lineno, curScope->bucketNum);
      }
      FuncList f = find_func(curScope->name);
      f->parameter[f->numParameter] = malloc(sizeof(Parameter));
      f->parameter[f->numParameter]->name = t->name;
      f->parameter[f->numParameter]->type = t->type;
      f->numParameter++;
      break;
    case CompK:
      if(madeScope == 1) madeScope = 0;
      else{
        char* sc = malloc(sizeof(curScope->name)+5);
        strcpy(sc, curScope->name);
        char b[10] = ".";
        char* c = malloc(10);
        strcat(b, c);
        strcat(sc, b);        
        ScopeList newScope = makeScope(sc, curScope);
        curScope = newScope;
      }
      break;
    case VarK:
      st_var_insert( curScope->name, t->name, t->lineno );
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
static void popProc(TreeNode * t){
  if(t->kind == CompK){
    if(error){error = 0;}
    else{
      curScope = curScope->parent;
      if (strcmp(curScope->name, glob) == 0){
        curFunc = glob;
      }
    }
  }
}

static void pushProc(TreeNode * t){
  if(t->kind == FunK){
    curScope = find_scope(t->name);
    curFunc = t->name;
    madeScope =1;
  }
  else if(t->kind ==CompK && madeScope == 1){
    madeScope = 0;
  }
  else if(t->kind ==CompK && madeScope == 0){
      char* sc = malloc(sizeof(curScope->name)+5);
      strcpy(sc, curScope->name);
      char b[10] = ".";
      char* c = malloc(10);
      strcat(b, c);
      strcat(sc, b);        
      ScopeList newScope = makeScope(sc, curScope);
      curScope = newScope;
  }
}
// static void pushProc(TreeNode* t){
//   if(t->kind == CompK){
//   }
// }

void addGlobalfunc(ScopeList sc){
  add_func(inputfunc, Integer);
  add_func(outputfunc, Void);
  FuncList f2 = find_func(outputfunc);
  f2->parameter[f2->numParameter] = malloc(sizeof(Parameter));
  f2->parameter[f2->numParameter]->name = "value";
  f2->parameter[f2->numParameter]->type = Integer;
  f2->numParameter++;
}
void buildSymtab(TreeNode * syntaxTree)
{ 
  curScope = makeScope(glob, NULL);
  addGlobalfunc(curScope);
  traverse(syntaxTree,insertNode,popProc);
  if (TraceAnalyze)
  { 
    printSymTab(listing);
  }
}


/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ 
  ScopeList sc;
  BucketList b;
  FuncList f;
  int numArgs;
  switch (t->kind){
    case VarDK:
      if (t->type == Void){
        fprintf(listing, "Semantic Error: type 'void' is only available for functions");
      }
      break;
    case FunK:
      break;
    case WhileK:
      if(t->child[0]->type != Integer){
        fprintf(listing, "Semantic Error: condition of While statment is not a value at line %d\n", t->lineno);
      }
      break;
    case IfelseK:
    case IfK:
      if(t->child[0]->type != Integer){
        fprintf(listing, "Semantic Error: condition of If statment is not a value at line %d\n", t->lineno);
      }
      break;
    case ExpK:
      if(t->child[0]->type == Integer && t->child[1]->type == Integer){
        t->type = Integer;
      }
      else{
        fprintf(listing, "Semantic Error: type1: %s, type2: %s, expression does not match type at line %d\n", 
        typename[t->child[0]->type],typename[t->child[1]->type],t->lineno);
        t->type = None;
      }
      break;
    case VarK:
      b = st_lookup(curScope->name, t->name);
      if(b==NULL){
        fprintf(listing, "Semantic Error: variable '%s' hasn't been declared. Error at line %d\n", t->name,t->lineno);
        t->type = None;
      }
      else{
        t->type = b->type;
      }
      break;
    case VarAK:
      b = st_lookup(curScope->name, t->name);
      if(b ==NULL){
        fprintf(listing, "Semantic Error: variable '%s' hasn't been declared. Error at line %d\n", t->name,t->lineno);
        t->type = None;
      }
      t->type = Integer;
      if(b->type != IntA){
        fprintf(listing, "Semantic Error: Cannot use indexing on non-array object. Error at line %d\n", t->lineno);
      }
      else if(t->child[0]->type != Integer){
            fprintf(listing, "Semantic Error: indices should be integer. Error at line %d\n", t->lineno);
      }
      break;
    case OpK:
      if(t->child[0]->type == Integer && t->child[1]->type == Integer){
        t->type = Integer;
      }
      else{
        t->type = None;
        fprintf(listing, "Semantic Error: Operands types do not match at line %d\n", t->lineno);
      }
      break;
    case CallK:
      f = find_func(t->name);
      if(!f){
        fprintf(listing, "Semantic Error: calling non-declared function at line %d\n", t->lineno);
        break;
      }
      t->type = f->type;
      numArgs = 0;
      TreeNode* ch = t->child[0];
      while(ch != NULL){
        numArgs++;
        if(numArgs>f->numParameter) break;
        if(ch->type != f->parameter[numArgs-1]->type){
          fprintf(listing, "Semantic Error: unmatched argument type '%s' at line %d\n", ch->name, t->lineno);
        }
        ch = ch ->sibling;
      }
      if(numArgs != f->numParameter){
        fprintf(listing, "Semantic Error: function '%s' unmatching number of parameter at line %d\n", f->name, t->lineno);
        break;
      }
      break;
    case CompK:
        curScope = curScope->parent;
        if(strcmp(curScope->name, glob) == 0){
          curFunc = glob;
        }
        break;
    case ConstK:
      t->type = Integer;
      break;
    case RetNK:
    case RetK:
      sc = curScope;
      while(strcmp(sc->parent->name, glob) != 0){
        sc = sc->parent;
      }
      f = find_func(sc->name);
      if(t->type == Void && f->type != Void){
        fprintf(listing, "Semantic Error: unmatching return type at line %d\n", t->lineno);
        }
      else if(f->type != t->child[0]->type){
          fprintf(listing, "Semantic Error: unmatching return type '%s', '%s' at line %d\n",
          typename[f->type], typename[t->child[0]->type], t->lineno);
      }
      break;
    default:
      break;
  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree, pushProc, checkNode);
}
