/*
**
**	Copyright (c) MIT, 1990, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   error.h
**
**	PURPOSE:    Interface to error handling routines
**
**	USAGE:	# include "general.h"
**		# include "string2sym.h"
**		# include "token.h"
**		# include "error.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT      -- 90.09.27 -- original
**	{1} Garland at MIT	-- 92.02.19 -- Add error_reportBadToken
**      {n} Who     at Where    -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

/* 
** Ordinarily, we don't put #includes in .h files.  But here we don't want
** anyone else messing with setjmp, and we want to hide its use so that
** clients of error.c don't need  "#include <setjmp.h>" in addition to
** "#include error.h".  
**
** We also need to define a couple of macros for the case of a non-DEC ANSI
** C compiler running on MIPS/ULTRIX 4.2 and later.  This is because the
** standard library file doesn't allow for such a case.
*/

# if defined(mips) && !defined(__mips)
#   define __mips mips
# endif

# if defined(vax) && !defined(__vax)
#   define __vax vax
# endif

# if defined(LANGUAGE_C) && !defined(__LANGUAGE_C)
#   define __LANGUAGE_C LANGUAGE_C
# endif

# include <setjmp.h>


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

typedef struct {
    char		*fileName;	/* file containing error 	  */
    unsigned int	 line;		/* line in file (1 is first) 	  */
    unsigned int	 col;		/* character in line (0 is first) */
} locator;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void	error_resetCount(void);
extern int	error_count(void);

extern void	error_report(char *msg);
extern void	error_reportAndFail(char *msg);
extern void	error_reportAndQuit(char *msg);
extern void	error_reportBadToken(LSLToken *tok, char *msg);
extern void	error_reportLocation(char *filename, unsigned int line,
				     unsigned int col);

extern void	error_point(char *line, unsigned int col);

extern void	error_programFailure(char *msg);
extern void	error_systemFailure(char *msg);

extern int	error_setQuit(void);
# define error_setQuit() setjmp(error_quitData)


/*
**
**  EXTERNAL VARIABLES
**
*/

extern jmp_buf	    error_quitData;
