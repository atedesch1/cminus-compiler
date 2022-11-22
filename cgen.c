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
static bool arrayLhs = false;

static char *getVariableName() {
  static int number = 0;
  char *buffer = (char*) malloc(sizeof(char)*10);
  sprintf(buffer, "t%d", number);
  ++number;
  return buffer;
}

static char *getLabelName() {
  static int number = 0;
  char *buffer = (char*) malloc(sizeof(char)*10);
  sprintf(buffer, "L%d", number);
  ++number;
  return buffer;
}

static char *convertConstantToString(int val) {
  char *buffer = (char*) malloc(sizeof(char)*32);
  sprintf(buffer, "%d", val);
  return buffer;
}

static void genExpr(TreeNode *tree)
{
  switch(tree->kind.exp)
  {
    case Operator:
      idStack[++idStackTop] = getVariableName();
      cGen(tree->child[0]);
      cGen(tree->child[1]);
      emitAssignThreeValues(idStack[idStackTop - 2], idStack[idStackTop - 1], idStack[idStackTop], tree->attr.op);
      idStackTop -= 2;
      break;
    case Constant:
      idStack[++idStackTop] = convertConstantToString(tree->attr.val);
      break;
    case Return:
      if (tree->child[0] == NULL) 
      {
        emitReturn(NULL);
      }
      else
      {
        cGen(tree->child[0]);
        emitReturn(idStack[idStackTop--]);
      }
      break;
    default:
      break;
  }
}

static void genStmt(TreeNode *tree)
{
  switch(tree->kind.stmt)
  {
    case Assign:
      if (TraceCode) emitComment("-> assign");
      if (tree->child[0]->kind.id == Array)
        arrayLhs = true;
      cGen(tree->child[0]);
      cGen(tree->child[1]);
      emitAssignTwoValues(idStack[idStackTop - 1], idStack[idStackTop]);
      idStackTop -= 2;
      if (TraceCode) emitComment("<- assign");
      break;
    case If:
      if (TraceCode) emitComment("-> if");
      cGen(tree->child[0]);

      char *label = getLabelName();
      emitIfGoto(idStack[idStackTop--], label);
      cGen(tree->child[1]);
      
      if (tree->child[2] != NULL) 
      {
        char *tmpLabel = getLabelName();
        emitGoto(tmpLabel);
        emitLabel(label);
        cGen(tree->child[2]);
        emitLabel(tmpLabel);
      }
      else
        emitLabel(label);
      if (TraceCode) emitComment("<- if");
      break;
    case While:
      if (TraceCode) emitComment("-> while");
      char *labelSuccess = getLabelName();
      char *labelFail = getLabelName();
      emitLabel(labelSuccess);
      cGen(tree->child[0]);
      emitIfGoto(idStack[idStackTop--], labelFail);
      cGen(tree->child[1]);
      emitGoto(labelSuccess);
      emitLabel(labelFail);
      if (TraceCode) emitComment("<- while");
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
            /* Function declaration */
            if (tree->parent != NULL && tree->parent->nodekind == Type)
            {
              emitLabel(tree->attr.name);
              cGen(tree->child[1]);
            }
            else
            {
              /* TODO: we need to change this else */
              for (int i = 0; i < MAXCHILDREN; ++i) 
              {
                cGen(tree->child[i]);
              }
            }
            break;
          case Variable:
            idStack[++idStackTop] = tree->attr.name;
            break;
          case Array:
            /* Index access */
            if (tree->parent == NULL || tree->parent->nodekind != Type)
            {
              cGen(tree->child[0]);
              char *index = getVariableName();
              emitAssignThreeValues(index, idStack[idStackTop--], "4", TIMES);
              char *buffer = (char*) malloc(sizeof(char)*32);
              sprintf(buffer, "%s[%s]", tree->attr.name, index);
              if (arrayLhs)
              {
                idStack[++idStackTop] = buffer;
                arrayLhs = false;
              }
              else
              {
                idStack[++idStackTop] = getVariableName();
                emitAssignTwoValues(idStack[idStackTop], buffer);
              }
            }
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
