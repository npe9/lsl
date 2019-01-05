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
**      FILENAME:   string2sym.c
**
**	PURPOSE:    String manager
**
**	This module implements an abstraction for efficiently managing
**      string comparisons.  It alloctes and manages a string context,
**      which consists of three major data structures:
**       - a StringEntry table 
**       - a character-string table
**       - a hash table
**
**      A StringEntry table is made of of StringEntries. StringEntries
**      are linked in hash-chains to support fast lookup. Every
**      allocated StringEntry corresponds to a unique character-string
**	in the character-string table. StringEntries can be referenced
**      via Symbol values.
**
**	A character-string table is composed of character-strings. A 
**      character-string is a variable length byte array that is always
**	null-terminated ('\0').
**
**	A hash table manages the start of each hash-list of StringEntries.
**	StringEntries are entered into the hash-list by hashing on its
**      character-string representation.
**
**	This module provides routines for retrieving a unique Symbol for a
**	given character-string, and returning the character-string given its
**	corresponding Symbol. An item is allocated in both tables whenever a
**	new character-string is encountered, otherwise the Symbol for the
**	character-string is found and returned.
**
**  AUTHORS:
**
**      Shota Aki
**
**  MODIFICATION HISTORY:
**
**	{0} Aki      at Digital -- 89.08.07 -- Original
**	{1} Aki      at Digital -- 89.11.13 -- Context switchable
**	{2} Aki      at Digital -- 89.11.28 -- Removed use of TABLES interface
**	{3} Aki      at Digital -- 89.11.29 -- Moved to RC
**	{4} Aki	     at Digital -- 90.04.10 -- Support primary-context
**	{5} McKeeman at Digital -- 90.05.08 -- C to Larch SL
**	{6} Garland  at MIT     -- 92.02.19 -- Reorder #include "error.h"
**	{7} Garland  at MIT     -- 93.12.06 -- Avoid warning about no return
**	{n} Who	     at Where   -- yy.mm.dd -- what
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

/*
**
**  MACRO DEFINITIONS
**
*/

# define INITCHARSTRING    10000
# define DELTACHARSTRING   1.1
# define INITSTRINGENTRY   1000
# define DELTASTRINGENTRY  1.1
# define HASHSIZE          256
# define HASHMASK         (HASHSIZE-1)	/* HASHSIZE must be 2^n     */

# define NULLFACTOR	    1

# define ComputeHash(charstring)       \
    {register int h = 0;              \
     register char *p = (charstring); \
     while (*p) h = (h<<1)+(*p++);    \
     HashValue = h&HASHMASK;          \
    }

# define MoreMem	    LSLMoreMem

# define CompilerFailure    error_programFailure


/*
**
**  TYPEDEFS
**
*/

typedef Handle	    CharIndex;	    /* character-string handle		*/

typedef struct {
    LSLSymbol	    HashNext;	    /* next in hash-chain		*/
    CharIndex	    i;		    /* Start of string for this symbol	*/
				    /* in StringTable			*/
} StringEntry;

/*
**
**  FORWARD FUNCTIONS
**
*/

extern LSLSymbol    LSLString2Symbol();
extern char        *LSLSymbol2String();

static void	    AllocCharSpace();
static CharIndex    AllocChar();
static void	    AllocEntrySpace();
static LSLSymbol    AllocEntry();

extern void	    LSLStringInit();
extern void	    LSLStringReset();
extern void	    LSLStringCleanup();

/*
**
**  STATIC VARIABLES
**
*/

static LSLSymbol    *HashArray;		/* current hash array          */

static long unsigned MaxChar;		/* size of CharString[]        */
static CharIndex     FreeChar;		/* top of CharString[]         */
static char	    *CharString;	/* ptr to current CharString[] */

static long unsigned MaxEntry;		/* size of Entry[]              */
static LSLSymbol     FreeEntry;		/* head of freelist for Entry[] */
static StringEntry  *Entry;		/* ptr to current Entry[]       */
 
static long unsigned HashValue;		/* set by ComputeHash()         */


/*
**++
**  FUNCTION NAME:
**
**      LSLString2Symbol()
**
**  FORMAL PARAMETERS:
**
**      name - ptr to a null-terminated character-string
**
**  RETURN VALUE:
**
**      A unique string-handle for the specified character-string. 
**      String-Handles are unique only for a given context.
**
**  INVARIANTS:
**   
**      A context must be established.
**      strcmp(name, LSLSymbol2String(LSLString2Symbol(name))) == 0
**   
**  DESCRIPTION:
**
**    This routine derives the unique string-handle for the given 
**    character-string in the current context.
**
**    A new StringEntry is allocated when:
**	. The Entry[] is empty (initial case)
**      . The Hash chain for the character-string is empty
**      . The character-string is not in the hash chain
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    MaxEntry   - (input) 0 when first time
**    HashArray  - (input) start of hash chains
**    HashValue  - (input/output) index into HashArray[]
**    CharString - (input) character-string array
*     Entry      - (input) StringEntry array
**
**  EXCEPTIONS:
**
**	system: LSLString2Symbol()
**--
*/
LSLSymbol
LSLString2Symbol(char *name){
    LSLSymbol        ss;			    

    ComputeHash(name);			            /* compute HashValue, 
						     * AllocEntry() always 
						     * uses this value  */

    if (MaxEntry == 0) {		    
	ss = AllocEntry(name);			    /* nothing initialized */
    } else {
	ss = HashArray[HashValue];		    /* start of hash chain */

	if (ss == NULLSYMBOL) {	    
	    ss = AllocEntry(name);		    /* hash not initialized */
	} else {
	    /*
	     * Traverse hash-chain. Loop terminates when
             * a match is found or end of chain is encountered.
             */
	    while (strcmp(&CharString[Entry[ss].i],name) != 0) {   
		if(NULLSYMBOL == (ss=Entry[ss].HashNext)) { 
		    ss = AllocEntry(name);		
		    break;
		}
	    }
	}
    }

    return ss;					    /* string-handle for name */
}


/*
**++
**  FUNCTION NAME:
**
**      LSLSymbol2String()
**
**  FORMAL PARAMETERS:
**
**      ss - string-handle for a character-string in a given context
**
**  RETURN VALUE:
**
**      A pointer to a null-terminated character-string that corresponds to
**	the specified string-handle.
**
**  INVARIANTS:
**   
**      The same context that was active when the string-handle 
**      was derived by a previous call to String2Symbol(),
**	must be established.
**   
**  DESCRIPTION:
**
**	Return a pointer to the null-terminated character-string 
**	associated with the specified string-handle. 
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    FreeEntry   - (input) used to range check string-handle
**    CharString  - (input) character-string array
*     Entry       - (input) StringEntry array
**
**  EXCEPTIONS:
**   
**      string-handle out of range
**--
*/
char *
LSLSymbol2String(LSLSymbol ss) {

    if (ss >= FreeEntry) CompilerFailure("LSLSymbol2String()");
    return &CharString[Entry[ss].i];
}

/*
**++
**  FUNCTION NAME:
**
**      AllocCharSpace()
**
**  FORMAL PARAMETERS:
**
**      newSize - the new size of CharString[] (in bytes)
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
**	Extend the current CharString[] by "newSize" number of 
**      elements. The entire portion of CharString[] before "extension" 
**	has been preserved. The new portion of the extended CharString[] 
**	has been initialized and linked into the free list.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    MaxChar    - (input/output) size of CharString[]
**    CharString - (input/output) character-string array
**
**  EXCEPTIONS:
**   
**      Cannot decrease size of array
**--
*/
static void
AllocCharSpace(unsigned newSize) {

    if (newSize <= MaxChar) {
	CompilerFailure("AllocCharSpace()");
    }

    /* 
     * Extend data  
     */
    CharString = (char*)MoreMem((void*)CharString,newSize*sizeof(*CharString));

    /*
     *	Update context values for extended CharString[]
     *     - CharString[] has been made larger
     */
    MaxChar = newSize;

}

/*
**++
**  FUNCTION NAME:
**
**      AllocChar()
**
**  FORMAL PARAMETERS:
**
**      name - ptr to null-terminated string
**
**  RETURN VALUE:
**
**      An index into the current character-string array that
**	locates the start of the specified string.
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine is used to allocate a character-string
**    from the current context. The specified string is
**    copied into the current character-string array and null-terminated.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**	MaxChar    - (input) size of CharString[]
**      CharString - (input/output) "name" copied into it
**      FreeChar   - (input/output) updated to index the next available slot
**                    after the copied string in the CharString[].
**
**  EXCEPTIONS:
**   
**      none
**--
*/
static CharIndex
AllocChar(char *name) {
    int		  namelength;
    CharIndex     retVal;	    
    long unsigned size;
    CharIndex     unused;

    /*
     * Get - length of name
     *     - next unused slot in CharString[]
     *	   - size of CharString[]
     */
    namelength = strlen(name);
    unused     = FreeChar;
    size       = MaxChar;

    /* 
     * EMPTY or FULL? 
     */
    if ((unused+namelength+NULLFACTOR) > size) {
	if (size == 0) 
	    size = INITCHARSTRING;
        else
	    size = (unsigned)(DELTACHARSTRING*size);

	AllocCharSpace(size);
    }

    retVal = unused;				/* save index to name */
    strcpy(&CharString[unused], name);		/* move text */
    unused += namelength;		
    CharString[unused] = '\0';			/* NULL-terminate it */
    unused += 1;

    /*
     * Indicate new available slot in CharString[]. One
     * byte after the copied, null-terminated name
     */
    FreeChar = unused;

    /* 
     * Return CharIndex for allocated chracter-string
     */
    return retVal;
}


/*
**++
**  FUNCTION NAME:
**
**      AllocEntrySpace()
**
**  FORMAL PARAMETERS:
**
**      newSize - the new size of the Entry[] (in StringEntries)
**
**  RETURN VALUE:
**
**      None
**
**  INVARIANTS:
**   
**      A context must be established.
**	FreeEntry` == NOTINCR
**	FreeEntry' == next available slot
**   
**  DESCRIPTION:
**
**	Extend the current Entry[] by "newSize" number of 
**      elements. The entire portion of Entry[] before "extension" 
**	has been preserved. The new portion of the extended Entry[] has been 
**	initialized and linked into the free list.
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    MaxEntry  - (input/output) size of Entry[]
**    Entry     - (input/output) StringEntry array is extended
**    FreeEntry - (output) head of new free list
**
**  EXCEPTIONS:
**   
**      Cannot decrease size of array
**--
*/

static void
AllocEntrySpace(unsigned newSize) {

    if (newSize <= MaxEntry) {
	CompilerFailure("AllocEntrySpace()");
    }

    /* 
     * Extend data  
     */
    Entry = (StringEntry*)MoreMem((void*)Entry,newSize*sizeof(*Entry));

    /*
     *	Update context values for extended Entry[]
     *     - new head of free list is the old max value
     *     - Entry[] has been made larger
     */
    FreeEntry = MaxEntry;
    MaxEntry  = newSize;

}

/*
**++
**  FUNCTION NAME:
**
**      AllocEntry()
**
**  FORMAL PARAMETERS:
**
**      name - ptr to null-terminated character-string
**
**  RETURN VALUE:
**
**      A string-handle for the specified character-string.
**
**  INVARIANTS:
**   
**      A context must be established.
**   
**  DESCRIPTION:
**
**    This routine is used to allocate a StringEntry
**    from the current context. The CharIndex for the
**    specified character-string is saved in the StringEntry. The StringEntry
**    is included in the appropriate hash-chain. StringEntries are always
**    allocated from the head of the free list. 
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    FreeEntry - (input/output) new StringEntry taken from head
**                 of free list
**    HashArray - (input/output) new StringEntry added to front
**                of hash chain. 
**    HashValue - (input) index into HashArray[]
**    Entry     - (input/output) new StringEntry allocated from it
**   
**  EXCEPTIONS:
**   
**      none
**--
*/
static LSLSymbol
AllocEntry(char *name) {
    LSLSymbol		retVal;	    
    long unsigned	size;

    size = MaxEntry;

    /* 
     * EMPTY or FULL? 
     */
    if ((retVal = FreeEntry) == size) {
	if (size == 0) 
	    size = INITSTRINGENTRY;
        else
	    size = (unsigned)(DELTASTRINGENTRY*size);

	AllocEntrySpace(size);
	retVal = FreeEntry;
    }

    /* 
     * Next entry becomes the new head of the free list
     */
    FreeEntry = retVal+1;

    /* 
     * Initialize data fields 
     *  - enter in hash chain
     *  - enter/allocate StringTable handle
     */
    Entry[retVal].HashNext = HashArray[HashValue];
    HashArray[HashValue]    = retVal;		    
    Entry[retVal].i        = AllocChar(name);

    /* 
     * Return String-handle for allocated item 
     */
    return retVal;
}


/*
**++
**  FUNCTION NAME:
**
**      LSLStringInit()
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
**	HashArray  - (output) point to allocated/initialized hash array
**      MaxEntry   - (output) initial context is EMPTY
**      FreeEntry  - (output) no free list
**      Entry      - (output) null array of StringEntries
**      MaxChar    - (output) initial context is EMPTY
**      FreeChar   - (output) no free list
**      CharString - (output) null array of character-strings
**		     future context operations
**
**  EXCEPTIONS:
**   
**      none
**--
*/

void
LSLStringInit(void) {
    int i;

    HashArray  = (LSLSymbol *) MoreMem(0, HASHSIZE*sizeof(Handle));

    for (i=0; i<HASHSIZE; i++) {
	HashArray[i] = NULLSYMBOL;
    }

    MaxChar    = 0;
    MaxEntry   = 0;

    FreeChar   = 0;
    FreeEntry  = 0;

    CharString = (char *) 0;
    Entry      = (StringEntry *) 0;
}

void
LSLStringReset(void) {
}

void
LSLStringCleanup(void) {
}

