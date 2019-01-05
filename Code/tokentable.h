/*
**
**	Copyright (c) MIT, 1990, 1991, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   tokentable.h
**
**	PURPOSE:    Interface to the token table
**
**	USAGE:	# include "string2sym.h"
**		# include "token.h"
**		# include "tokentable.h"
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	JPW     90.08.09	Original
**  X0.1-1	SJG	93.11.26	Add SetPriority
**  [@modification history entry@]...
**--
*/

/*
**
**  MACRO DEFINITIONS (none)
**
*/

/*
**
**  TYPEDEFS
**
*/

/*
**
**  FUNCTION PROTOTYPES
**
*/


extern LSLToken	    LSLInsertToken(LSLTokenCode, LSLSymbol, LSLSymbol, bool);
extern void	    LSLUpdateToken(LSLTokenCode, LSLSymbol, bool);
extern void	    LSLSetTokenHasSyn(LSLSymbol, bool);
extern void	    LSLSetTokenPriority(LSLSymbol, int);
extern LSLToken	    LSLGetToken(LSLSymbol);
extern LSLToken     LSLReserveToken(LSLTokenCode, char *);

extern void	    LSLTokenTableInit(void);
extern void	    LSLTokenTableReset(void);
extern void	    LSLTokenTableCleanup(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/

