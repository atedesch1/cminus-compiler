#ifndef _SCOPETREE_H
#define _SCOPETREE_H

#include <stdbool.h>

typedef struct scopeList
{
  int scope;
  struct scopeList *next;
} *ScopeList;

ScopeList newScopeList(int scope);

ScopeList pushScopeList(ScopeList list, int scope);

typedef struct scopeNode
{
  int scope;
  struct scopeNode *parent;
  struct scopeNode **children;
  int numChildren;
} ScopeNode;

ScopeNode *newRootScopeNode();

ScopeNode *newScopeNode(int scope);

ScopeNode *insertScope(ScopeNode *currNode, int lastScope);

bool isInsideScope(ScopeNode *node, ScopeList scopes);

#endif