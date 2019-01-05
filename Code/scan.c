/*
**
**	Copyright (c) MIT, 1990, 1991, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   scan.c
**
**	PURPOSE:    Deliver tokens one at a time
**
**	METHOD:
**	The input arrives via LSLScanFreshToken().
**	The output is reported via LSLScanNextToken().
**
**	The tokens are built in module ScanLine.
**	The tokens are delivered from this module.
**	Meantimes, they are saved in a static array.
**	
**	The tokenizing is split off from the delivery of tokens
**	to facilitate incremental scanning at a later date.
**	The essential is that scanline() can be called or not
**	if the input text is dirty or not.  Clean lines cause
**	tokens to be played out from the saved token list (not
**	yet implemented in this version).
**
**  MODIFICATION HISTORY:
**
**  	{0} McKeeman at Digital -- 90.05.09 -- Original
**  	{1} Wild     at Digital -- 90.08.15 -- Update token names to 2.3
**	{2} Garland  at MIT	-- 90.10.04 -- Update for source.h
**	{3} Wild     at Digital	-- 91.02.07 -- Reorder includes for
**					       source_getPath().
**	{4} Garland  at MIT     -- 92.02.19 -- Reorder #include "error.h"
**	{n} Who      at Where   -- yy.mm.dd -- did what?
**
*/

/*
**
**  INCLUDE FILES
**
*/

# include <string.h>
# include	"general.h"
# include	"string2sym.h"
# include	"token.h"
# include	"error.h"
# include	"source.h"
# include	"scan.h"
# include	"scanline.h"

/*
**
**  MACRO DEFINITIONS
**
*/

# define MAXLINE    1000			/* max tokens on one line  */
# define CompilerFailure error_programFailure


/*
**
**  TYPEDEFS (none)
**
*/

/*
**
**  FORWARD FUNCTIONS
**
*/

extern LSLToken		LSLScanNextToken();	/* frequent entry   */
extern LSLToken		LSLScanLookAhead();

extern void		LSLScanFreshToken();	/* frequent entry   */

extern source	       *LSLScanSource();	/* current source   */
extern void		LSLScanInit();		/* setup	    */
extern void		LSLScanReset();		/* warm start	    */
extern void		LSLScanCleanup();	/* b4 warm start    */

/*
**
**  STATIC VARIABLES
**
*/

static source		*scanFile;		/* file to scan		*/
static LSLToken		TokenList[MAXLINE];	/* available tokens	*/
static int		nextToken;		/* next available token */
static int		lastToken;		/* next available slot	*/

static char		*line;			/* input text		*/
static unsigned int	lineNumber;		/* current line number	*/

/*
**
**  FUNCTION DEFINITIONS
**
*/

LSLToken
LSLScanNextToken(void) {
    if (nextToken < lastToken) {		/* token available	*/
	return TokenList[nextToken++];
    } else {
	lastToken   = 0;			/* get new list		*/
	lineNumber++;
	line	    = source_nextLine(scanFile);/* source text		*/
	if (line != (char *) 0) {
	    LSLScanLine(line);			/* tokenize		*/
	    nextToken = 0;			/* first available	*/
	    return LSLScanNextToken();		/* get it		*/
	} else {
	    return LSLScanEofToken();		/* end of file		*/
	}
    }
}

LSLToken
LSLScanLookAhead(void) {
    if (nextToken < lastToken) {		/* token available	*/
	return TokenList[nextToken];
    } else {
	lastToken   = 0;			/* get new list		*/
	line	    = source_nextLine(scanFile);/* source text		*/
	if (line != (char *) 0) {
	    LSLScanLine(line);			/* tokenize		*/
	    nextToken = 0;			/* first available	*/
	    return LSLScanLookAhead();		/* get it		*/
	} else {
	    return LSLScanEofToken();		/* end of file		*/
	}
    }
}

void
LSLScanFreshToken(LSLToken tok) {
    if (lastToken < MAXLINE) {			/* there is room	*/
	TokenList[lastToken++] = tok;		/* append token to list	*/
    } else {
	CompilerFailure("LSLScanFreshToken()");
    }
}


source
*LSLScanSource(void) {
    return scanFile;
}


void
LSLScanInit(void) {
}

void
LSLScanReset(source *s) {
    scanFile  = s;
    lastToken = 0;
    nextToken = lastToken+1;			/* force call to scanline   */
    lineNumber = 0;
}

void
LSLScanCleanup(void) {
}
