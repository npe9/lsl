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
**  	FILENAME:   spec_stdlib.h
**
**  	PURPOSE:    Common Tools
**
**	NOTE:	    This module is a substitute for stdlib.h.  We include our 
**		    own because we've had several problems locating stdlib.h 
**		    on various systems.
**
**  AUTHORS:
**
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  CREATION DATE:  10 Apr 1991
**
**  MODIFICATION HISTORY:
**
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
**  FUNCTION PROTOTYPES
**
*/

extern char *getenv(const char *);
extern void free(void *);
extern void *realloc(void *, unsigned);
extern void *calloc(unsigned, unsigned);
extern void *malloc(unsigned);
                                                      
