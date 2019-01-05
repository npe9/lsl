/*
**
**	Copyright (c) MIT, 1990, 1991
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   scanline.h
**
**	PURPOSE:    Interface to Larch SL scanner engine
**	
**	This module delivers source lines one at a time to the scanner.
** 
**	USAGE:	# include <string.h>
**		# include 	"general.h"
**		# include	"token.h"
**		# include 	"scan.h"
**		# include	"scanline.h"
**	
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at Digital -- 90.04.20 -- Original
**	{1} Garland  at MIT 	-- 90.05.18 -- Add EOL tokens
**	{2} Feldman  at Digital -- 90.07.30 -- Add characterClass array
**	{3} Feldman  at Digital -- 90.08.14 -- Set NULL to end comments
**      {4} Wild     at Digital -- 90.08.16 -- Add init file processing
**	{5} Wild     at Digital -- 90.08.30 -- Fix minor gcc errors.  Remove
**					   	((c) > MAXCHAR) checks.
**	{6} Garland  at MIT	-- 90.09.06 -- Fix comment tokens
**      {7} Garland  at MIT	-- 90.10.05 -- Move keyword tokens to token.c
**	{8} Feldman  at Digital -- 91.05.30 -- Add SLASHCHAR character code.
**      {n} Who      at Where 	-- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define LASTCHAR	    255
# define EXTENDCHARDEF	    '\\'


/*
**
**  TYPEDEFS
**
*/

typedef int character;		    /* to allow for internationalization    */

typedef enum {
    NULLCHARCLASS,
    IDCHAR,
    OPCHAR,
    SLASHCHAR,
    WHITECHAR,
    EXTENSIONCHAR,
    SINGLECHAR,
    PERMCHAR,
    BADCHARCLASS
}		charCode;

typedef struct {
    charCode	code;
    bool	endCommentChar;
} charClassData;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void	    LSLScanLine(char *line);
extern void	    LSLScanOption(int flagName, int flagValue);
extern LSLToken	    LSLScanEofToken(void);
extern void	    LSLReportEolTokens(bool setting);
extern void	    LSLReportCommentTokens(bool setting);

extern void	    LSLScanLineInit(void);
extern void	    LSLScanLineReset(void);
extern void	    LSLScanLineCleanup(void);

/* 
** The following function prototypes are for use by lslinit.c, which
** customizes subsequent actions of scanline.c.
*/

extern charCode	    LSLScanCharClass(character c);
# define	    LSLScanCharClass(c)					    \
		       (charClass[(c)].code)

extern bool	    LSLIsEndComment(character c);
# define	    LSLIsEndComment(c)					    \
		       (charClass[(c)].endCommentChar)

extern void	    LSLSetCharClass(character c, charCode cod);
# define	    LSLSetCharClass(c, cod)				    \
		       (charClass[(c)].code = (cod))
extern void	    LSLSetEndCommentChar(character c, bool flag);
# define	    LSLSetEndCommentChar(c, flag)			    \
		       (charClass[(c)].endCommentChar = (flag))

/*
**
**  EXTERNAL VARIABLES
**
*/

extern		charClassData charClass[LASTCHAR + 1];
