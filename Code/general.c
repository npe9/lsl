/*
**
**	Copyright (c) MIT, 1990
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   general.c
**
**	PURPOSE:    Low level memory, string management
**
**  MODIFICATION HISTORY:
**
**	{0} Garland at MIT     -- 90.10.04 -- Extract from drivers
**	{1} Wild    at Digital -- 90.11.20 -- malloc -> calloc
**	{2} Garland at MIT     -- 93.12.05 -- Added #includes for clean compile
**     	{n} Who     at Where   -- yy.mm.dd -- did what?
**	
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include <string.h>
# include "general.h"
# include "osdfiles.h"
# include STDLIB
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

extern void        *LSLMoreMem();
extern void	    LSLFreeMem();
extern char	   *string_paste();


/*
**
**  STATIC VARIABLES (none)
**
*/


/*
**
** FUNCTIONS
**
**
*/



/*
**++
**  FUNCTION NAME:
**
**      LSLMoreMem
**
**  FORMAL PARAMETERS:
**
**      ptr     - location of memory to reuse (if non-zero)
**	newsize - number of bytes to allocate
**
**  RETURN VALUE:
**
**
**  INVARIANTS:
**   
**      [@tbs@]
**   
**  DESCRIPTION:
**
**      stub memory allocator
**
**  SIDE EFFECTS:
**
**      [@description or none@]
**
**  EXCEPTIONS:
**   
**      [@tbs@]
**--
*/

extern void *
LSLMoreMem(void *ptr, int newsize) {
    if (ptr == (void *) 0) {				/* vax rtl kludge */
	ptr = (void *) calloc(newsize, 1);		/* newsize bytes */
    } else {
	ptr = (void *) realloc(ptr, newsize);
    }
    if (ptr ==  (void *) 0) error_systemFailure("out of memory");

    return ptr;
}

/*
**++
**  FUNCTION NAME:
**
**      LSLFreeMem
**
**  FORMAL PARAMETERS:
**
**      ptr     - location of memory to deallocate
**
**  RETURN VALUE:
**
**      [@description or none@]
**
**  INVARIANTS:
**   
**      [@tbs@]
**   
**  DESCRIPTION:
**
**      stub memory deallocator
**
**  SIDE EFFECTS:
**
**      [@description or none@]
**
**  EXCEPTIONS:
**   
**      [@tbs@]
**--
*/

extern void 
LSLFreeMem(void *ptr) {
    free(ptr);
}

/*
**++
**  FUNCTION NAME:
**
**      string_paste
**
**  FORMAL PARAMETERS:
**
**      *s1, *s2 - strings to concatenate
**
**  RETURN VALUE:
**
**	Newly-allocated concatentation of *s1, *s2
**
**  INVARIANTS:
**   
**      [@tbs@]
**   
**  DESCRIPTION:
**
**      String concatenation
**
**  SIDE EFFECTS:
**
**      [@description or none@]
**
**  EXCEPTIONS:
**   
**      [@tbs@]
**--
*/

extern char
*string_paste (char *s1, char *s2) {
    char *s = (char *)LSLMoreMem((void *)0, strlen(s1) + strlen(s2) + 1);
    strcpy(s, s1);
    strcat(s, s2);
    return s;
}


