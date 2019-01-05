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
**      FILENAME:   syntable.h
**
**	PURPOSE:    Interface to the synonym table
**
**	USAGE:	# include "general.h"
**		# include "string2sym.h"
**		# include "scan.h"
**		# include "syntable.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Wild      at Digital -- 90.08.10 -- Original
**      {n} Who       at Where 	 -- yy.mm.dd -- What
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


extern void	    LSLAddSyn(LSLSymbol, LSLSymbol);
extern LSLToken	    LSLGetTokenForSyn(LSLSymbol);
extern bool	    LSLIsSyn(LSLSymbol);

extern void	    LSLSynTableInit(void);
extern void	    LSLSynTableReset(void);
extern void	    LSLSynTableCleanup(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/

