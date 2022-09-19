#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString ) {
  switch (token) { 
    case ELSE:
    case IF:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
      fprintf(listing, "reserved word: %s\n",tokenString);
      break;
    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case LESS_THAN: fprintf(listing, "<\n"); break;
    case LESS_EQUAL_THAN: fprintf(listing, "<=\n"); break;
    case GREATER_THAN: fprintf(listing, ">\n"); break;
    case GREATER_EQUAL_THAN: fprintf(listing, ">=\n"); break;
    case EQUAL: fprintf(listing, "==\n"); break;
    case DIFF: fprintf(listing, "!=\n"); break;
    case ASSIGN: fprintf(listing, "=\n"); break;
    case SEMICOLON: fprintf(listing, ";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;
    case LEFT_PARENTHESIS: fprintf(listing, "(\n"); break;
    case RIGHT_PARENTHESIS: fprintf(listing, ")\n"); break;
    case LEFT_SQUARE_BRACKET: fprintf(listing, "[\n"); break;
    case RIGHT_SQUARE_BRACKET: fprintf(listing, "]\n"); break;
    case LEFT_CURLY_BRACKET: fprintf(listing, "{\n"); break;
    case RIGHT_CURLY_BRACKET: fprintf(listing, "}\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing, "NUM, val= %s\n", tokenString);
      break;
    case ID:
      fprintf(listing, "ID, name= %s\n", tokenString);
      break;
    case ERROR:
      fprintf(listing, "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
      break;
  }
}