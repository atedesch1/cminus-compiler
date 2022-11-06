#ifndef _SCAN_H_
#define _SCAN_H_

/* Maximum size of a token */
#define MAXTOKENLEN 40

/* Stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];

/* Stores the name of the last ID */
extern char idName[MAXTOKENLEN + 1];

/* Stores the value of the last NUM */
extern int numValue;

/* Returns the 
 * next token in source file
 */
TokenType getToken(void);

#endif
