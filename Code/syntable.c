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
**      FILENAME:   syntable.c
**
**	PURPOSE:    Larch shared language synonym table
**
**	This table stores synonyms for the Larch Shared Language.  It is
**	essentially a array of token-handles indexed by string-handles.
**	Therefore, synonyms (strings) can be converted to the actual token.
**
**  MODIFICATION HISTORY:
**
**	{0} Wild    at Digital -- 90.08.10 -- Original
**	{1} Garland at MIT     -- 92.02.19 -- Reorder #include "error.h"
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include	"general.h"
# include	"string2sym.h"
# include	"token.h"
# include	"error.h"
# include	"tokentable.h"
# include	"syntable.h" 

/*
**
**  MACRO DEFINITIONS
**
*/

# define INITSYNTABLE   1000
/* ### Why only reallocate 10% more?  Seems small. */
# define DELTASYNTABLE  1.1

# define MoreMem	    LSLMoreMem

# define SetHasSyn	    LSLSetTokenHasSyn

# define CompilerFailure    error_programFailure


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

extern void	    LSLAddSyn();
extern LSLToken	    LSLGetTokenForSyn();
extern bool	    LSLIsSyn();

static void	    AllocSynTableSpace();
static void	    AllocSynTable();

extern void	    LSLSynTableInit();
extern void	    LSLSynTableReset();
extern void	    LSLSynTableCleanup();

/*
**
**  STATIC VARIABLES
**
*/

static long unsigned MaxSyn;		/* size of SynTable[]              */
static LSLSymbol  *SynTable;		/* ptr to current SynTable[]       */


/*
**++
**  FUNCTION NAME:
**
**      LSLAddSyn()
**
**  FORMAL PARAMETERS:
**
**      otok  - token-handle for token associated with oldToken
**	ntok  - string-handle for the string to be a synonym with oldToken.
**
**  RETURN VALUE:
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine inserts a synonym into the synonym table.  The synonym table
**    is used to define synonyms in the form:
**
**	    synonym oldToken newToken
**
**    The table associates the string for newToken with the token for oldToken.
**    This table is used to find the the actual token (oldToken) from a synonym
**    string (newToken).
**
**    A new SynTable is allocated when:
**	. The SynTable[] is empty (initial case)
**      . The location where to insert the synonym is not in SynTable[]
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    SynTable      - (input/output) SynTable array
**
**  EXCEPTIONS:
**    A synonym already exists at the location where the it is to be added.
**
**--
*/
void
LSLAddSyn(LSLSymbol ntok, LSLSymbol otok) {

    while (otok >= MaxSyn) {
    	/* No more space available.  Allocate more. */
	AllocSynTable();
    }

    if (SynTable[ntok] == 0) {
	/* Entry is empty. Fill it in. */
	SynTable[ntok] = otok;
	SetHasSyn(otok,TRUE);	    /* Mark oldToken as having a synonym. */
    }
    else {
	CompilerFailure("LSLAddSyn()");
    }
}


/*
**++
**  FUNCTION NAME:
**
**      LSLGetTokenForSyn()
**
**  FORMAL PARAMETERS:
**
**	ntok  - string-handle for the newToken synonym.
**
**  RETURN VALUE:
**
**      The token that is the synonym of the new token string specified.
**
**  INVARIANTS:
**   
**      The same context that was active when the string-handle 
**      was derived by a previous call to String2Symbol(),
**	must be established.
**   
**  DESCRIPTION:
**
**	Return the oldToken associated the newToken string synonym.
**
**  IMPLICIT INPUTS/OUTPUT:
**
*     SynTable    - (input) SynTable array
**
**  EXCEPTIONS:
**   
**      Synonym does not exist in table.
**--
*/
LSLToken
LSLGetTokenForSyn(LSLSymbol ntok) {

    if ((ntok < MaxSyn) || (SynTable[ntok] != 0)) {
	/* Synonym exists in table.  Return it. */
	return LSLGetToken(SynTable[ntok]);
    }
    else {
	CompilerFailure("LSLGetSyn()");
    }
}

/*
**++
**  FUNCTION NAME:
**
**      LSLIsSyn()
**
**  FORMAL PARAMETERS:
**
**	str  - string-handle.
**
**  RETURN VALUE:
**
**      Returns TRUE if string specified is a synonym for a token, FALSE
**	otherwise. 
**
**  INVARIANTS:
**   
**      The same context that was active when the string-handle 
**      was derived by a previous call to String2Symbol(),
**	must be established.
**   
**
**  IMPLICIT INPUTS/OUTPUT:
**
**     SynTable    - (input) SynTable array
**
**--
*/
bool
LSLIsSyn(LSLSymbol str) {

    if (str < MaxSyn) {
	/* Check for synonym entry in table. */
	return (SynTable[str] != 0);
    }
    else {
	/* No token for synonym.  Return FALSE. */
	return FALSE;
    }
}

/*
**++
**  FUNCTION NAME:
**
**      AllocSynTableSpace()
**
**  FORMAL PARAMETERS:
**
**      newSize - the new size of the TokenEntry[]
**
**  RETURN VALUE:
**
**      None
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**	Extend the current SynTable[] by "newSize" number of elements. The
**	entire portion of SynTable[] before "extension" has been preserved.
**	The new portion of the extended SynTable[] has been initialized.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    MaxSyn   - (input/output) size of SynTable[]
**    SynTable - (input/output) Synonym array is extended
**
**  EXCEPTIONS:
**   
**      Cannot decrease size of array
**--
*/
static void
AllocSynTableSpace(unsigned newSize) {

    if (newSize <= MaxSyn) {
	CompilerFailure("AllocSynTableSpace()");
    }

    /* 
     * Extend data  
     */
    SynTable =
	    (LSLSymbol*)MoreMem((void*)SynTable,newSize*sizeof(*SynTable));

    /*
     *	Update context values for extended SynEntry[]
     *     - SynTable[] has been made larger
     */
    MaxSyn  = newSize;

}

/*
**++
**  FUNCTION NAME:
**
**      AllocSynTable()
**
**  FORMAL PARAMETERS:
**
**      none
**
**  RETURN VALUE:
**
**      none
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine is used to allocate a TokenTable
**    from the current context.
**    
**  IMPLICIT INPUTS/OUTPUT:
**
**    SynTable     - (input/output) new synonym table allocated from it.
**   
**  EXCEPTIONS:
**   
**      none
**--
*/

static void
AllocSynTable(void) {
    long unsigned	newSize, oldSize;
    int			i;
    
    oldSize = MaxSyn;

    if (oldSize == 0) {
	/* First time SynTable allocated.  Set initial size. */
	newSize = INITSYNTABLE;
    }
    else {
	/* Synonym table already allocated.  Calulate extension size. */
	newSize = (unsigned)(DELTASYNTABLE*oldSize);
    }

    AllocSynTableSpace(newSize);

    /* Zero out new allocated space.  Need to detect when cells are empty   */
    /* and do this by checking that SynTable[x] == 0.			    */

    /* ###  Should the "for" loop be replaced with the following?	    */
    /* #if VMS								    */
    /* # include <string.h>;						    */
    /* #else								    */
    /* # include <memory.h>;						    */
    /*									    */
    /* memset(SynTable[oldSize], 0,					    */
    /*	      (newSize - oldSize) * sizeof(*SynTable));		    */

    for (i = oldSize; i < newSize ; i++) {
	SynTable[i] = 0;
    }
}



/*
**++
**  FUNCTION NAME:
**
**      LSLSynTableInit()
**
**  FORMAL PARAMETERS:
**
**      None
**
**  RETURN VALUE:
**
**      None
**
**  INVARIANTS:
**   
**      A new context is established
**   
**  DESCRIPTION:
**
**    Create a new context and establish it as the current one.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**      MaxSyn   - (output) initial context is EMPTY
**
**  EXCEPTIONS:
**   
**      none
**--
*/

void
LSLSynTableInit(void) {

    MaxSyn   = 0;
}

void
LSLSynTableReset(void) {
}

void
LSLSynTableCleanup(void) {
}

