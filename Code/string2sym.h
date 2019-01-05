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
**      FILENAME:   string2sym.h
**
**	PURPOSE:    Interface to string manager
**
**  MODIFICATION HISTORY:
**
**      {0} Aki      at Digital -- 89.12.05 -- Original
**      {1} Aki      at Digital -- 90.04.10 -- Support primary context only
**      {2} McKeeman at Digital -- 90.05.08 -- C to LSL
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/

/*
**
**  MACRO DEFINITIONS
**
*/

# define NULLSYMBOL ((LSLSymbol) -1)

/*
**
**  TYPEDEFS
**
*/

typedef Handle	    LSLSymbol;

/*
**
**  FUNCTION PROTOTYPES
**
*/

extern LSLSymbol    LSLString2Symbol(char *);
extern char        *LSLSymbol2String(LSLSymbol);

extern void	    LSLStringInit(void);
extern void	    LSLStringReset(void);
extern void	    LSLStringCleanup(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/

