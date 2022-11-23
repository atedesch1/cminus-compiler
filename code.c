#include "globals.h"
#include "code.h"

/* TM location number for current instruction emission */
static int emitLoc = 0 ;

/* Highest TM location emitted so far
   For use in conjunction with emitSkip,
   emitBackup, and emitRestore */
static int highEmitLoc = 0;

/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment( char * c )
{ if (TraceCode) fprintf(code,"* %s\n",c);}

/* Function emitSkip skips "howMany" code
 * locations for later backpatch. It also
 * returns the current code position
 */
int emitSkip( int howMany)
{  int i = emitLoc;
   emitLoc += howMany ;
   if (highEmitLoc < emitLoc)  highEmitLoc = emitLoc ;
   return i;
} /* emitSkip */

/* Procedure emitBackup backs up to 
 * loc = a previously skipped location
 */
void emitBackup( int loc)
{ if (loc > highEmitLoc) emitComment("BUG in emitBackup");
  emitLoc = loc ;
} /* emitBackup */

/* Procedure emitAssign returns a
 * assign instruction in the 
 * intermediate code.
 */
void emitAssignTwoValues(char *lhs, char *rhs) {
   fprintf(code, "%s = %s\n", lhs, rhs);
}

void emitAssignThreeValues(char *lhs, char *rhs1, char *rhs2, TokenType op) {
   char opChar1;
   char opChar2 = ' ';
   switch(op) {
      case PLUS:
         opChar1 = '+';
         break;
      case MINUS:
         opChar1 = '-';
         break;
      case TIMES:
         opChar1 = '*';
         break;
      case OVER:
         opChar1 = '/';
         break;
      case LESS_EQUAL_THAN:
         opChar1 = '<';
         opChar2 = '=';
         break;
      case LESS_THAN:
         opChar1 = '<';
         break;
      case GREATER_EQUAL_THAN:
         opChar1 = '<';
         opChar2 = '=';
         break;
      case GREATER_THAN:
         opChar1 = '<';
         break;
      case EQUAL:
         opChar1 = '=';
         opChar2 = '=';
         break;
      case DIFF:
         opChar1 = '!';
         opChar2 = '=';
         break;
      default:
         break;
   }
   if (opChar2 == ' ')
      fprintf(code, "%s = %s %c %s\n", lhs, rhs1, opChar1, rhs2);
   else
      fprintf(code, "%s = %s %c%c %s\n", lhs, rhs1, opChar1, opChar2, rhs2);
}

void emitAssignIndex(char *lhs, char *array, char *index)
{
   fprintf(code, "%s = %s[%s]\n", lhs, array, index);
}

void emitIfGoto(char *condition, char *label)
{
   fprintf(code, "if_false %s goto %s\n", condition, label);
}

void emitLabel(char *label)
{
   fprintf(code, "%s:\n", label);
}

void emitGoto(char *label)
{
   fprintf(code, "goto %s\n", label);
}

void emitReturn(char *expr)
{
   if (expr == NULL)
      fprintf(code, "return\n");
   else
      fprintf(code, "return %s\n", expr);
}

void emitParam(char *param)
{
   fprintf(code, "param %s\n", param);
}

void emitCall(char *identifier, int val)
{
   if (val == 0)
      fprintf(code, "call %s\n", identifier);
   else
      fprintf(code, "call %s, %d\n", identifier, val);
}

/* Procedure emitRestore restores the current 
 * code position to the highest previously
 * unemitted position
 */
void emitRestore(void)
{ emitLoc = highEmitLoc;}

/* Procedure emitRM_Abs converts an absolute reference 
 * to a pc-relative reference when emitting a
 * register-to-memory TM instruction
 * op = the opcode
 * r = target register
 * a = the absolute location in memory
 * c = a comment to be printed if TraceCode is TRUE
 */
// void emitRM_Abs( char *op, int r, int a, char * c)
// { fprintf(code,"%3d:  %5s  %d,%d(%d) ",
//                emitLoc,op,r,a-(emitLoc+1),pc);
//   ++emitLoc ;
//   if (TraceCode) fprintf(code,"\t%s",c) ;
//   fprintf(code,"\n") ;
//   if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
// } /* emitRM_Abs */
