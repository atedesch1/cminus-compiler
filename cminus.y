%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode*
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);

%}

%union {
  TreeNode *node;
}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token PLUS MINUS TIMES OVER LESS_THAN LESS_EQUAL_THAN GREATER_THAN GREATER_EQUAL_THAN 
       EQUAL DIFF ASSIGN SEMICOLON COMMA LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKET 
       RIGHT_SQUARE_BRACKET LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token ERROR

%type <node>
declaracao_lista declaracao var_declaracao tipo_especificador 
fun_declaracao params param_lista param
composto_decl local_declaracoes statement_lista statement
expressao_decl selecao_decl iteracao_decl retorno_decl 
expressao var simples_expressao relacional 
soma_expressao soma termo mult 
fator ativacao args arg_lista

%% /* Grammar for CMINUS */

programa            : declaracao_lista { savedTree = $1; }
                    ;
declaracao_lista    : declaracao_lista declaracao { 
                        YYSTYPE t = $1;
                        if (t != NULL) {
                          while (t->sibling != NULL) {
                            t = t->sibling;
                          }
                          t->sibling = $2;
                          $$ = $1;
                        }
                        else $$ = $1;
                      }
                    | declaracao { $$ = $1; }
                    ;
declaracao          : var_declaracao { $$ = $1; }
                    | fun_declaracao { $$ = $1; }
                    ;
var_declaracao      : INT ID SEMICOLON {
                        $$ = newStmtNode(VariableDeclaration);
                        $$->attr.name = copyString(idName);
                        $$->type = Integer;
                      }
                    | INT ID LEFT_SQUARE_BRACKET NUM RIGHT_SQUARE_BRACKET SEMICOLON {
                        $$ = newStmtNode(ArrayDeclaration);
                        $$->attr.name = copyString(idName);
                        $$->type = Integer;
                        $$->child[0] = newExpNode(ConstK);
                        $$->child[0]->attr.val = numValue;
                      }
                    ;
tipo_especificador  : INT { $$ = $1; }
                    | VOID { $$ = $1; }
                    ;
fun_declaracao      : tipo_especificador ID LEFT_PARENTHESIS params RIGHT_PARENTHESIS composto_decl { }
                    ;
params              : param_lista { }
                    | VOID { }
                    ;
param_lista         : param_lista COMMA param { }
                    | param                   { }
                    ;
param               : tipo_especificador ID                                          { }
                    | tipo_especificador ID LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET { }
                    ;
composto_decl       : LEFT_CURLY_BRACKET local_declaracoes statement_lista RIGHT_CURLY_BRACKET { }
                    ;
local_declaracoes   : local_declaracoes var_declaracao { }
                    | %empty { }
                    ;
statement_lista     : statement_lista statement { }
                    | %empty { }
                    ;
statement           : expressao_decl  { }
                    | composto_decl   { }
                    | selecao_decl    { }
                    | iteracao_decl   { }
                    | retorno_decl    { }
                    ;
expressao_decl      : expressao SEMICOLON { }
                    | SEMICOLON { }
                    ;
selecao_decl        : IF LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement { }
                    | IF LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement ELSE statement { }
                    ;
iteracao_decl       : WHILE LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement { }
                    ;
retorno_decl        : RETURN SEMICOLON           { }
                    | RETURN expressao SEMICOLON { }
                    ;
expressao           : var ASSIGN expressao { }
                    | simples_expressao    { }
                    ;
var                 : ID { }
                    | ID LEFT_SQUARE_BRACKET expressao RIGHT_SQUARE_BRACKET { }
                    ;
simples_expressao   : soma_expressao relacional soma_expressao { }
                    | soma_expressao { }
                    ;
relacional          : LESS_EQUAL_THAN     { }
                    | LESS_THAN           { }
                    | GREATER_THAN        { }
                    | GREATER_EQUAL_THAN  { }
                    | EQUAL               { }
                    | DIFF                { }
                    ;
soma_expressao      : soma_expressao soma termo { }
                    | termo                     { }
                    ;
soma                : PLUS  { }
                    | MINUS { }
                    ;
termo               : termo mult fator { }
                    | fator            { }
                    ;
mult                : TIMES { }
                    | OVER  { }
                    ;
fator               : LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS { }
                    | var                                          { }
                    | ativacao                                     { }
                    | NUM                                          { }
                    ;
ativacao            : ID LEFT_PARENTHESIS args RIGHT_PARENTHESIS { }
                    ;
args                : arg_lista { }
                    | %empty { }
                    ;
arg_lista           : arg_lista COMMA expressao { }
                    | expressao                 { }
                    ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
