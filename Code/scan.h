/*
**
**	Copyright (c) MIT, 1990
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   scan.h
**
**	PURPOSE:    Interface to Larch SL scanner
**	
**	The scanner is responsible for taking source text and producing
**	a sequence of tokens.  
**
**	USAGE:	# include "general.h"
**		# include "token.h"
**		# include "source.h"
**		# include "scan.h"
**	
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at Digital -- 90.04.20 -- Original
**	{1} Garland  at MIT	-- 90.05.18 -- Add eol token
**	{2} Feldman  at Digital	-- 90.07.23 -- Rename __TOKEN to MARKERSYM
**					       Add SELECTSYM
**      {3} Wild     at Digital -- 90.08.15 -- Use 2.3 token names for enums
**      {4} Wild     at Digital -- 90.08.23 -- Add hasSyn field to LSLToken
**	{5} Garland  at MIT	-- 90.10.03 -- Split token.h from scan.h
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS (none)
**
*/


/*
**
**  TYPEDEFS (none)
**
*/


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern LSLToken		LSLScanNextToken(void);		/* frequent entry   */
extern LSLToken		LSLScanLookAhead(void);

extern void		LSLScanFreshToken(LSLToken tok);/* frequent entry   */

extern source	       *LSLScanSource(void);		/* current source   */
extern void		LSLScanInit(void);		/* setup	    */
extern void		LSLScanReset(source *s);	/* warm start	    */
extern void		LSLScanCleanup(void);		/* b4 warm start    */


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
