#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "scopetree.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash(char *key)
{
  int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure symbolTableInsert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */ // todo: must review this method
void symbolTableInsert(char *name, IdKind idkind, TypeKind typekind, int lineno, ScopeNode* currScopeNode)
{
  int h = hash(name);
  BucketList l = hashTable[h];
  while (l != NULL) {
    if (strcmp(name, l->id) == 0) {
      if (isInsideScope(currScopeNode, l->scopes)) {
        /* if it is needed, we update the scope list of the found variable */
        bool updateScopeList = true;
        ScopeList s = l->scopes;
        ScopeList sLast = s;
        while (s != NULL) {
          if (s->scope == currScopeNode->scope) {
            updateScopeList = false;
            break;
          }
          sLast = s;
          s = s->next;
        }

        if (updateScopeList) {
          printf("We're here ; varName: %s\n", name);
          sLast->next = (ScopeList)malloc(sizeof(struct scopeList));
          sLast->next->scope = currScopeNode->scope;
          sLast->next->next = NULL;
        }
        /* if it is needed, we update the line list of the found variable */
        bool updateLineList = true;
        LineList t = l->lines;
        LineList tLast = t;
        while (t != NULL) {
          if (t->lineno == lineno) {
            updateLineList = false;
            break;
          }
          tLast = t;
          t = t->next;
        }

        if (updateLineList) {
          tLast->next = (LineList)malloc(sizeof(struct LineList));
          tLast->next->lineno = lineno;
          tLast->next->next = NULL;
        }

        /* since there is a single row for a variable in a scope, we can return */
        return;
      }
    }
    l = l->next;
  }
  /* the variable was not found in currScope, so create a new one */
  l = (BucketList)malloc(sizeof(struct BucketList));
  l->id = name;
  l->idkind = idkind;
  l->typekind = typekind;
  l->scopes = (ScopeList)malloc(sizeof(struct scopeList));
  l->scopes->scope = currScopeNode->scope;
  l->scopes->next = NULL;
  l->lines = (LineList)malloc(sizeof(struct LineList));
  l->lines->lineno = lineno;
  l->lines->next = NULL;
  l->next = NULL;

  BucketList tail = hashTable[h];
  if (tail == NULL) {
    hashTable[h] = l;
  }
  else {
    while (tail->next != NULL)
      tail = tail->next;
    tail->next = l;
  }
}

/* Function symbolTableLookup returns a pointer to the
 * existing variable in the lookup table or a null pointer 
 * if the variable was not found.
 */
BucketList symbolTableLookup(char *name, ScopeNode* currScopeNode)
{
  int h = hash(name);
  BucketList l = hashTable[h];
  while (l != NULL) {
    if (strcmp(name, l->id) == 0) {
      if (isInsideScope(currScopeNode, l->scopes)) {
        return l;        
      }
    }
    l = l->next;
  }
  return NULL;
}

/* Procedure printSymbolTable prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymbolTable(FILE *listing)
{
  int i;
  fprintf(listing, "Id\t\tTypeKind\t\tIdKind\t\tScopes\t\tLine Numbers\n");
  fprintf(listing, "----\t\t--------\t\t------\t\t------\t\t------------\n");
  for (i = 0; i < SIZE; ++i)
  {
    if (hashTable[i] != NULL)
    {
      BucketList l = hashTable[i];
      while (l != NULL)
      {
        LineList t = l->lines;
        fprintf(listing, "%-14s\t", l->id);
        
        switch(l->typekind) 
        {
          case Void:
            fprintf(listing, "Void\t\t\t");
            break;
          case Int:
            fprintf(listing, "Int\t\t\t");
            break;
          default:
            break;
        }
        
        switch(l->idkind) 
        {
          case Variable:
            fprintf(listing, "Variable\t");
            break;
          case Array:
            fprintf(listing, "Array\t\t");
            break;
          case Function:
            fprintf(listing, "Function\t");
            break;
          default:
            break;
        }

        ScopeList s = l->scopes;
        while (s != NULL) 
        {
          fprintf(listing, "%d ", s->scope);
          s = s->next;
        }
        fprintf(listing, "\t\t");

        while (t != NULL)
        {
          fprintf(listing, "%d ", t->lineno);
          t = t->next;
        }
        fprintf(listing, "\n");
        l = l->next;
      }
    }
  }
}
