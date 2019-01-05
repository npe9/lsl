/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:	LSL Checker
**
**  MODULE DESCRIPTION:
**
**  	FILENAME:   shift.c
**
**	PURPOSE:    Shift sequence manager
**	
**	NOTE:	    Tokens are sent by the parser to this module.  Here they 
**		    are saved in a stack and delivered on demand during reduce 
**		    actions.  The programmer must coordinate what the parser 
**		    sends and what reduce consumes.
**	
**  MODIFICATION HISTORY:
**
**  X0.1-1	WM	90.03.06	Original
**  X0.1-2	GAF	90.08.16	Add "Shifting:" to trace message
**  X0.1-3	JPW	90.08.17	Add IsEmptyShiftStack for init file.
**  X0.1-4	JPW	90.10.11	Remove "#include parse.h". Not needed
**  X0.1-5	JPW	90.11.19	Remove #include driver.h,scan.h
**  X0.1-6  	GAF	91.01.22	Allow shifting of nonterminals
**  X0.1-7	GAF	91.03.27	Add LSLGenTopmostToken and alphabetize
**  					routines.
**  X0.1-8	GAF	91.04.01	Change LSLGenTopmostToken to take an
**					integer count, indicating that we want
**					the Nth from the top.
**  X0.1-9	SJG	92.02.19	Reorder #include "error.h"
**  X0.1-10	SJG	93.08.03	Remove shifting of nonterminals,
**					LSLGenTopmostToken
**  [@modification history entry@]...
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include	<stdio.h>
# include		"general.h"
# include		"string2sym.h"
# include		"token.h"
# include		"error.h"
# include		"shift.h"

/*
**
**  MACRO DEFINITIONS
**
*/

# if TRACING == 1
#    define Trace(tok)          printf("Shifting: %s\n", token_text(tok) )
# else
#    define Trace(tok)
# endif

# define SHIFTMAX	200


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

extern bool		LSLGenIsEmptyShiftStack(void);
extern void		LSLGenPopShiftStack(void);
extern void		LSLGenShift(LSLToken tok);
extern LSLToken		LSLGenTopPopShiftStack(void);
extern LSLToken		LSLGenTopShiftStack(void);

extern void		LSLGenInit(void);
extern void		LSLGenReset(void);
extern void		LSLGenCleanup(void);


/*
**
**  STATIC VARIABLES
**
*/

LSLToken		Shifts[SHIFTMAX];
unsigned int		shiftIndex;

/*
**
**  FUNCTION DEFINITIONS
**
*/

extern bool
LSLGenIsEmptyShiftStack(void) {
    return (shiftIndex == 0);
}

extern void
LSLGenPopShiftStack(void) {
    if (LSLGenIsEmptyShiftStack()) {
        error_programFailure("LSLGenPopShiftStack() -- Empty stack");
    }
    --shiftIndex;
}

extern void
LSLGenShift(LSLToken tok) {
    Trace(tok);
    if (shiftIndex < SHIFTMAX) {
	Shifts[shiftIndex++] = tok;
     } else {
	error_programFailure("LSLGenShift() -- needs MoreMem()");
     }
}

extern LSLToken
LSLGenTopPopShiftStack(void) {
    if (LSLGenIsEmptyShiftStack()) {
        error_programFailure("LSLGenTopPopShiftStack() -- Empty stack");
    }
    return Shifts[--shiftIndex];
}

LSLToken
LSLGenTopShiftStack(void) {
    if (LSLGenIsEmptyShiftStack()) {
        error_programFailure("LSLGenTopShiftStack() -- Empty stack");
    }
    return Shifts[shiftIndex-1];
}

void
LSLGenInit(void) {
}

void
LSLGenReset(void) {
    shiftIndex = 0;					/* empty stack	*/
}

void
LSLGenCleanup(void) {
}


