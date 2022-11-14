%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode*
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);

%}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token PLUS MINUS TIMES OVER LESS_THAN LESS_EQUAL_THAN GREATER_THAN GREATER_EQUAL_THAN 
             EQUAL DIFF ASSIGN SEMICOLON COMMA LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKET 
             RIGHT_SQUARE_BRACKET LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token ERROR

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
var_declaracao      : tipo_especificador ID SEMICOLON {
                      $$ = $1;
                      $$->child[0] = newIdNode(Variable);
                      $$->child[0]->attr.name = copyString(popId());
                      $$->child[0]->parent = $$;
                      $$->child[0]->lineno = lineno;
                      $$->child[0]->scopeNode = currentScope;
                    }
                    | tipo_especificador ID LEFT_SQUARE_BRACKET NUM RIGHT_SQUARE_BRACKET SEMICOLON {
                      $$ = $1;
                      $$->child[0] = newIdNode(Array);
                      $$->child[0]->attr.name = copyString(popId());
                      $$->child[0]->parent = $$;
                      $$->child[0]->lineno = lineno;
                      $$->child[0]->child[0] = newExpNode(Constant);
                      $$->child[0]->child[0]->attr.val = numValue;
                      $$->child[0]->scopeNode = currentScope;
                    }
                    ;
tipo_especificador  : INT { $$ = newTypeNode(Int); }
                    | VOID { $$ = newTypeNode(Void); }
                    ;
fun_declaracao      : tipo_especificador ID LEFT_PARENTHESIS params RIGHT_PARENTHESIS composto_decl {
                      $$ = $1;
                      $$->child[0] = newIdNode(Function);
                      $$->child[0]->attr.name = copyString(popId());
                      $$->child[0]->parent = $$;
                      $$->child[0]->lineno = lineno;
                      $$->child[0]->scopeNode = currentScope;
                      $$->child[0]->child[0] = $4;
                      $$->child[0]->child[1] = $6;
                    }
                    ;
params              : param_lista { $$ = $1; }
                    | VOID { $$ = NULL; }
                    ;
param_lista         : param_lista COMMA param {
                      YYSTYPE t = $1;
                      if (t != NULL) {
                        while (t->sibling != NULL) {
                          t = t->sibling;
                        }
                        t->sibling = $3;
                        $$ = $1;
                      }
                      else $$ = $1;
                    }
                    | param { $$ = $1; }
                    ;
param               : tipo_especificador ID { 
                      $$ = $1;
                      $$->child[0] = newIdNode(Variable);
                      $$->child[0]->parent = $$;
                      $$->child[0]->lineno = lineno;
                      $$->child[0]->attr.name = copyString(popId());
                      /* TODO: we need to add a scope to the function's param */
                    }
                    | tipo_especificador ID LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET {
                      $$ = $1;
                      $$->child[0] = newIdNode(Array);
                      $$->child[0]->parent = $$;
                      $$->child[0]->lineno = lineno;
                      $$->child[0]->attr.name = copyString(popId());
                      /* TODO: we need to add a scope to the function's param */
                    }
                    ;
composto_decl       : LEFT_CURLY_BRACKET local_declaracoes statement_lista RIGHT_CURLY_BRACKET {
                      YYSTYPE t = $2;
                      if (t != NULL) {
                        while (t->sibling != NULL) {
                          t = t->sibling;
                        }
                        t->sibling = $3;
                        $$ = $2;
                      }
                      else $$ = $3;
                    }
                    ;
local_declaracoes   : local_declaracoes var_declaracao {
                      YYSTYPE t = $1;
                      if (t != NULL) {
                        while (t->sibling != NULL) {
                          t = t->sibling;
                        }
                        t->sibling = $2;
                        $$ = $1;
                      }
                      else $$ = $2;
                    }
                    | %empty { $$ = NULL; }
                    ;
statement_lista     : statement_lista statement { 
                      YYSTYPE t = $1;
                      if (t != NULL) {
                        while (t->sibling != NULL) {
                          t = t->sibling;
                        }
                        t->sibling = $2;
                        $$ = $1;
                      }
                      else $$ = $2;
                    }
                    | %empty { $$ = NULL; }
                    ;
statement           : expressao_decl  { $$ = $1; }
                    | composto_decl   { $$ = $1; }
                    | selecao_decl    { $$ = $1; }
                    | iteracao_decl   { $$ = $1; }
                    | retorno_decl    { $$ = $1; }
                    ;
expressao_decl      : expressao SEMICOLON { $$ = $1; }
                    | SEMICOLON { $$ = NULL; }
                    ;
selecao_decl        : IF LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement { 
                      $$ = newStmtNode(If);
                      $$->child[0] = $3;
                      $$->child[1] = $5;
                    }
                    | IF LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement ELSE statement { 
                      $$ = newStmtNode(If);
                      $$->child[0] = $3;
                      $$->child[1] = $5;
                      $$->child[2] = $7;
                    }
                    ;
iteracao_decl       : WHILE LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS statement {
                      $$ = newStmtNode(While);
                      $$->child[0] = $3;
                      $$->child[1] = $5;
                    }
                    ;
retorno_decl        : RETURN SEMICOLON { 
                      $$ = newExpNode(Return);
                      $$->type = VoidType;
                    }
                    | RETURN expressao SEMICOLON { 
                      $$ = newExpNode(Return);
                      $$->child[0] = $2;
                    }
                    ;
expressao           : var ASSIGN expressao { 
                      $$ = newStmtNode(Assign);
                      $$->child[0] = $1; /* convention: assigned variable is the left child */
                      $$->child[1] = $3;
                      $$->child[0]->parent = $$;
                      $$->child[1]->parent = $$;
                      // $$->type = $$->child[1]->type; /* todo: review this line */
                    }
                    | simples_expressao { $$ = $1; }
                    ;
var                 : ID { 
                      $$ = newIdNode(Variable);
                      $$->attr.name = copyString(popId());
                      $$->lineno = lineno;
                      $$->scopeNode = currentScope;
                    }
                    | ID LEFT_SQUARE_BRACKET expressao RIGHT_SQUARE_BRACKET { 
                      $$ = newIdNode(Array);
                      $$->attr.name = copyString(popId());
                      $$->lineno = lineno;
                      $$->child[0] = $3;
                      $$->scopeNode = currentScope;
                    }
                    ;
simples_expressao   : soma_expressao relacional soma_expressao { 
                      $$ = $2;
                      $$->child[0] = $1;
                      $$->child[1] = $3;
                    }
                    | soma_expressao { $$ = $1; }
                    ;
relacional          : LESS_EQUAL_THAN     { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = LESS_EQUAL_THAN;
                    }
                    | LESS_THAN { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = LESS_THAN;
                    }
                    | GREATER_THAN { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = GREATER_THAN;
                    }
                    | GREATER_EQUAL_THAN { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = GREATER_EQUAL_THAN;
                    }
                    | EQUAL { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = EQUAL;
                    }
                    | DIFF { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = DIFF;
                    }
                    ;
soma_expressao      : soma_expressao soma termo { 
                      $$ = $2;
                      $$->child[0] = $1;
                      $$->child[1] = $3;
                    }
                    | termo { $$ = $1; }
                    ;
soma                : PLUS { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = PLUS;
                    }
                    | MINUS { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = MINUS;
                    }
                    ;
termo               : termo mult fator { 
                      $$ = $2;
                      $$->child[0] = $1;
                      $$->child[1] = $3;
                    }
                    | fator { $$ = $1; }
                    ;
mult                : TIMES { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = TIMES;
                    }
                    | OVER { 
                      $$ = newExpNode(Operator);
                      $$->attr.op = OVER;
                    }
                    ;
fator               : LEFT_PARENTHESIS expressao RIGHT_PARENTHESIS { $$ = $2; }
                    | var { $$ = $1; }
                    | ativacao { $$ = $1; }
                    | NUM { 
                      $$ = newExpNode(Constant);
                      $$->attr.val = numValue;
                      $$->type = IntegerType;
                    }
                    ;
ativacao            : ID LEFT_PARENTHESIS args RIGHT_PARENTHESIS { 
                      $$ = newIdNode(Function);
                      $$->attr.name = copyString(popId()); 
                      $$->lineno = lineno;
                      $$->child[0] = $3;
                      $$->scopeNode = currentScope;
                    }
                    ;
args                : arg_lista { $$ = $1; }
                    | %empty { $$ = NULL; }
                    ;
arg_lista           : arg_lista COMMA expressao {
                      YYSTYPE t = $1;
                      if (t != NULL) {
                        while (t->sibling != NULL) {
                          t = t->sibling;
                        }
                        t->sibling = $3;
                        $$ = $1;
                      }
                      else $$ = $3;
                    }
                    | expressao { $$ = $1; }
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
