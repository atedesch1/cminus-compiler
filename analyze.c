#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode *t, void (*preProc)(TreeNode *),
                     void (*postProc)(TreeNode *))
{
  if (t != NULL)
  {
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++)
        traverse(t->child[i], preProc, postProc);
    }
    postProc(t);
    traverse(t->sibling, preProc, postProc);
  }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode *t)
{
  if (t == NULL)
    return;
  else
    return;
}

static void declarationError(TreeNode *t, char *message, char *variableName)
{
  fprintf(listing, "SEMANTIC ERROR: declaration of %s at line %d: %s\n", variableName,
          t->lineno, message);
  Error = TRUE;
}

static void usageError(TreeNode *t, char *message, char *variableName)
{
  fprintf(listing, "SEMANTIC ERROR: bad usage of %s at line %d: %s\n", variableName,
          t->lineno, message);
  Error = TRUE;
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode(TreeNode *t)
{
  if (t->nodekind != Id)
    return;

  int numArgs = 0;
  BucketList symbol = symbolTableLookup(t->attr.name, t->scopeNode);
  TreeNode *parent = t->parent;
  bool shouldInsert = true;

  switch (t->kind.id)
  {
  case Variable:
  case Array:
    if (parent != NULL)
    {
      switch (parent->nodekind)
      {
      case Type:
        switch (parent->kind.type)
        {
        case Void:
          declarationError(t, "void type for variables is not allowed.",
                           t->attr.name);
          shouldInsert = false;
          break;
        case Int:
          if (symbol != NULL)
          {
            declarationError(t, "redeclaration of variables or functions is not allowed.",
                             t->attr.name);
            shouldInsert = false;
          }
          break;
        default:
          break;
        }
        break;
      case Expression:
        switch (parent->kind.exp)
        {
        case Operator:
        case Return:
          if (symbol == NULL)
          {
            usageError(t, "implicit declaration is not allowed.",
                       t->attr.name);
            shouldInsert = false;
          }
          break;
        default:
          break;
        }
        break;
      case Statement:
        switch (parent->kind.stmt)
        {
        case Assign:
          if (symbol == NULL)
          {
            usageError(t, "implicit declaration is not allowed.",
                       t->attr.name);
            shouldInsert = false;
          }
        default:
          break;
        }
        break;
      case Function:
        if (symbol == NULL)
        {
          usageError(t, "implicit declaration is not allowed.",
                     t->attr.name);
          shouldInsert = false;
        }
        break;
      default:
        break;
      }
    }
    break;
  case Function:
    if (parent != NULL)
    {
      switch (parent->nodekind)
      {
      case Type:
        switch (parent->kind.type)
        {
        case Void:
        case Int:
          if (t->child[0] != NULL)
          {
            for (TreeNode *arg = t->child[0]; arg != NULL; arg = arg->sibling, numArgs++)
              ;
          }
          /* Check for double declaration in the scope */
          if (symbol != NULL)
          {
            declarationError(
                t, "redeclaration of variables or functions is not allowed.",
                t->attr.name);
            shouldInsert = false;
          }
          break;
        default:
          break;
        }
        break;
      case Expression:
        switch (parent->kind.exp)
        {
        case Operator:
        case Return:
          /* Check if function was defined */
          if (symbol == NULL)
          {
            usageError(t, "implicit declaration is not allowed.",
                       t->attr.name);
            shouldInsert = false;
          }
          break;
        default:
          break;
        }
        break;
      case Statement:
        switch (parent->kind.stmt)
        {
        case Assign:
          /* Check if function was defined */
          if (symbol == NULL)
          {
            usageError(t, "implicit declaration is not allowed.",
                       t->attr.name);
            shouldInsert = false;
          }
          break;
        default:
          break;
        }
        break;
      case Function:
        if (symbol == NULL)
        {
          usageError(t, "implicit declaration is not allowed.",
                     t->attr.name);
          shouldInsert = false;
        }
      default:
        break;
      }
    }
    else
    {
      /* Call of a function without a parent (just a call) */
      if (symbol == NULL)
      {
        usageError(t, "implicit declaration is not allowed.", t->attr.name);
        shouldInsert = false;
      }
    }
    break;
  default:
    break;
  }

  if (shouldInsert)
    symbolTableInsert(t->attr.name,
                      t->kind.id,
                      parent != NULL ? parent->kind.id : symbol->typekind,
                      t->lineno,
                      t->scopeNode,
                      numArgs);
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode *syntaxTree)
{
  traverse(syntaxTree, insertNode, nullProc);
}

static void typeError(TreeNode *t, char *message)
{
  fprintf(listing, "SEMANTIC ERROR: type error at line %d: %s\n", t->lineno, message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode *t)
{
  switch (t->nodekind)
  {
  case Expression:
    switch (t->kind.exp)
    {
    case Operator:
      for (int nchild = 0; nchild < 2; nchild++)
      {
        switch (t->child[nchild]->nodekind)
        {
        case Id:;
          BucketList childSymbol = symbolTableLookup(t->child[nchild]->attr.name, t->child[nchild]->scopeNode);
          if (childSymbol != NULL && childSymbol->typekind == Void)
            typeError(t->child[nchild], "Op applied to non-integer");
          break;
        default:
          break;
        }
      }
      break;
    // case Return:
    // BucketList functionSymbol = getFunctionOfReturn(t);
    // break;
    default:
      break;
    }
    break;
  case Statement:
    switch (t->kind.stmt)
    {
    case If:
    case While:
      if (t->child[0]->nodekind == Id)
      {
        BucketList childSymbol = symbolTableLookup(t->child[0]->attr.name, t->child[0]->scopeNode);
        if (childSymbol != NULL && childSymbol->typekind == Void)
          typeError(t->child[0], "can't have void as condition");
      }
      break;
    case Assign:
      if (t->child[1]->nodekind == Id)
      {
        BucketList childSymbol = symbolTableLookup(t->child[1]->attr.name, t->child[1]->scopeNode);
        if (childSymbol != NULL && childSymbol->typekind == Void)
          typeError(t->child[1], "assignment of non-integer value");
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode *syntaxTree)
{
  traverse(syntaxTree, nullProc, checkNode);
}

/* Procedure mainCheck verifies if
 * a function called main is present
 * in a cminus program.
 */
void mainCheck()
{
  if (symbolTableLookup("main", scopeTree) == NULL)
  {
    fprintf(listing, "SEMANTIC ERROR: no main function was found.\n");
    Error = TRUE;
  }
}
