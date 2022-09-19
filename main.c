#include "globals.h"
#include "scan.h"

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char * argv[]) {
  char program[120]; /* source code file name */

  if (argc != 2) {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  
  strcpy(program, argv[1]);
  if (strchr (program, '.') == NULL)
     strcat(program, ".cm");
  
  source = fopen(program, "r");
  if (source == NULL) { 
    fprintf(stderr, "File %s not found\n", program);
    exit(1);
  }

  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nCMINUS COMPILATION: %s\n", program);

  while (getToken() != ENDFILE);

  fclose(source);
  return 0;
}