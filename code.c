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
   char opChar;
   switch(op) {
      case PLUS:
         opChar = '+';
         break;
      case MINUS:
         opChar = '-';
         break;
      case TIMES:
         opChar = '*';
         break;
      case OVER:
         opChar = '/';
         break;
      default:
         break;
   }
   fprintf(code, "%s = %s %c %s\n", lhs, rhs1, opChar, rhs2);
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
