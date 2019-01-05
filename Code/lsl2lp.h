/*
**
**	Copyright (c) MIT, 1991
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   lsl2lp.h
**
**	PURPOSE:    Interface to LP output routines
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 91.12.20 -- Original
**      {1} Garland at MIT   -- 92.01.23 -- Add -tr option support
**      {n} Who     at Where -- yy.mm.dd -- What
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

extern void	outputLP (traitNodeList *allTraits, char **trFiles, 
			  int nTrFiles);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
