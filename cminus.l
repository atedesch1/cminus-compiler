%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];
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
                      case '\\':
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
  }
  currentToken = yylex();
  strncpy(tokenString, yytext, MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;
}
