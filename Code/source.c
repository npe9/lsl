/*
**
**	Copyright (c) MIT, 1990, 1991, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   source.c
**
**	PURPOSE:    Interface to source file abstraction
**
**	NOTE:	    This module is almost identical to the one for LCL.  The
**		    only difference is that a couple of source lines have been
**		    commented out.
**
**		    This module has too many dependencies to be in the common
**		    source area.  Any of the solutions that would allow this
**		    module to be common had its own set of compromises.  It
**		    seemed best and most straightforward to just keep separte
**		    copies for LSL and LCL.  We should examine this again if we
**		    ever reorganize the module structure.
**
**  AUTHORS:
**
**     Steve Garland, Massachusetts Institute of Technology
**     Joe Wild, Technical Languages and Environments, DECspec project
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	SG	90.10.04	Original (abstracted from drivers)
**  X0.1-2	JPW	90.11.12	Call strrchr instead of rindex.
**  X0.1-3	SG/JPW	91.02.04	Add echoing of each source line	
**  X0.1-4	JPW	91.02.06	Add source_getPath.
**  X0.1-5	GAF	91.02.11	Add include of STDLIB (for getenv)
**  X0.1-6	JPW     91.02.21	Merge in MIT changes.
**  X0.1-7	GAF	91.03.22	Copy changes from lsl version:
**					Allow null token to be passed to 
**  					source_getPath.
**					Split source_getPath into two smaller
**					routines.
**					Abstract out yet another routine
**					(nextdir) from source_getPath.
**  X0.1-8	GAF	91.04.30	Add .lh file echoing.
**					static variable buffer - not used.
**  X0.1-9	JPW	91.07.17	Move source_getPath to osd_getPath
**					in osdfiles.c.
**  X0.1-10	JPW	91.07.25	Put source_getPath back, but have it
**					call osd_getPath.
**  X0.1-11	JPW	91.08.15	Put source back into lsl and lcl.
**					Include problems in cmn.
**  X0.1-12     SJG     92.02.16	Add source_fromString
**  X0.1-13	SJG	92.02.19	Use error_reportBadToken
**  [@modification history entry@]...
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include	"osdfiles.h"
# include	STDLIB
# include	<stdio.h>
# include	<string.h>
# include	"general.h"
# include	"string2sym.h"
# include	"token.h"
# include 	"error.h"
# include	"source.h"


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

extern bool	 source_close(source *s);
extern source	*source_create(char *name, char *suffix, bool echo, bool doLh);
extern source	*source_fromString(char *name, char *str);
extern char	*source_nextLine(source *s);
extern bool	 source_open(source *s);
extern bool	 source_getPath (char *path, source *s, LSLToken t);

static void	 CompilerFailure();
static void	 UserError(char *msg);
static void	 UserErrorLoc(LSLToken t, char *msg);

/*
**
**  STATIC VARIABLES (none)
**
*/


/*
**
**  FUNCTION DEFINITIONS
**
*/


extern bool
source_close (source *s) {
    if (s->file == 0) return FALSE;
    fclose(s->file);
    s->file = 0;
    return TRUE;
}


extern source
*source_create (char *name, char *suffix, bool echo, bool doLh) {
    char 	*ps;
    source 	*s;
    s = (source *)LSLMoreMem((void *)0, sizeof(source));
    s->name = (char *)LSLMoreMem((void *)0, strlen(name) + strlen(suffix) + 1);
    s->file = 0;
    strcpy(s->name, name);
    ps = strrchr(s->name, '/');
    if (ps == (char *)0) ps = s->name;
    if (strchr(ps, '.') == NULL) {
	strcat(s->name, suffix);
    }
    s->echo = echo;
    s->echoLh = doLh;
    s->fromString = FALSE;
    return s;
}

extern source
*source_fromString (char *name, char *str) {
    source *s = (source *)LSLMoreMem((void *)0, sizeof(source));
    s->name = (char *)LSLMoreMem((void *)0, strlen(name) + 1);
    strcpy(s->name, name);
    s->stringSource = (char *)LSLMoreMem((void *)0, strlen(str) + 1);
    strcpy(s->stringSource, str);
    s->stringSourceTail = s->stringSource;
    s->file = 0;
    s->echo = FALSE;
    s->echoLh = FALSE;
    s->fromString = TRUE;
    return s;
}


extern char *
source_nextLine(source *s) {
    char *currentLine;
    int len;
    if (s->fromString) {
	if (strlen(s->stringSourceTail) == 0) {
	    currentLine = 0;
	} else {
	    char *c = strchr(s->stringSourceTail, '\n');
	    len = c - s->stringSourceTail + 1;
	    if (len > STUBMAXRECORDSIZE -2) len = STUBMAXRECORDSIZE - 2;
	    currentLine = &(s->buffer)[0];
	    strncpy(currentLine, s->stringSourceTail, len);
	    currentLine[len] = '\0';
	    s->stringSourceTail += len;
	}

    } else {
	currentLine = fgets(&(s->buffer)[0], STUBMAXRECORDSIZE, s->file);
    }
    if (currentLine == 0) {
	strcpy(s->buffer, "*** End of File ***");
    } else {
	s->lineNo++;
	len = strlen(currentLine) - 1;
	if (s->buffer[len] == '\n') {
	    s->buffer[len] = '\0';
	} else if (len >= STUBMAXRECORDSIZE - 2) {
	    error_report("Input line too long:");
	    error_report(currentLine);
	}
    }
    /* if (s->echo) slo_echoLine(currentLine);		only needed in LCL */
    /* if (s->echoLh) LCLLhEchoLine(currentLine);	only needed in LCL */
    return currentLine;
}


extern bool
source_open (source *s) {
    if (s->fromString) {
	s->stringSourceTail = s->stringSource;
	return TRUE;
    }
    s->file = fopen(s->name, "r");
    return s->file != 0 || s->fromString;
}

/*
** requires
**  path != NULL \and
**  s != NULL \and
**  *s.name == filename(*s.name) || filetype(*s.name)
**      // *s.name consists of a file name and type only ("<filename>.<type>)
**	// No path name is included 
**
** ensures
**  if filefound(*path, *s) then
**	result = true \and *s.name = filespec_where_file_found(*path, *s)
**  else
**	result = false
*/

extern bool
source_getPath (char *path, source *s, LSLToken t) {
    char	*returnPath;
    osd_status	 status;	    /* return status of osd_getPath.*/
    bool	 rVal;		    /* return value of this procedure. */
    
    /* Check if requires met. */
    if (path == NULL || s == NULL || s->name == NULL) {
	CompilerFailure("source_getPath: invalid parameter");
    }
    
    status = osd_getPath(path, s->name, &returnPath);
    if (status == osd_FILEFOUND) {	    /* Should be majority of cases. */
        rVal = TRUE;						
	LSLFreeMem(s->name);
	s->name = returnPath;
    } else if (status == osd_FILENOTFOUND) {
	rVal = FALSE;
    } else if (status == osd_PATHTOOLONG) {
	rVal = FALSE;
	/* Directory and filename are too long.  Report error. */
	if (token_code(t) == NOTTOKEN) {
	    UserError("filename plus directory from search path too long");
	} else {
	    UserErrorLoc(t,"filename plus directory from search path too long");
	}
    } else {
	rVal = FALSE;
	CompilerFailure("source_getPath: invalid return status");
    }		
    return rVal;
}

static void
CompilerFailure(char *msg) {
    error_programFailure(msg);
    exit(FAILURE);					/* one is plenty    */
}

static void
UserError(char *msg) {
    error_report(msg);
}

static void
UserErrorLoc(LSLToken t, char *msg) {
    error_reportBadToken(&t, msg);
}
