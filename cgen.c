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

static void generateStatement(TreeNode *tree)
{
  TreeNode *p1, *p2, *p3;
  int savedLoc1,savedLoc2,currentLoc;
  int loc;
  switch (tree->kind.stmt) {
      case If:
         break;
      case Assign:
         break;
      case While:
         break;
      default:
         break;
    }
}

static void generateExpression(TreeNode *tree)
{ 
  int loc;
  TreeNode *p1, *p2;
  switch (tree->kind.exp) {
    case Constant:
      break;
    case Operator:
      break;
    default:
      break;
  }
}

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen( TreeNode * tree)
{ 
  if (tree != NULL)
  { 
   switch (tree->nodekind) {
      case Statement:
        generateStatement(tree);
        break;
      case Expression:
        generateExpression(tree);
        break;
      case Id:
        // genId(tree);
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
