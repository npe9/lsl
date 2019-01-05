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
**      FILENAME:   source.h
**
**	PURPOSE:    Interface to source file input routines
**
**	USAGE:	# include "general.h"
**		# include "source.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.10.04 -- Abstract from drivers
**      {1} Wild    at Digital -- 91.02.06 -- Add source_getPath().
**	{2} Feldman at Digital -- 91.04.11 -- Add echoLh field to source struct
**					      and doLh param to source_create.
**	{3} Wild    at Digital -- 91.07.17 -- Move source_getPath to 
**					      osd_getPath in osdfiles.h.
**	{4} Wild    at Digital -- 91.07.25 -- Put source_getPath back, but have
**					      it call osd_getPath.
**	{5} Garland at MIT     -- 92.02.16 -- Add source_fromString
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/


/*
**  INCLUDE FILES
*/

# include <stdio.h>		/* for definition of FILE */


/*
**
**  MACRO DEFINITIONS
**
*/

# define STUBMAXRECORDSIZE 1024


/*
**
**  TYPEDEFS
**
*/

typedef struct {
    char	*name;
    FILE	*file;
    char	 buffer[STUBMAXRECORDSIZE+1];
    int		 lineNo;
    bool	 echo, echoLh, fromString;
    char	*stringSource, *stringSourceTail;
} source;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern bool	 source_close(source *s);
extern source	*source_create(char *name, char *suffix, bool echo,bool doLh);
extern source	*source_fromString(char *name, char *str);
extern char	*source_nextLine(source *s);
extern bool	 source_open(source *s);
extern bool	 source_getPath (char *path, source *s, LSLToken t);

extern char		*source_fileName(source *s);
# define 		 source_fileName(pS) 			\
    				((pS)->name)

extern bool		 source_isOpen(source *s);
# define		 source_isOpen(pS) 			\
    				((pS)->file != 0 || (pS)->fromString)

extern char		*source_thisLine(source *s);
# define 		 source_thisLine(pS) 			\
    				((pS)->buffer)

extern int		 source_thisLineNumber(source *s);
# define 		 source_thisLineNumber(pS) 		\
    				((pS)->lineNo)

/*
**
**  EXTERNAL VARIABLES (none)
**
*/
