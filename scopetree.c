#include <stdlib.h>
#include "scopetree.h"

ScopeList newScopeList(int scope)
{
  ScopeList list = (ScopeList)malloc(sizeof(struct scopeList));
  list->scope = scope;
  list->next = NULL;
  return list;
}

ScopeList pushScopeList(ScopeList list, int scope)
{
  ScopeList newNode = (ScopeList)malloc(sizeof(struct scopeList));
  newNode->scope = scope;
  newNode->next = NULL;
  ScopeList p = list;
  while (p->next != NULL)
    p = p->next;
  p->next = newNode;
  return list;
}

ScopeNode *newRootScopeNode()
{
  ScopeNode *newNode = (ScopeNode *)malloc(sizeof(ScopeNode));
  newNode->scope = -1;
  newNode->children = NULL;
  newNode->parent = NULL;
  newNode->numChildren = 0;
  return newNode;
}

ScopeNode *newScopeNode(int scope)
{
  ScopeNode *newNode = (ScopeNode *)malloc(sizeof(ScopeNode));
  newNode->scope = scope;
  newNode->children = NULL;
  newNode->parent = NULL;
  newNode->numChildren = 0;
  return newNode;
}

ScopeNode *insertScope(ScopeNode *currNode, int lastScope)
{
  ScopeNode *newNode = newScopeNode(lastScope + 1);
  if (currNode->children == NULL)
  {
    currNode->children = (ScopeNode **)malloc(sizeof(ScopeNode *));
    currNode->children[0] = newNode;
    currNode->children[0]->parent = currNode;
    ++(currNode->numChildren);
  }
  else
  {
    currNode->children = (ScopeNode **)realloc(currNode->children, (++(currNode->numChildren)));
    currNode->children[currNode->numChildren - 1] = newNode;
    currNode->children[currNode->numChildren - 1]->parent = currNode;
  }
  return currNode->children[currNode->numChildren - 1];
}

bool isInsideScope(ScopeNode *node, ScopeList scopes)
{
  while (node != NULL)
  {
    for (ScopeList s = scopes; s != NULL; s = s->next)
    {
      if (node->scope == s->scope)
      {
        return true;
      }
    }
    node = node->parent;
  }
  return false;
}