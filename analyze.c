#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
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
static void declarationError(TreeNode *t, char *message, char *variableName)
{
  fprintf(listing, "ERROR: declaration of %s at line %d: %s\n", variableName, t->lineno, message);
  Error = TRUE;
}

static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case Statement:
      switch (t->kind.stmt)
      { case Assign:
        case While:
        case If:
        default:
          break;
      }
      break;
    case Expression:
      switch (t->kind.exp)
      { case Return:
        case Constant:
        case Operator:
        default:
          break;
      }
      break;
    case Id:
      switch (t->kind.id)
      { 
        case Variable:
        case Array:
          /* t->parent returns NULL for some reason... check this later */
          if (t->parent != NULL) 
          {
            switch(t->parent->nodekind) 
            {
              case Type:
                switch(t->parent->kind.id) 
                {
                  case Void:
                    /* Variable of type void is not allowed */
                    declarationError(t, "void type for variables is not allowed.", t->attr.name);
                    break;
                  case Int:
                    /* Check for double declaration in the scope */
                    if (symbolTableLookup(t->attr.name, t->scopeNode) != NULL)
                      declarationError(t, "redeclaration of variables is not allowed.", t->attr.name);
                    else
                      /* Otherwise, we add the new symbol in the table (or just a use of it) */
                      symbolTableInsert(t->attr.name, t->kind.id, t->parent->kind.id, t->lineno, t->scopeNode);
                    break;
                  default:
                    break;
                }
                break;
              case Expression:
                switch(t->parent->kind.id)
                {
                  case Operator:
                  case Return:
                    /* Check if variable was declared previously in the scope */
                    break;
                  default:
                    break;
                }
                break;
              default:
                break;
            }
          }
          break;
        case Function:
          if (t->parent != NULL) {
            switch(t->parent->nodekind)
            {
              case Type:
                switch(t->parent->kind.id) 
                {
                  case Void:
                  case Int:
                    /* Check for double declaration in the scope */
                    if (symbolTableLookup(t->attr.name, t->scopeNode) != NULL)
                      declarationError(t, "redeclaration of functions is not allowed.", t->attr.name);
                    else
                      /* Otherwise, we add the new symbol in the table (or just a call of it) */
                      symbolTableInsert(t->attr.name, t->kind.id, t->parent->kind.id, t->lineno, t->scopeNode);
                    break;
                  default:
                    break;
                }
              default:
                break;
            }
          }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ 
  traverse(syntaxTree, insertNode, nullProc);
  if (TraceAnalyze) { 
    fprintf(listing, "\nSymbol table:\n\n");
    printSymbolTable(listing);
  }
}

static void typeError(TreeNode * t, char * message) { 
  fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case Expression:
      switch (t->kind.exp)
      { case Operator: /* dummy instruction */
          if ((t->child[0]->type != IntegerType) ||
              (t->child[1]->type != IntegerType))
            typeError(t,"Op applied to non-integer");
          if ((t->attr.op == EQUAL) || (t->attr.op == LESS_THAN))
            t->type = BooleanType;
          else
            t->type = IntegerType;
          break;
        case Constant: /* dummy instruction */
          t->type = IntegerType;
          break;
        default:
          break;
      }
      break;
    case Statement:
      switch (t->kind.stmt)
      { case If: /* dummy instruction */
          if (t->child[0]->type == IntegerType)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case Assign: /* dummy instruction */
          if (t->child[0]->type != IntegerType)
            typeError(t->child[0],"assignment of non-integer value");
          break;
        case While: /* dummy instruction */
          if (t->child[0]->type != IntegerType)
            typeError(t->child[0],"write of non-integer value");
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
void typeCheck(TreeNode * syntaxTree) { traverse(syntaxTree,nullProc,checkNode); }

/* Procedure mainCheck verifies if
 * a function called main is present
 * in a cminus program.
 */
void mainCheck() {
  if (symbolTableLookup("main", scopeTree) == NULL) {
    fprintf(listing, "ERROR: no main function was found.\n");
    Error = TRUE;
  }
}
