#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "code.h"

/* tmpOffset is the memory offset for temps
   It is decremented each time a temp is
   stored, and incremeted when loaded again
*/
static int tmpOffset = 0;

/* prototype for internal recursive code generator */
static void cGen(TreeNode *tree);

static char *idStack[999];
static int idStackTop = -1;

static char *getVariableName() {
  static int number = 0;
  char *buffer = (char*) malloc(sizeof(char)*10);
  sprintf(buffer, "t%d", number);
  ++number;
  return buffer;
}

static void genExpr(TreeNode *tree)
{
  switch(tree->kind.exp)
  {
    case Operator:
      switch(tree->attr.op)
      {
        case PLUS:
        case MINUS:
        case TIMES:
        case OVER:
          if (tree->child[1]->nodekind == Id) {
            emitAssignThreeValues(idStack[idStackTop--], tree->child[0]->attr.name, tree->child[1]->attr.name, tree->attr.op);
          }
          else {
            char *newTmp = getVariableName();
            emitAssignThreeValues(idStack[idStackTop--], tree->child[0]->attr.name, newTmp, tree->attr.op);
            idStack[++idStackTop] = newTmp;
            cGen(tree->child[1]);
          }
          break;
        default:
          break;
      }
  }
}

static void genStmt(TreeNode *tree)
{
  switch(tree->kind.stmt)
  {
    case Assign:
      if (TraceCode) emitComment("-> assign");
      if (tree->child[1]->nodekind == Id) {
        emitAssignTwoValues(tree->child[0]->attr.name, tree->child[1]->attr.name);
      }
      else {
        char *newTmp = getVariableName();
        emitAssignTwoValues(tree->child[0]->attr.name, newTmp);
        idStack[++idStackTop] = newTmp;
        cGen(tree->child[1]);
      }
      if (TraceCode) emitComment("<- assign");
      break;
    default:
      break;
  }
}

/* Procedure cGen recursively generates code by
 * tree traversal (pre-order)
 */
static void cGen(TreeNode * tree)
{ 
  if (tree != NULL)
  {
   switch (tree->nodekind) 
   {
      case Statement:
        genStmt(tree);
        break;
      case Expression:
        genExpr(tree);
        break;
      case Id:
        switch(tree->kind.id)
        {
          case Function:
            for (int i = 0; i < MAXCHILDREN; ++i) {
              cGen(tree->child[i]);
            }
            break;
          case Variable:
            idStack[++idStackTop] = tree->attr.name;
            break;
          default:
            break;
        }
        break;
      case Type:
        if (tree->child[0]->kind.id == Function)
          cGen(tree->child[0]);
        break;
      default:
        break;
    }
    cGen(tree->sibling);
  }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode *syntaxTree, char *codefile)
{  
   char *s = (char*) malloc(strlen(codefile) + 7);
   strcpy(s, "File: ");
   strcat(s, codefile);
   emitComment("CMINUS Compilation to CMINUS Intermediate Code");
   emitComment(s);
   /* generate code for CMINUS program */
   cGen(syntaxTree);
   /* finish */
   emitComment("End of execution.");
}
