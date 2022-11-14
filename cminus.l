%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* lexeme of the last ID token */
char idLexeme[MAXTOKENLEN + 1];

/* lexeme of the last NUM token */
int numValue;

/* stores the lexeme of ID tokens */
char* savedId[128];
int savedIdIndex = -1;

void saveId(char *id) {
  savedId[++savedIdIndex] = copyString(id);
}

char *popId() {
  if (savedIdIndex < 0) {
    return "";
  }
  
  return savedId[savedIdIndex--];
}

%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"<"             {return LESS_THAN;}
"<="            {return LESS_EQUAL_THAN;}
">"             {return GREATER_THAN;}
">="            {return GREATER_EQUAL_THAN;}
"=="            {return EQUAL;}
"!="            {return DIFF;}
"="             {return ASSIGN;}
";"             {return SEMICOLON;}
","             {return COMMA;}
"("             {return LEFT_PARENTHESIS;}
")"             {return RIGHT_PARENTHESIS;}
"["             {return LEFT_SQUARE_BRACKET;}
"]"             {return RIGHT_SQUARE_BRACKET;}
"{"             {return LEFT_CURLY_BRACKET;}
"}"             {return RIGHT_CURLY_BRACKET;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { char c;
                  char foundTimesNext = 0;
                  char foundEnd = 0;
                  do { 
                    c = input();
                    switch(c) {
                      case '\n':
                        lineno++;
                        if (foundTimesNext == 1)
                          foundTimesNext = 0;
                        break;
                      case '*':
                        foundTimesNext = 1;
                        break;
                      case '/':
                        if (foundTimesNext == 1)
                          foundEnd = 1;
                        break;
                      default:
                        if (foundTimesNext == 1)
                          foundTimesNext = 0;
                        break;
                    }
                  } while (c != EOF && !foundEnd);
                }
.               {return ERROR;}
%%

TokenType getToken(void) {
  static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime) {
    firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
    scopeTree = newRootScopeNode();
    currentScope = scopeTree;
    maxScope = -1;
  }
  currentToken = yylex();
  strncpy(tokenString, yytext, MAXTOKENLEN);
  switch(currentToken) {
    case ID:
      strncpy(idLexeme, yytext, MAXTOKENLEN);
      saveId(idLexeme);
      break;
    case NUM:
      numValue = atoi(tokenString);
      break;
    case LEFT_CURLY_BRACKET:
      currentScope = insertScope(currentScope, maxScope);
      ++maxScope;
      break;
    case RIGHT_CURLY_BRACKET:
      currentScope = currentScope->parent;
      break;
    default:
      break;
  }
  if (TraceScan) {
    fprintf(listing,"\t%d: ", lineno);

    /* DEBUG PURPOSES. SHOULD REMOVE LATER. */ 
    switch(currentToken) {
      case LEFT_CURLY_BRACKET:
      case RIGHT_CURLY_BRACKET:
        fprintf(listing,"\tCurrent scope:%d\n", currentScope->scope);
        break;
      default:
        break;
    }
    /* DEBUG PURPOSES. SHOULD REMOVE LATER. */
    printToken(currentToken, tokenString);
  }
  return currentToken;
}