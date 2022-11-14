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

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StatementKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
      fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
      for (i = 0; i < MAXCHILDREN; i++)
        t->child[i] = NULL;
      t->sibling = NULL;
      t->parent = NULL;
      t->nodekind = Statement;
      t->kind.stmt = kind;
      t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpressionKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
      fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
      for (i = 0; i < MAXCHILDREN; i++)
        t->child[i] = NULL;
      t->sibling = NULL;
      t->parent = NULL;
      t->nodekind = Expression;
      t->kind.exp = kind;
      t->lineno = lineno;
      t->type = VoidType;
  }
  return t;
}

/* Function newIdNode creates a new identifier
 * node for syntax tree construction
 */
TreeNode *newIdNode(IdKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->parent = NULL;
    t->nodekind = Id;
    t->kind.id = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newTypeNode creates a new TypeKind
 * node for syntax tree construction
 */
TreeNode *newTypeNode(TypeKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
      fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
      for (i = 0; i < MAXCHILDREN; i++)
        t->child[i] = NULL;
      t->sibling = NULL;
      t->parent = NULL;
      t->nodekind = Type;
      t->kind.type = kind;
      t->lineno = lineno;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
      return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
      fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
      strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
      fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
      printSpaces();
      if (tree->nodekind == Statement)
      {
        switch (tree->kind.stmt)
        {
        case If:
          fprintf(listing, "If\n");
          break;
        case Assign:
          fprintf(listing, "Assign\n");
          break;
        case While:
          fprintf(listing, "While\n");
          break;
        default:
          fprintf(listing, "Unknown StatementNode kind\n");
          break;
        }
      }
      else if (tree->nodekind == Expression)
      {
        switch (tree->kind.exp)
        {
        case Operator:
          fprintf(listing, "Op: ");
          printToken(tree->attr.op, "\0");
          break;
        case Constant:
          fprintf(listing, "Const: %d\n", tree->attr.val);
          break;
        case Return:
          fprintf(listing, "Return: \n");
          break;
        default:
          fprintf(listing, "Unknown ExpressionNode kind\n");
          break;
        }
      }
      else if (tree->nodekind == Id)
      {
        switch(tree->kind.id)
        {
        case Variable:
          fprintf(listing, "VariableId: %s\n", tree->attr.name);
#ifdef PRINT_DEBUG
// TODO : remove this later
          if (tree->scopeNode != NULL)
            fprintf(listing, "VaribleScope: %d\n", tree->scopeNode->scope);
#endif
          break;
        case Array:
          fprintf(listing, "ArrayId: %s\n", tree->attr.name);
#ifdef PRINT_DEBUG
// TODO : remove this later
          if (tree->scopeNode != NULL)
            fprintf(listing, "ArrayScope: %d\n", tree->scopeNode->scope);
#endif
          break;
        case Function:
          fprintf(listing, "FunctionId: %s\n", tree->attr.name);
#ifdef PRINT_DEBUG
// TODO : remove this later
          if (tree->scopeNode != NULL)
            fprintf(listing, "FunctionScope: %d\n", tree->scopeNode->scope);
#endif
          break;
        default:
          fprintf(listing, "Unknown IdNode kind\n");
          break;
        }
      }
      else if (tree->nodekind == Type) 
      {
        switch(tree->kind.type)
        {
        case Void:
          fprintf(listing, "Type: Void\n");
          break;
        case Int:
          fprintf(listing, "Type: Int\n");
          break;
        default:
          fprintf(listing, "Unknown TypeNode kind\n");
          break;
        }
      }
      else
        fprintf(listing, "Unknown node kind\n");
      for (i = 0; i < MAXCHILDREN; i++)
        printTree(tree->child[i]);
      if (tree->sibling != NULL) {
        printSpaces();
        fprintf(listing, "||\n");
      }
      tree = tree->sibling;
  }
  UNINDENT;
}