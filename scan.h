#ifndef _SCAN_H_
#define _SCAN_H_

/* Maximum size of a token */
#define MAXTOKENLEN 40

/* Stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];

/* Stores the name of the last ID */
extern char idLexeme[MAXTOKENLEN + 1];

/* Stores the value of the last NUM */
extern int numValue;

/* stores the lexeme of ID tokens */
extern char *savedId[128];
extern int savedIdIndex;
extern char *popId();

/* Returns the 
 * next token in source file
 */
TokenType getToken(void);

#endif
