#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;

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
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case Statement:
      switch (t->kind.stmt)
      { case Assign: /* dummy instruction */
        case While: /* dummy instruction */
        case If: /* dummy instruction */
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0);
          break;
        default:
          break;
      }
      break;
    case Expression:
      switch (t->kind.exp)
      { case Return: /* dummy instruction */
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0);
          break;
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
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
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
