/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**  	FILENAME:   tokentable.c
**
**	PURPOSE:    Stores predefined tokens
**
**  AUTHORS:
**
**	Gary Feldman, Technical Languages and Environments, DECspec project
**	Steve Garland, MIT
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  CREATION DATE:
**
**	8 August 1990
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	JPW	90.08.08	Original
**  X0.1-2	SG	90.10.04	Add components to token
**  X0.1-3	GAF	90.12.21	Change BADTOKEN to NOTTOKEN
**  X0.1-4	SJG	92.02.19	Reorder #include "error.h"
**  X0.1-5	SJG	93.11.26	Add priority for tokens
**  [@modification history entry@]...
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

/*
**
**  MACRO DEFINITIONS
**
*/

# define INITTOKENTABLE   1000
/* ### Why only reallocate 10% more?  Seems small. */
# define DELTATOKENTABLE  1.1

# define MoreMem	    LSLMoreMem

# define String2Symbol	    LSLString2Symbol

# define CodePart	    token_code
# define SetTextSym	    token_setTextSym
# define SetRawTextSym	    token_setRawTextSym
# define SetCode	    token_setCode
# define SetDefined	    token_setDefined
# define SetHasSyn	    token_setHasSyn
# define SetPriority	    token_setPriority

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


extern LSLToken	    LSLInsertToken();
extern void	    LSLUpdateToken();
extern void	    LSLSetTokenHasSyn();
extern void	    LSLSetTokenPriority();
extern LSLToken	    LSLGetToken();
extern LSLToken	    LSLReserveToken();

static void	    AllocTokenTableSpace();
static void	    AllocTokenTable();

extern void	    LSLTokenTableInit();
extern void	    LSLTokenTableReset();
extern void	    LSLTokenTableCleanup();

/*
**
**  STATIC VARIABLES
**
*/

static long unsigned MaxToken;		/* size of TokenTable[]              */
static LSLToken	    *TokenTable;	/* ptr to current TokenTable[]       */



/*
**++
**  FUNCTION NAME:
**
**      LSLInsertToken()
**
**  FORMAL PARAMETERS:
**
**      code - a token code
**	sym  - token-handle for token, string-handle for the token text
**	rTxt - string-handle for the token's the raw text
**	def  - flag indicated whether or not the token is defined.
**
**  RETURN VALUE:
**
**      The token inserted.
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    If the token is already in the token table, it is returned.  Otherwise,
**    the token is inserted into the table and then returned.
**
**    A new TokenTable is allocated when:
**	. The TokenTable[] is empty (initial case)
**      . The location where to insert the token is not in TokenTable[]
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    TokenTable      - (input/output) TokenTable array
**
**  EXCEPTIONS:
**
**--
*/
LSLToken
LSLInsertToken(LSLTokenCode cod, LSLSymbol sym, LSLSymbol rTxt, bool def) {

    while (sym >= MaxToken) {
    	/* No more space available.  Allocate more. */
	AllocTokenTable();
    }


    /* ### Will new allocated memory be set to zero? */
    if (CodePart(TokenTable[sym]) == NOTTOKEN) {
	/* Entry is empty. Fill it in. */
	SetTextSym(TokenTable[sym], sym);
	SetRawTextSym(TokenTable[sym], rTxt);
	SetCode(TokenTable[sym], cod);
	SetDefined(TokenTable[sym], def);
	SetPriority(TokenTable[sym], 0);
    }

    return TokenTable[sym];
}


/*
**++
**  FUNCTION NAME:
**
**      LSLUpdateToken()
**
**  FORMAL PARAMETERS:
**
**      code - a token code
**	sym  - token-handle for token
**	def  - flag indicated whether or not the token is defined.
**	
**  RETURN VALUE:
**
**	None
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine sets the token code and the defined flag for a token
**    previously entered in the token table.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    TokenTable      - (input/output) TokenTable array
**
**  EXCEPTIONS:
**	The token to be updated is not in the table.
**--
*/
void
LSLUpdateToken(LSLTokenCode cod, LSLSymbol sym, bool def) {

    if (CodePart(TokenTable[sym]) != NOTTOKEN) {
	/* Entry is not empty. Set token code and defined flag. */
	SetCode(TokenTable[sym], cod);
	SetDefined(TokenTable[sym], def);
    }
    else {
	CompilerFailure("LSLUpdateToken()");
    }
}

/*
**++
**  FUNCTION NAME:
**
**      LSLSetTokenHasSyn()
**
**  FORMAL PARAMETERS:
**
**	sym  - token-handle for token
**	syn  - flag indicated whether or not the token has a synonym.
**	
**  RETURN VALUE:
**
**	None
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine sets the "has synonym" flag for a token
**    previously entered in the token table.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    TokenTable      - (input/output) TokenTable array
**
**  EXCEPTIONS:
**	The token to be updated is not in the table.
**--
*/
void
LSLSetTokenHasSyn(LSLSymbol sym, bool syn) {

    if (CodePart(TokenTable[sym]) != NOTTOKEN) {
	/* Entry is not empty. Set hasSyn flag. */
	SetHasSyn(TokenTable[sym], syn);
    }
    else {
	CompilerFailure("LSLSetTokenHasSyn()");
    }
}

void
LSLSetTokenPriority(LSLSymbol sym, int p) {

    if (CodePart(TokenTable[sym]) != NOTTOKEN) {
	/* Entry is not empty. Set priority. */
	SetPriority(TokenTable[sym], p);
    }
    else {
	CompilerFailure("LSLSetTokenPriority()");
    }
}


/*
**++
**  FUNCTION NAME:
**
**      LSLGetToken()
**
**  FORMAL PARAMETERS:
**
**      sym - the symbol handle for a token.
**
**  RETURN VALUE:
**
**      The token corresponding to the symbol handle specified.
**
**  INVARIANTS:
**   
**      The same context that was active when the string-handle 
**      was derived by a previous call to String2Symbol(),
**	must be established.
**   
**  DESCRIPTION:
**
**	Return the token associated with the token handle.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    TokenTable  - (output) TokenTable array
**
**  EXCEPTIONS:
**   
**      Token does not exist in table.
**--
*/
LSLToken
LSLGetToken(LSLSymbol sym) {

    if ((sym < MaxToken) || (TokenTable[sym].code != NOTTOKEN)) {
	/* Token exists in table. */
	return TokenTable[sym];
    }
    else {
	CompilerFailure("LSLGetToken()");
    }
}


/*
**++
**  FUNCTION NAME:
**
**      LSLReserveToken()
**
**  FORMAL PARAMETERS:
**
**      cod - token code
**	txt - token text
**
**  RETURN VALUE:
**
**      The token corresponding token inserted into the table as a reserved
**	word. 
**
**  INVARIANTS:
**   
**      The same context that was active when the string-handle 
**      was derived by a previous call to String2Symbol(),
**	must be established.
**   
**  DESCRIPTION:
**
**	Insert the token into the token table as a reserved token and return it.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**
**  EXCEPTIONS:
**   
**--
*/
LSLToken
LSLReserveToken(LSLTokenCode cod, char *txt) {
    LSLSymbol sym;

    sym = String2Symbol(txt);

    /* Reserved tokens never have raw text like synonyms.  Call with raw    */
    /* text = 0 */
    return LSLInsertToken(cod, sym, 0, TRUE);
}

/*
**++
**  FUNCTION NAME:
**
**      AllocTokenTableSpace()
**
**  FORMAL PARAMETERS:
**
**      newSize - the new size of the TokenTable[]
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
**	Extend the current TokenTable[] by "newSize" number of elements. The
**	entire portion of TableTable[] before "extension" has been preserved.
**	The new portion of the extended TokenTable[] has been initialized.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    MaxToken   - (input/output) size of TokenTable[]
**    TokenTable - (input/output) Token array is extended
**
**  EXCEPTIONS:
**   
**      Cannot decrease size of array
**--
*/
static void
AllocTokenTableSpace(unsigned newSize) {

    if (newSize <= MaxToken) {
	CompilerFailure("AllocTokenTableSpace()");
    }

    /* 
     * Extend data  
     */
    TokenTable =
	    (LSLToken*)MoreMem((void*)TokenTable,newSize*sizeof(*TokenTable));

    /*
     *	Update context values for extended TokenTable[]
     *     - TokenTable[] has been made larger
     */
    MaxToken  = newSize;

}

/*
**++
**  FUNCTION NAME:
**
**      AllocTokenTable()
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
**    TokenTable     - (input/output) new token table allocated from it.
**   
**  EXCEPTIONS:
**   
**      none
**--
*/

static void
AllocTokenTable(void) {
    long unsigned	oldSize, newSize;
    int			i;
    
    oldSize = MaxToken;

    if (oldSize == 0) {
	/* First time TokenTable allocated.  Set initial size. */
	newSize = INITTOKENTABLE;
    }
    else {
	/* Token table already allocated.  Calulate extension size. */
	newSize = (unsigned)(DELTATOKENTABLE*oldSize);
    }

    AllocTokenTableSpace(newSize);

    /* Zero out new allocated space.  Need to detect when cells are empty   */
    /* and do this by checking that TokenTable[x].code == BADTOKEN.	    */

    /* ###  Should the "for" loop be replaced with the following?	    */
    /* #if VMS								    */
    /* # include <string.h>;						    */
    /* #else								    */
    /* # include <memory.h>;						    */
    /*									    */
    /* memset(TokenTable[oldSize], 0,					    */
    /*	      (newSize - oldSize) * sizeof(*TokenTable));		    */

    for (i = oldSize; i < newSize ; i++) {
	TokenTable[i] = token_null();
    }

}


/*
**++
**  FUNCTION NAME:
**
**      LSLTokenTableInit()
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
**      MaxToken   - (output) initial context is EMPTY
**
**  EXCEPTIONS:
**   
**      none
**--
*/

void
LSLTokenTableInit(void) {
    MaxToken   = 0;
}

void
LSLTokenTableReset(void) {
}

void
LSLTokenTableCleanup(void) {
}

