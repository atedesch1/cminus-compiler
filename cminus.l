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

/* Additional states of FSM to identify scope */
enum ScopeStatusEnum {INIT_STATE, SEEN_LEFT_PARENTHESIS, SEEN_TYPE, INSIDE_FUNCTION};

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
  /* Variable for scope identification */
  static enum ScopeStatusEnum scopeStatus = INIT_STATE;

  /* Function starts here */
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
      if (scopeStatus != SEEN_TYPE && scopeStatus != INSIDE_FUNCTION) {
        // printf("Set to INIT_STATE inside ID\n");
        scopeStatus = INIT_STATE;
      }
      strncpy(idLexeme, yytext, MAXTOKENLEN);
      saveId(idLexeme);
      break;
    case NUM:
      if (scopeStatus != INSIDE_FUNCTION) {
        // printf("Set to INIT_STATE inside NUM\n");
        scopeStatus = INIT_STATE;
      }
      numValue = atoi(tokenString);
      break;
    case LEFT_PARENTHESIS:
      if (scopeStatus == INIT_STATE)
        scopeStatus = SEEN_LEFT_PARENTHESIS;
      break;
    case INT:
    case VOID:
      if (scopeStatus == SEEN_LEFT_PARENTHESIS) {
        /* new scope assuming that syntax analysis is correct */
        scopeStatus = SEEN_TYPE;
        currentScope = insertScope(currentScope, savedId[savedIdIndex], maxScope);
        ++maxScope;
      }
      break;
    case LEFT_CURLY_BRACKET:
      if (scopeStatus == SEEN_TYPE)
        scopeStatus = INSIDE_FUNCTION;
      else if (scopeStatus == INSIDE_FUNCTION) {
        currentScope = insertScope(currentScope, savedId[savedIdIndex], maxScope);
        ++maxScope;
      }
      // TODO: REMOVE printf("Current scope: %d ; scopeStatus: %d\n", currentScope->scope, scopeStatus);
      break;
    case RIGHT_CURLY_BRACKET:
      // if (currentScope->parent == NULL) // the error is here probably
      //  printf("We're here!\n");
      currentScope = currentScope->parent;
      if (currentScope->scope->id == -1)
        /* Return to the global scope */
        scopeStatus = INIT_STATE;
      // TODO: REMOVE printf("Current scope: %d ; scopeStatus: %d\n", currentScope->scope, scopeStatus);
      break;
    default:
      break;
  }
  if (TraceScan) {
    fprintf(listing,"\t%d: ", lineno);

    /* DEBUG PURPOSES. SHOULD REMOVE LATER. */ 
    /* switch(currentToken) {
      case LEFT_CURLY_BRACKET:
      case RIGHT_CURLY_BRACKET:
        fprintf(listing,"\tCurrent scope:%d\n", currentScope->scope);
        break;
      default:
        break;
    } */
    /* DEBUG PURPOSES. SHOULD REMOVE LATER. */
    printToken(currentToken, tokenString);
  }
  return currentToken;
}