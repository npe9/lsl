/*
**
**	Copyright (c) MIT, 1990, 1991, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright
** 	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   error.c
**
**	PURPOSE:    Error handling procedures
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT      -- 90.09.27 -- Original
**	{1} Wild    at Digital  __ 91.07.26 -- Include osdfiles.h
**	{2} Garland at MIT	-- 92.02.19 -- Add error_reportBadToken
**      {n} Who     at Where    -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include "osdfiles.h"
# include <stdio.h>
# include "general.h"
# include "string2sym.h"
# include "token.h"
# include "error.h"

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
**  FORWARD FUNCTIONS
**
*/

extern void	error_resetCount();
extern int	error_count();

extern void	error_report();
extern void	error_reportAndFail();
extern void	error_reportAndQuit();
extern void	error_reportLocation();
extern void	error_point();

extern void	error_programFailure();
extern void	error_systemFailure();



/*
**
**  STATIC VARIABLES
**
*/

static int errors = 0;
jmp_buf	   error_quitData;


/*
**
**  FUNCTION DEFINITIONS
**
*/

void
error_resetCount (void) {
    errors = 0;
}


int
error_count (void) {
    return errors;
}


extern void
error_reportBadToken (LSLToken *tok, char *msg) {
error_reportLocation(token_fileName(*tok), token_line(*tok), token_col(*tok));
     printf("`%s\' ", token_text(*tok));                                 \
     error_report(msg);
}



void
error_reportLocation (char *filename, unsigned int line, unsigned int col) {
    fprintf(stdout, "%s:%d,%d: ", filename, line, col);
}

void
error_point(char *line, unsigned int col) {
    int i;
    puts(line);
    for (i=0; i < col; i++) putchar(' ');
    puts("^");
}

void
error_report (char *msg) {
    errors++;
    puts(msg);
}

void
error_reportAndFail (char *msg) {
    error_report(msg);
    exit(FAILURE);
}

void
error_reportAndQuit (char *msg) {
    error_report(msg);
    longjmp(error_quitData, 1);
}

void		
error_programFailure (char *msg) {
    printf("Program failure: %s\n", msg);
    exit(FAILURE);
}

void
error_systemFailure (char *msg) {
    printf("System failure: %s\n", msg);
    exit(FAILURE);
}

