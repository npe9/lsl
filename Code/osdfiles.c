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
**      FILENAME:   osdfiles.c
**
**	PURPOSE:    Provide a system-independent interface to system-dependent
**		    file operations.
**
**  AUTHORS:
**
**      Gary Feldman, Technical Languages and Environments, DECspec project
**      Joe Wild, Technical Languages and Environments, DECspec project
**
**  CREATION DATE:  17 Sep 1990
**
**  DESIGN ISSUES:
**
**      We use #ifdef macros to implement each functions.
**
**	Since we want this module to be shareable, we haven't used LSL memory
**	management routines.  Instead, we use malloc.  This is less than ideal.
**	A better solution would be to have a project-specific require file that
**	defines macros for the memory management routines.
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	GAF	90.09.17	Created
**  X0.1-2	GAF	91.01.15	Replaced include of malloc with include
**					    of stdlib.
**  X0.1-3	JPW	91.02.06	Add osd_fileExists().
**  X0.1-4	GAF	91.03.12	Fix bug in Unix version of LSLRootName,
**					where it failed to add the null
**					character at the end of the root name
**					string.
**  X0.1-5	JPW	91.07.17	Add osd_getPath that works on VMS also.
**  X0.1-6	JPW	91.07.25	Change osd_getPath so that it only
**					depends on any system includes.  Make
**					osd_fileExists static and rename
**					unixFileExists.
**  X0.1-7	SJG	92.09.14	Change osd_getPath to handle fully
**					qualified file names
**  [@modification history entry@]...
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include "osdfiles.h"
#  if defined(VMS)
#  include <stat.h>
#  include rms
# else
#  include STDLIB
#  include <stdio.h>
#  include <string.h>
#  include <sys/types.h>
#  include <sys/stat.h>
# endif

/* included to get bool.  if general.h ever needs other includes may be	    */
/* best to redefine bool here.						    */
# include "general.h"


/*
**
**  MACRO DEFINITIONS
**
*/

/* Directory separator character for search list. */
# define    SEPCHAR	':'

/* Connection string inserted between directory and filename to make a  */
/* full path name.							*/
# if defined(VMS)
#  define    CONNECTSTR	":"
# else
#  define    CONNECTSTR	"/"
# endif

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
extern char	   *LSLRootName (char *filespec);
extern osd_status   osd_getPath (char *path, char *file, char **returnPath);

static bool	 unixFileExists (char *filespec);
static bool	 nextdir (char **current_dir, char **dir, unsigned int *len);

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


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function computes the rootname of a file specification, in the
**	sense of the :r modifier of csh.  
**
**  FORMAL PARAMETERS:
**
**      char *filespec	-   The file specification from which we will extract a
**	rootname.
**
**  RETURN VALUE:
**
**      char *	- The rootname of the file specification.  The space for this
**	rootname is allocated with malloc and should be freed by the client with
**	free.
**
**  SIDE EFFECTS:
**
**      Space is allocated.
**
**  DESIGN:
**
**      On VMS, we use sys$parse to parse the file spec, and return the file
**	name.  On Ultrix, we manually parse the file spec, removing everything
**	upto and including the last / (removing nothing if there are no
**	slashes), and removing the last period (if any) to the end of the spec.
**
**	VMS parsing is incomplete, because of the possibility of getting a
**	quoted DECnet file name.  For example, NODE::"/remote/dir/file.ext" will
**	result in "/remote/dir/file.ext" (including the quotes) as the file
**	name; this is not usable for building an output file.  It's not at all
**	clear whether a solution exists for this.
**
**	Memory management is hardwired to malloc/free.  This ought to be made
**	more flexible, by somehow using a user-defined memory management system.
**
**--
*/

extern char
*LSLRootName (char *filespec) {
#if defined(VMS)
    int i;
    struct FAB fab = cc$rms_fab;
    struct NAM nam = cc$rms_nam;
    char *result, namesa[NAM$C_MAXRSS];

    fab.fab$l_fna = filespec;
    fab.fab$b_fns = strlen(filespec);
    fab.fab$l_nam = &nam;
    nam.nam$b_nop = NAM$M_SYNCHK;
    nam.nam$l_esa = &namesa[0];
    nam.nam$b_ess = NAM$C_MAXRSS;
    sys$parse (
    	&fab, 
    	0, 
    	0);
    result = malloc (nam.nam$b_name + 1);
    strncpy (result, nam.nam$l_name, nam.nam$b_name);
    result[nam.nam$b_name] = '\0';
    /* force to lower case, since VMS returns uppercase names. */
    for (i = 0; result[i] != '\0'; i++) result[i] = tolower(result[i]);
    return result;
#else
    char *result, *startName, *tail;
    int nameLength;

    tail = strrchr(filespec, '/');
    startName = (tail == NULL ? filespec : &tail[1]);
    tail = strrchr(startName, '.');
    nameLength = (tail == NULL ? strlen(startName) : tail - startName);
    result = malloc(nameLength + 1);
    strncpy (result, startName, nameLength);
    result[nameLength] = 0;
    return result;
#endif
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function attempts to locate a file in a search list.  On VMS, it
**	just concatinates the path and file names, and then lets RMS do the
**	searching.  On Ultrix, it searches for the file on the path.
**
**  FORMAL PARAMETERS:
**
**      path:	    search path where to look for the file.
**	file:	    name of file to search for.
**	returnPath: if a file is found, this is where the concatenated
**		    directory and file name are returned.
**
**  RETURN VALUE:
**
**      osd_FILEFOUND:	    the file was found on the search list.
**	osd_FILENOTFOUND    the file was not found.
**	osd_PATHTOOLONG	    the concatenated directory and file name are too
**			    long.
**
**  SIDE EFFECTS:
**
**      None
**
**  PRECONDITIONS:
**   
**  	Requires that parameters, path and file, are valid C strings.
**   
**
**--
*/ 
extern osd_status
osd_getPath (char *path, char *file, char **returnPath) {
    char	    *fullPath;
    char	    *dirPtr;
    unsigned int    dirLen;
    char	    aPath[MAXPATHLEN];
    osd_status	    rVal;		/* return Value */
    
    rVal = osd_FILENOTFOUND;		/* assume file not found. */

#if defined(VMS)
    /* concatenate the path for VMS and let file system handle. */
    if ((strlen(path) + strlen(file) + 2) <= MAXPATHLEN) {
	strcpy(&aPath[0], path);
	strcat(&aPath[0], CONNECTSTR);
	strcat(&aPath[0], file);
	*returnPath = LSLMoreMem((void *) 0, strlen(&aPath[0]) + 1);
	strcpy(*returnPath, &aPath[0]);
	rVal = osd_FILEFOUND;
    } else {
	rVal = osd_PATHTOOLONG;
    }
#else
    /* get the path for UNIX. */   
    fullPath = getenv(path);

    if (fullPath == NULL || *file == '/') {
	/* No path specified or fully qualified file name. Look for file    */
	/* in the current directory.					    */

	strcpy(&aPath[0], file);

	if (unixFileExists(&aPath[0])) {
	    rVal = osd_FILEFOUND;
	    *returnPath = LSLMoreMem((void *) 0, strlen(&aPath[0]) + 1);
	    strcpy(*returnPath, &aPath[0]);
	}
    } else {
	/* Path specified. Loop through directories in path looking for the */
	/* first occurrence of the file.				    */
	while (nextdir(&fullPath, &dirPtr, &dirLen) &&
	       rVal == osd_FILENOTFOUND) {
	    if ((dirLen + strlen(file) + 2) <= MAXPATHLEN) {
		/* Cat directory and filename, and see if file exists.  */
		strncpy(&aPath[0], dirPtr, dirLen);
		strcpy(&aPath[0] + dirLen, "");  /* Null terminate aPath. */
		strcat(&aPath[0], CONNECTSTR);
		strcat(&aPath[0], file);

		if (unixFileExists(&aPath[0])) {
		    rVal = osd_FILEFOUND;
		    *returnPath = (char *)LSLMoreMem((void *)0,
						     strlen(&aPath[0]) + 1);
		    strcpy(*returnPath, &aPath[0]);
		}
	    } else {
		rVal = osd_PATHTOOLONG;
	    }
	} /* while */
    }
#endif
    return rVal;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns whether or not a file exists.
**
**  FORMAL PARAMETERS:
**
**      char *filespec - The specification of the file to check for.
**
**  RETURN VALUE:
**
**      bool - TRUE if the file exists. FALSE otherwise.
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      Uses stat() to find out if the file exists or not.
**
**
**--
*/
static bool
unixFileExists(char *filespec) {
    struct stat buf;

    return (!stat(filespec, &buf));
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Find the next directory from a directory path.
**
**  FORMAL PARAMETERS:
**
**      char ** current_dir :
**	    Points to the current position in the path string.  The first time
**	    you call this routine, this should point to the first character of
**	    the path.  On return, this will be updated to point to the
**	    terminating \0 or : of the first directory found.  You can then pass
**	    it unchanged for subsequent calls; this routine will correctly skip
**	    over the :.
**
**	char ** dir :
**	    On exit, this will point to the first character of the directory
**	    that was found.  This will be a pointer directly into the client's
**	    path string.
**
**	unsigned int * len :
**	    On exit, this will contain the length of the directory that was
**	    found, not counting any terminating \0 or :.  If no directory was
**	    found, this will be 0.
**
**  RETURN VALUE:
**	TRUE if we found another directory.
**	FALSE otherwise.
**
**  DESIGN:
**
**      We return a pointer and length, rather than a string, because of a)
**	historical reasons; and b) this avoids having to allocate storage.
**
**
**
**--
*/
static bool
nextdir (char **current_dir, char **dir, unsigned int *len) {
    char * tchar;

    if (**current_dir == '\0') {
    	*len = 0;
	return FALSE;
    }

    *dir = (**current_dir == SEPCHAR ? *current_dir + 1 : *current_dir);

    /* Find next ':' or end of string */
    for (tchar = *dir ; *tchar != '\0' && *tchar != SEPCHAR ; tchar++);

    *current_dir = tchar;
    *len = tchar - *dir;
    return TRUE;
}
