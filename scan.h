#ifndef _SCAN_H_
#define _SCAN_H_

#include "globals.h"

/* Maximum size of a token */
#define MAXTOKENLEN 40

/* Stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];

/* Returns the 
 * next token in source file
 */
TokenType getToken(void);

#endif
