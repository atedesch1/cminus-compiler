#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "scopetree.h"
#include "globals.h"

/* the list of line numbers of the source
 * code in which a variable is referenced
 */
typedef struct LineList
{
  int lineno;
  struct LineList *next;
} *LineList;

/* The record in the bucket lists for
 * each variable, including id,
 * id kind, type kind, the scope list and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketList
{
  char *id;
  IdKind idkind;
  TypeKind typekind;
  ScopeList scopes;
  LineList lines;
  struct BucketList *next;
} *BucketList;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void symbolTableInsert(char *name, IdKind idkind, TypeKind typekind, int currScope, int lineno, ScopeNode *currScopeNode);

/* Function symbolTableLookup returns a pointer to the
 * existing variable in the lookup table or a null pointer
 * if the variable was not found.
 */
BucketList symbolTableLookup(char *name, int scope);

/* Procedure printSymbolTable prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymbolTable(FILE * listing);

#endif
