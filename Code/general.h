/*
**
**	Copyright (c) MIT, 1990, 1991, 1994
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   general.h
**
**	PURPOSE:    Interface to low level abstractions
**	
**  MODIFICATION HISTORY:
**
**	{0} Garland at MIT      -- 90.10.04 -- Extract from drivers
**	{1} Wild    at Digital	-- 91.07.25 -- Move SUCCESS and FAILURE to
**					       osdfiles.h because OS depend.
**	{2} Garland at MIT      -- 94.04.20 -- Use #ifndef around TRUE, FALSE
**      {n} Who     at Where    -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# ifndef FALSE
# define FALSE		    0
# define TRUE		    (!FALSE)
# endif


/*
**
**  TYPEDEFS
**
*/

typedef int		    bool;
typedef long unsigned	    Handle;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void *LSLMoreMem(void *ptr, int sz);
extern void  LSLFreeMem(void *ptr);
extern char *string_paste(char *s1, char *s2);


/*
**
**  EXTERNAL VARIABLES
**
*/
