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
**		    file operations and other system-dependent features.
**
**  AUTHORS:
**
**      Gary Feldman, Technical Languages and Environments, DECspec project
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  CREATION DATE:  17 Sep 1990
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	GAF	90.09.17	Original
**  X0.1-2	JPW	90.10.08	Added PASTE macro.
**  X0.1-3	JPW	90.10.08	Add STR macro.
**  X0.1-4	GAF	91.02.06	Add osd_fileExists() and MAXPATHLEN.
**  X0.1-5	GAF	91.02.08	Add STDLIB macro.
**  X0.1-6	GAF	91.02.22	Add TMPDIR macro.
**  X0.1-7	JPW	91.04.10	Change STDLIB to "spec_stdlib.h".
**  X0.1-8	JPW	91.04.11	add nostdlib macro to
**					STDLIB to "spec_stdlib.h" only when set.
**  X0.1-9	JPW	91.04.17	Change nostdlib to NOSTDLIB.
**  X0.1-10	JPW	91.07.17	Add osd_getPath that also works on VMS.
**  X0.1-11	JPW	91.07.25	Change osd_getPath so that it only
**					depends on any system includes.  Make
**					osd_fileExists static and rename
**					unixFileExists.  Move SUCCESS and
**					FAILURE here because OS depend.
**  [@modification history entry@]...
**--
*/

/*
**
**  MACRO DEFINITIONS
**
*/

/* Macro to substitute for ## because VAX C currently doesn't allow it. */
# ifdef __STDC__
    /* Use an intermediate paste macro so PASTE macro arguments will get    */
    /* evaluated.  Therefore, PASTE(PASTE(a,b),c) will work. */
# define IPASTE(L,R)	    L ## R
# define PASTE(L,R)         IPASTE(L,R)
# else
# define PASTE(L,R )	    L/**/R
# endif

/* Macro to substitute for # because VAX C currently doesn't allow it. */
# ifdef __STDC__
#  define ISTR(x)	# x
#  define STR(x)         ISTR(x)
# else
#  define ISTR(x)	"x"
#  define STR(x)		ISTR(x)
# endif

/*
** MAXPATHLEN defines the longest permissable path length.
** Is defined in <sys/param.h> in MIPS/Ultrix, but it not defined on the VAX.
** Define here so available on both systems.
*/
# define MAXPATHLEN      1024


/* STDLIB is defined to point to the appropriate standard library on the    */
/* system.  This must be <stdlib.h> under VMS (which doesn't scan the	    */
/* system libraries for quoted file names), "stdlib.h" under DEC/C on	    */
/* Ultrix (which doesn't understand angle brackets in this type of macro    */
/* usage) and "libc.h" under BSD 4.3 at MIT and CMU, which don't have	    */
/* stdlib.								    */
/*									    */
/* Because of all the hassle finding stdlib.h on various systems.  We now   */
/* include our own file, spec_stdlib.h, if none is available.		    */
# if defined(NOSTDLIB) 
#  define STDLIB "spec_stdlib.h"
# elif defined(__ultrix)
#  define STDLIB "stdlib.h"
# else
#  define STDLIB <stdlib.h>
# endif

/* VMS doesn't have the Unix function "unlink."  Mach and BSD4.3, which	    */
/* they're running at CMU and MIT, doesn't have "remove".  Therefore we	    */
/* define this macros to make sure that right function is available.	    */
/*									    */
/* Ultrix, by the way, has both.  "remove" is an ANSI C library function,   */
/* while "unlink" isn't.  Thus the ultimate fix is to make Mach and BSD4.3  */
/* Ansi/POSIX compliant.						    */
# if defined(vms)
#  define unlink remove
# endif

# if ! defined(vms)
#  define remove unlink
# endif

/* We need system-specific temporary directories			    */
# if defined(vms)
#  define TMPDIR "SYS$SCRATCH:"
# else
#  define TMPDIR "/usr/tmp/"
# endif

# if defined(VMS)
#  define SUCCESS 1
#  define FAILURE 0
# else
#  define SUCCESS 0
#  define FAILURE 1
# endif


/*
**
**  TYPEDEFS
**
*/

typedef enum {
    osd_FILEFOUND,
    osd_FILENOTFOUND,
    osd_PATHTOOLONG}	osd_status;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern char	   *LSLRootName (char *filespec);
extern osd_status   osd_getPath (char *path, char *file, char **returnPath);
