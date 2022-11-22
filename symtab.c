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
 */
// todo: must review this method
void symbolTableInsert(char *name, IdKind idkind, TypeKind typekind, int lineno, ScopeNode *currScopeNode)
{
  int h = hash(name);
  for (BucketList l = hashTable[h]; l != NULL; l = l->next)
  {
    if (!(strcmp(name, l->id) == 0 && isInsideScope(currScopeNode, l->scopes)))
      continue;

    for (LineList ll = l->lines; ll != NULL && ll->lineno != lineno; ll = ll->next)
    {
      if (ll->next == NULL)
      {
        ll->next = (LineList)malloc(sizeof(struct LineList));
        ll->next->lineno = lineno;
        ll->next->next = NULL;
      }
    }

    return;
  }

  /* the variable was not found in currScope, so create a new one */
  BucketList b = (BucketList)malloc(sizeof(struct BucketList));
  b->id = name;
  b->idkind = idkind;
  b->typekind = typekind;
  b->scopes = newScopeList(currScopeNode->scope->name, currScopeNode->scope->id);
  b->lines = (LineList)malloc(sizeof(struct LineList));
  b->lines->lineno = lineno;
  b->lines->next = NULL;
  b->next = NULL;

  BucketList t = hashTable[h];
  if (t == NULL)
  {
    hashTable[h] = b;
  }
  else
  {
    while (t->next != NULL)
      t = t->next;
    t->next = b;
  }
}

/* Function symbolTableLookup returns a pointer to the
 * existing variable in the lookup table or a null pointer
 * if the variable was not found.
 */
BucketList symbolTableLookup(char *name, ScopeNode *currScopeNode)
{
  int h = hash(name);
  BucketList l = hashTable[h];
  while (l != NULL)
  {
    if (strcmp(name, l->id) == 0 && isInsideScope(currScopeNode, l->scopes))
    {
      return l;
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
  fprintf(listing, "Id\t\tTypeKind\t\tIdKind\t\tScope\t\tLine Numbers\n");
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

        switch (l->typekind)
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

        switch (l->idkind)
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
          fprintf(listing, "%d ", s->scope->id);
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
