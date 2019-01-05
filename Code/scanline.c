/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993, 1994
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   scanline.c
**
**	PURPOSE:    Scan one line of Larch SL input at a time.
**	
**	The input is source text, line at a time.   The output is a sequence
**	of tokens, reported by call-out LSLScanFreshToken.
**
**	This organization allows implementation of line-at-a-time
**	incremental scanning.  The incremental mechanism is in the
**	driving module scan.c, which can save and replay tokens for
**	unchanged lines.  This module works either way.
**	
**	The main loop of the scanner keys on the leading character.
**	Actions within the loop collect the token starting with the 
**	and determine its kind.
**
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at WangInst -- 86.10.08 -- original release
**      {1} McKeeman at WangInst -- 87.06.11 -- from C to D
**      {2} McKeeman at WangInst -- 87.07.07 -- single scan loop
**      {3} McKeeman at Harvard  -- 88.03.01 -- went hyper
**      {4} McKeeman at Harvard  -- 88.04.12 -- from D to X
**      {5} Aki      at Digital  -- 89.06.21 -- support X2.0 RCASE
**      {6} McKeeman at Digital  -- 89.08.01 -- from X to C
**      {7} Aki      at Digital  -- 89.11.30 -- context support
**      {8} Aki      at Digital  -- 89.12.13 -- cleanup
**      {9} Aki      at Digital  -- 90.01.19 -- new naming conventions
**     {10} Aki      at Digital  -- 90.02.05 -- support include
**     {11} McKeeman at Digital  -- 90.05.07 -- C to LSL
**     {12} Feldman  at Digital	 -- 90.08.07 -- Convert for init files
**     {13} Feldman  at Digital	 -- 90.08.14 -- Fix bug in extensionChar
**     {14} Wild     at Digital  -- 90.08.16 -- Init file processing
**     {15} Garland  at MIT      -- 90.09.06 -- Provide comment tokens
**     {16} Garland  at MIT	 -- 90.10.05 -- Move keyword tokens to token.c
**						Add TrueToken, ..., NotToken
**     {17} Garland  at MIT      -- 90.11.06 -- Add locator to ScanEofToken
**     {18} Feldman  at Digital  -- 91.05.30 -- Special case /\.
**     {19} Feldman  at Digital  -- 91.06.11 -- Fix bug in /\ processing.
**     {20} Garland  at MIT      -- 91.12.20 -- Add IfToken
**     {21} Garland  at MIT      -- 92.02.19 -- Reorder #include "error.h"
**     {22} Garland  at MIT      -- 92.06.14 -- Add COMPOSESYM
**     {23} Garland  at MIT      -- 93.11.09 -- Add AllToken, ExistsToken
**     {24} Garland  at MIT      -- 93.11.26 -- Add IffToken, priority
**     {25} Garland  at MIT      -- 94.04.20 -- Add freelyTOKEN
**     {24} Garland  6t MIT      -- 94.05.20 -- Remove COMPOSESYM
**						Add sortTOKEN, LBRACK, RBRACK
**	{n} Who      at Where    -- yy.mm.dd -- Did what?
**
*/

/*
**
**  INCLUDE FILES
**
*/

# include <string.h>
# include	"general.h"
# include	"lsl.h"
# include	"string2sym.h"
# include	"token.h"
# include 	"error.h"
# include	"source.h"
# include	"scan.h"
# include	"scanline.h"
# include	"tokentable.h"
# include	"syntable.h"

/*
**
**  MACRO DEFINITIONS
**
*/


# define	    cClass LSLScanCharClass
# define	    FreshToken LSLScanFreshToken

# define	    MoveChar() {				    \
			*bufptr++ = c;				    \
			c = *currentLine++;			    \
			colNumber++;				    \
		    }

# define    LookaheadChar()	    *currentLine
 
# define MAXCHAR     512			/* storage for a lexeme     */
# define MAXRES	    1000			/* bounds on sym2cod[]	    */
# define TABSIZE       8			/* keep error ^ in synch    */

# define String2Symbol	LSLString2Symbol

# define ReserveToken	LSLReserveToken
# define InsertToken	LSLInsertToken
# define SetTokenPriority LSLSetTokenPriority

# define IsSyn		LSLIsSyn
# define GetTokenForSyn	LSLGetTokenForSyn

# define CodePart	token_code
# define TextSymPart    token_textSym
# define SetRawTextSym	token_setRawTextSym
# define SetCol		token_setCol
# define SetFileName	token_setFileName
# define SetLine	token_setLine
# define SetPriority	token_setPriority

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

extern void	    LSLScanLine();		/* Main entry here	    */
extern LSLToken	    LSLScanEofToken(void);
extern void	    LSLReportEolTokens();
extern void	    LSLReportCommentTokens();
extern void	    LSLScanOption();		/* from command line	    */

static void	    UserError();

extern void	    LSLScanLineInit();		/* Initialization (once)    */
extern void	    LSLScanLineReset();		/* Initialization (compile) */
extern void	    LSLScanLineCleanup();	/* prepare for reset	    */



/*
**
**  PUBLIC VARIABLES
**
*/

/* Array to store character class defintions and record end-of-comment	    */
/* characters.								    */
   
charClassData charClass[LASTCHAR + 1];


/*
**
**  STATIC VARIABLES
**
*/

static unsigned int colNumber;			/* current source col	    */
static unsigned int startCol;			/* start of token	    */
static bool reportEOL;				/* report EOL tokens	    */
static bool reportComments;			/* report comment tokens    */

static char	    tokenBuffer[MAXCHAR];	/* temp buffer to store the *
                                                 * text for a token         */

const charClassData charClassDef [] = {
	    /* Control characters					    */
            NULLCHARCLASS,  TRUE,	    /*	 0 NULL			    */
            SINGLECHAR,	    FALSE,	    /*	 1 CTRL-A		    */
            SINGLECHAR,	    FALSE,	    /*	 2 CTRL-B		    */
            SINGLECHAR,	    FALSE,	    /*	 3 CTRL-C		    */
            SINGLECHAR,	    FALSE,	    /*	 4 CTRL-D		    */
            SINGLECHAR,	    FALSE,	    /*	 5 CTRL-E		    */
            SINGLECHAR,	    FALSE,	    /*	 6 CTRL-F		    */
            SINGLECHAR,	    FALSE,	    /*	 7 CTRL-G		    */
            SINGLECHAR,	    FALSE,	    /*	 8 CTRL-H		    */

	    /* defined formatting characters				    */
            WHITECHAR,	    FALSE,	    /*	 9 CTRL-I   TAB		    */
            WHITECHAR,	    TRUE,	    /*	10 CTRL-J   EOL		    */

	    /* more control characters					    */
            SINGLECHAR,	    FALSE,	    /*	11 CTRL-K		    */
            SINGLECHAR,	    FALSE,	    /*	12 CTRL-L		    */
            SINGLECHAR,	    FALSE,	    /*	13 CTRL-M		    */
            SINGLECHAR,	    FALSE,	    /*	14 CTRL-N		    */
            SINGLECHAR,	    FALSE,	    /*	15 CTRL-O		    */
            SINGLECHAR,	    FALSE,	    /*	16 CTRL-P		    */
            SINGLECHAR,	    FALSE,	    /*	17 CTRL-Q		    */
            SINGLECHAR,	    FALSE,	    /*	18 CTRL-R		    */
            SINGLECHAR,	    FALSE,	    /*	19 CTRL-S		    */
            SINGLECHAR,	    FALSE,	    /*	20 CTRL-T		    */
            SINGLECHAR,	    FALSE,	    /*	21 CTRL-U		    */
            SINGLECHAR,	    FALSE,	    /*	22 CTRL-V		    */
            SINGLECHAR,	    FALSE,	    /*	23 CTRL-W		    */
            SINGLECHAR,	    FALSE,	    /*	24 CTRL-X		    */
            SINGLECHAR,	    FALSE,	    /*	25 CTRL-Y		    */
            SINGLECHAR,	    FALSE,	    /*	26 CTRL-Z		    */
            SINGLECHAR,	    FALSE,	    /*	27 CTRL-[   ESC		    */
            SINGLECHAR,	    FALSE,	    /*	28 CTRL-\   FS		    */
            SINGLECHAR,	    FALSE,	    /*	29 CTRL-]   GS		    */
            SINGLECHAR,	    FALSE,	    /*	30 CTRL-^   RS		    */
            SINGLECHAR,	    FALSE,	    /*	31 CTRL-_   US		    */

	    /* Special printing characters				    */
            WHITECHAR,	    FALSE,	    /*	32 space		    */
	    SINGLECHAR,	    FALSE,	    /*	33 !			    */
	    SINGLECHAR,	    FALSE,	    /*	34 "			    */
	    SINGLECHAR,	    FALSE,	    /*	35 #			    */
	    SINGLECHAR,	    FALSE,	    /*	36 $			    */
	    SINGLECHAR,	    FALSE,	    /*	37 %			    */
	    SINGLECHAR,	    FALSE,	    /*	38 &			    */
	    SINGLECHAR,	    FALSE,	    /*	39 '			    */

	    /* Reserved characters					    */
	    PERMCHAR,	    FALSE,	    /*	40 (			    */
	    PERMCHAR,	    FALSE,	    /*	41 )			    */
	    OPCHAR,	    FALSE,	    /*	42 *			    */
	    OPCHAR,	    FALSE,	    /*	43 +			    */
	    PERMCHAR,	    FALSE,	    /*	44 ,			    */
	    OPCHAR,	    FALSE,	    /*	45 -			    */
	    OPCHAR,	    FALSE,	    /*	46 .			    */
	    SLASHCHAR,	    FALSE,	    /*	47 /			    */

	    /* Numbers							    */
	    IDCHAR,	    FALSE,	    /*	48 0			    */
	    IDCHAR,	    FALSE,	    /*	49 1			    */
	    IDCHAR,	    FALSE,	    /*	50 2			    */
	    IDCHAR,	    FALSE,	    /*	51 3			    */
	    IDCHAR,	    FALSE,	    /*	52 4			    */
	    IDCHAR,	    FALSE,	    /*	53 5			    */
	    IDCHAR,	    FALSE,	    /*	54 6			    */
	    IDCHAR,	    FALSE,	    /*	55 7			    */
	    IDCHAR,	    FALSE,	    /*	56 8			    */
	    IDCHAR,	    FALSE,	    /*	57 9			    */

	    /* More reserved and special printing characters		    */
	    PERMCHAR,	    FALSE,	    /*	58 :			    */
	    SINGLECHAR,	    FALSE,	    /*	59 ;			    */
	    OPCHAR,	    FALSE,	    /*	60 <			    */
	    OPCHAR,	    FALSE,	    /*	61 =			    */
	    OPCHAR,	    FALSE,	    /*	62 >			    */
	    SINGLECHAR,	    FALSE,	    /*	63 ?			    */
	    SINGLECHAR,	    FALSE,	    /*	64 @			    */

	    /* Uppercase Alphabetics					    */
	    IDCHAR,	    FALSE,	    /*  65 A			    */
	    IDCHAR,	    FALSE,	    /*  66 B			    */
	    IDCHAR,	    FALSE,	    /*  67 C			    */
	    IDCHAR,	    FALSE,	    /*  68 D			    */
	    IDCHAR,	    FALSE,	    /*  69 E			    */
	    IDCHAR,	    FALSE,	    /*	70 F			    */
	    IDCHAR,	    FALSE,	    /*	71 G			    */
	    IDCHAR,	    FALSE,	    /*	72 H			    */
	    IDCHAR,	    FALSE,	    /*	73 I			    */
	    IDCHAR,	    FALSE,	    /*	74 J			    */
	    IDCHAR,	    FALSE,	    /*	75 K			    */
	    IDCHAR,	    FALSE,	    /*	76 L			    */
	    IDCHAR,	    FALSE,	    /*	77 M			    */
	    IDCHAR,	    FALSE,	    /*	78 N			    */
	    IDCHAR,	    FALSE,	    /*	79 O			    */
	    IDCHAR,	    FALSE,	    /*	80 P			    */
	    IDCHAR,	    FALSE,	    /*	81 Q			    */
	    IDCHAR,	    FALSE,	    /*	82 R			    */
	    IDCHAR,	    FALSE,	    /*	83 S			    */
	    IDCHAR,	    FALSE,	    /*	84 T			    */
	    IDCHAR,	    FALSE,	    /*	85 U			    */
	    IDCHAR,	    FALSE,	    /*	86 V			    */
	    IDCHAR,	    FALSE,	    /*	87 W			    */
	    IDCHAR,	    FALSE,	    /*	88 X			    */
	    IDCHAR,	    FALSE,	    /*	89 Y			    */
	    IDCHAR,	    FALSE,	    /*	90 Z			    */

	    /* Still more reserved and special printing characters	    */
	    SINGLECHAR,	    FALSE,	    /*	91 [			    */
	    EXTENSIONCHAR,  FALSE,	    /*	92 \			    */
	    SINGLECHAR,	    FALSE,	    /*	93 ]			    */
	    SINGLECHAR,	    FALSE,	    /*	94 ^			    */
	    IDCHAR,	    FALSE,	    /*	95 _			    */
	    SINGLECHAR,	    FALSE,	    /*	96 `			    */

	    /* Lowercase alphabetics					    */
	    IDCHAR,	    FALSE,	    /*	97 a			    */
	    IDCHAR,	    FALSE,	    /*	98 b			    */
	    IDCHAR,	    FALSE,	    /*	99 c			    */
	    IDCHAR,	    FALSE,	    /*	100 d			    */
	    IDCHAR,	    FALSE,	    /*	101 e			    */
	    IDCHAR,	    FALSE,	    /*	102 f			    */
	    IDCHAR,	    FALSE,	    /*	103 g			    */
	    IDCHAR,	    FALSE,	    /*	104 h			    */
	    IDCHAR,	    FALSE,	    /*	105 i			    */
	    IDCHAR,	    FALSE,	    /*	106 j			    */
	    IDCHAR,	    FALSE,	    /*	107 k			    */
	    IDCHAR,	    FALSE,	    /*	108 l			    */
	    IDCHAR,	    FALSE,	    /*	109 m			    */
	    IDCHAR,	    FALSE,	    /*	110 n			    */
	    IDCHAR,	    FALSE,	    /*	111 o			    */
	    IDCHAR,	    FALSE,	    /*	112 p			    */
	    IDCHAR,	    FALSE,	    /*	113 q			    */
	    IDCHAR,	    FALSE,	    /*	114 r			    */
	    IDCHAR,	    FALSE,	    /*	115 s			    */
	    IDCHAR,	    FALSE,	    /*	116 t			    */
	    IDCHAR,	    FALSE,	    /*	117 u			    */
	    IDCHAR,	    FALSE,	    /*	118 v			    */
	    IDCHAR,	    FALSE,	    /*	119 w			    */
	    IDCHAR,	    FALSE,	    /*	120 x			    */
	    IDCHAR,	    FALSE,	    /*	121 y			    */
	    IDCHAR,	    FALSE,	    /*  122 z			    */

	    SINGLECHAR,	    FALSE,	    /*	123 {			    */
	    SINGLECHAR,	    FALSE,	    /*	124 |			    */
	    SINGLECHAR,	    FALSE,	    /*	125 }			    */
	    SINGLECHAR,	    FALSE,	    /*	126 ~			    */
	    SINGLECHAR,	    FALSE,	    /*	127 DEL			    */

	    /* MCS - unused in English					    */
	    SINGLECHAR,	    FALSE,	    /*	128			    */
	    SINGLECHAR,	    FALSE,	    /*	129			    */
	    SINGLECHAR,	    FALSE,	    /*	130			    */
	    SINGLECHAR,	    FALSE,	    /*	131			    */
	    SINGLECHAR,	    FALSE,	    /*	132			    */
	    SINGLECHAR,	    FALSE,	    /*	133			    */
	    SINGLECHAR,	    FALSE,	    /*	134			    */
	    SINGLECHAR,	    FALSE,	    /*	135			    */
	    SINGLECHAR,	    FALSE,	    /*	136			    */
	    SINGLECHAR,	    FALSE,	    /*	137			    */
	    SINGLECHAR,	    FALSE,	    /*	138			    */
	    SINGLECHAR,	    FALSE,	    /*	139			    */
	    SINGLECHAR,	    FALSE,	    /*	140			    */
	    SINGLECHAR,	    FALSE,	    /*	141			    */
	    SINGLECHAR,	    FALSE,	    /*	142			    */
	    SINGLECHAR,	    FALSE,	    /*	143			    */
	    SINGLECHAR,	    FALSE,	    /*	144			    */
	    SINGLECHAR,	    FALSE,	    /*	145			    */
	    SINGLECHAR,	    FALSE,	    /*	146			    */
	    SINGLECHAR,	    FALSE,	    /*	147			    */
	    SINGLECHAR,	    FALSE,	    /*	148			    */
	    SINGLECHAR,	    FALSE,	    /*	149			    */
	    SINGLECHAR,	    FALSE,	    /*	150			    */
	    SINGLECHAR,	    FALSE,	    /*	151			    */
	    SINGLECHAR,	    FALSE,	    /*	152			    */
	    SINGLECHAR,	    FALSE,	    /*	153			    */
	    SINGLECHAR,	    FALSE,	    /*	154			    */
	    SINGLECHAR,	    FALSE,	    /*	155			    */
	    SINGLECHAR,	    FALSE,	    /*	156			    */
	    SINGLECHAR,	    FALSE,	    /*	157			    */
	    SINGLECHAR,	    FALSE,	    /*	158			    */
	    SINGLECHAR,	    FALSE,	    /*	159			    */
	    SINGLECHAR,	    FALSE,	    /*	160			    */
	    SINGLECHAR,	    FALSE,	    /*	161			    */
	    SINGLECHAR,	    FALSE,	    /*	162			    */
	    SINGLECHAR,	    FALSE,	    /*	163			    */
	    SINGLECHAR,	    FALSE,	    /*	164			    */
	    SINGLECHAR,	    FALSE,	    /*	165			    */
	    SINGLECHAR,	    FALSE,	    /*	166			    */
	    SINGLECHAR,	    FALSE,	    /*	167			    */
	    SINGLECHAR,	    FALSE,	    /*	168			    */
	    SINGLECHAR,	    FALSE,	    /*	169			    */
	    SINGLECHAR,	    FALSE,	    /*	170			    */
	    SINGLECHAR,	    FALSE,	    /*	171			    */
	    SINGLECHAR,	    FALSE,	    /*	172			    */
	    SINGLECHAR,	    FALSE,	    /*	173			    */
	    SINGLECHAR,	    FALSE,	    /*	174			    */
	    SINGLECHAR,	    FALSE,	    /*	175			    */
	    SINGLECHAR,	    FALSE,	    /*	176			    */
	    SINGLECHAR,	    FALSE,	    /*	177			    */
	    SINGLECHAR,	    FALSE,	    /*	178			    */
	    SINGLECHAR,	    FALSE,	    /*	179			    */
	    SINGLECHAR,	    FALSE,	    /*	180			    */
	    SINGLECHAR,	    FALSE,	    /*	181			    */
	    SINGLECHAR,	    FALSE,	    /*	182			    */
	    SINGLECHAR,	    FALSE,	    /*	183			    */
	    SINGLECHAR,	    FALSE,	    /*	184			    */
	    SINGLECHAR,	    FALSE,	    /*	185			    */
	    SINGLECHAR,	    FALSE,	    /*	186			    */
	    SINGLECHAR,	    FALSE,	    /*	187			    */
	    SINGLECHAR,	    FALSE,	    /*	188			    */
	    SINGLECHAR,	    FALSE,	    /*	189			    */
	    SINGLECHAR,	    FALSE,	    /*	190			    */
	    SINGLECHAR,	    FALSE,	    /*	191			    */
	    SINGLECHAR,	    FALSE,	    /*	192			    */
	    SINGLECHAR,	    FALSE,	    /*	193			    */
	    SINGLECHAR,	    FALSE,	    /*	194			    */
	    SINGLECHAR,	    FALSE,	    /*	195			    */
	    SINGLECHAR,	    FALSE,	    /*	196			    */
	    SINGLECHAR,	    FALSE,	    /*	197			    */
	    SINGLECHAR,	    FALSE,	    /*	198			    */
	    SINGLECHAR,	    FALSE,	    /*	199			    */
	    SINGLECHAR,	    FALSE,	    /*	200			    */
	    SINGLECHAR,	    FALSE,	    /*	201			    */
	    SINGLECHAR,	    FALSE,	    /*	202			    */
	    SINGLECHAR,	    FALSE,	    /*	203			    */
	    SINGLECHAR,	    FALSE,	    /*	204			    */
	    SINGLECHAR,	    FALSE,	    /*	205			    */
	    SINGLECHAR,	    FALSE,	    /*	206			    */
	    SINGLECHAR,	    FALSE,	    /*	207			    */
	    SINGLECHAR,	    FALSE,	    /*	208			    */
	    SINGLECHAR,	    FALSE,	    /*	209			    */
	    SINGLECHAR,	    FALSE,	    /*	210			    */
	    SINGLECHAR,	    FALSE,	    /*	211			    */
	    SINGLECHAR,	    FALSE,	    /*	212			    */
	    SINGLECHAR,	    FALSE,	    /*	213			    */
	    SINGLECHAR,	    FALSE,	    /*	214			    */
	    SINGLECHAR,	    FALSE,	    /*	215			    */
	    SINGLECHAR,	    FALSE,	    /*	216			    */
	    SINGLECHAR,	    FALSE,	    /*	217			    */
	    SINGLECHAR,	    FALSE,	    /*	218			    */
	    SINGLECHAR,	    FALSE,	    /*	219			    */
	    SINGLECHAR,	    FALSE,	    /*	220			    */
	    SINGLECHAR,	    FALSE,	    /*	221			    */
	    SINGLECHAR,	    FALSE,	    /*	222			    */
	    SINGLECHAR,	    FALSE,	    /*	223			    */
	    SINGLECHAR,	    FALSE,	    /*	224			    */
	    SINGLECHAR,	    FALSE,	    /*	225			    */
	    SINGLECHAR,	    FALSE,	    /*	226			    */
	    SINGLECHAR,	    FALSE,	    /*	227			    */
	    SINGLECHAR,	    FALSE,	    /*	228			    */
	    SINGLECHAR,	    FALSE,	    /*	229			    */
	    SINGLECHAR,	    FALSE,	    /*	230			    */
	    SINGLECHAR,	    FALSE,	    /*	231			    */
	    SINGLECHAR,	    FALSE,	    /*	232			    */
	    SINGLECHAR,	    FALSE,	    /*	233			    */
	    SINGLECHAR,	    FALSE,	    /*	234			    */
	    SINGLECHAR,	    FALSE,	    /*	235			    */
	    SINGLECHAR,	    FALSE,	    /*	236			    */
	    SINGLECHAR,	    FALSE,	    /*	237			    */
	    SINGLECHAR,	    FALSE,	    /*	238			    */
	    SINGLECHAR,	    FALSE,	    /*	239			    */
	    SINGLECHAR,	    FALSE,	    /*	240			    */
	    SINGLECHAR,	    FALSE,	    /*	241			    */
	    SINGLECHAR,	    FALSE,	    /*	242			    */
	    SINGLECHAR,	    FALSE,	    /*	243			    */
	    SINGLECHAR,	    FALSE,	    /*	244			    */
	    SINGLECHAR,	    FALSE,	    /*	245			    */
	    SINGLECHAR,	    FALSE,	    /*	246			    */
	    SINGLECHAR,	    FALSE,	    /*	247			    */
	    SINGLECHAR,	    FALSE,	    /*	248			    */
	    SINGLECHAR,	    FALSE,	    /*	249			    */
	    SINGLECHAR,	    FALSE,	    /*	250			    */
	    SINGLECHAR,	    FALSE,	    /*	251			    */
	    SINGLECHAR,	    FALSE,	    /*	252			    */
	    SINGLECHAR,	    FALSE,	    /*	253			    */
	    SINGLECHAR,	    FALSE,	    /*	254			    */
	    SINGLECHAR,	    FALSE	    /*	255			    */
	    };

/*
**++
**  FUNCTION NAME:
**
**      LSLScanline()
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
**      [@description or none@]
**   
**  DESCRIPTION:
**
**	One line of text is processed.
**	Tokens are delivered via the call LSLScanFreshToken().
**
**  EXCEPTIONS:
**
**--
*/

void
LSLScanLine(char *currentLine) {
    LSLTokenCode	cod;			/* temporary token holder   */
    LSLSymbol		sym;			/* temp symbol holder       */
    register int        c;			/* fast local ch            */
    register char	*bufptr;
    LSLToken		newToken;		/* token scanned */


    c = *currentLine++;				/* first ch of line	    */
    colNumber = 0;				/* start of line	    */

    for (;;) {                                  /* send token sequence	    */
	bufptr = &tokenBuffer[0];		/* temp for strings	    */
	startCol = colNumber;			/* start of token	    */

        switch (cClass(c)) {			/* main switch		    */

	case NULLCHARCLASS:			/* null ends line	    */
	    sym = String2Symbol("E O L");
	    cod = EOLTOKEN;
	    break;

	/* - - - - - - - - - - - -  identifiers - - - - - - - - - - - - - - */

	case IDCHAR:

	    while (cClass(c) == IDCHAR) {	/* identifier: find end	    */
		MoveChar();
	    }

	    *bufptr = '\0';			/* null terminate in buffer */
	    sym	    = String2Symbol(&tokenBuffer[0]);
            cod	    = SIMPLEID;
	    break;

	/* - - - - - - - - - - one-character tokens - - - - - - - - - - - - */

	case SINGLECHAR:
	case PERMCHAR:				/* one character tokens     */
	    MoveChar();
	    *bufptr	= '\0';			/* null terminate in buffer */
	    sym		= String2Symbol(&tokenBuffer[0]);
            cod		= SIMPLEOP;
	    break;

	/* - - - - - - - - - - special case for /\  - - - - - - - - - - - - */

	case SLASHCHAR:
	    if (LookaheadChar() == '\\') {
		MoveChar();
		MoveChar();
		*bufptr = '\0';
		sym = String2Symbol(&tokenBuffer[0]);
		cod = SIMPLEOP;
		break;
	    }
	    MoveChar();
	    /* WARNING: we fall through to next case if we have / followed  */
	    /* by anything else.					    */

	case OPCHAR:

	/* - - - - - - - - - - operator symbols - - - - - - - - - - - - - - */

	                                        /* possible multi character */
	    while (cClass(c) == OPCHAR) {	/* operator: find end	    */
		MoveChar();
	    }

	    *bufptr = '\0';			/* null terminate in buffer */
	    sym	    = String2Symbol(&tokenBuffer[0]);
            cod	    = SIMPLEOP;
	    break;

	/* - - - - - - - - - - -  white space - - - - - - - - - - - - - - - */
        case WHITECHAR:
	    switch (c) {
		case '\t':
		    MoveChar();
		    colNumber--;
		    colNumber += TABSIZE;
		    colNumber -= (colNumber%TABSIZE);
		    break;

		case '\v':
		case '\f':
		    MoveChar();
		    colNumber--;		/* does not change column   */
		    break;

	    	default:
		    MoveChar();
		    break;
	    }
	    *bufptr = '\0';			/* null terminate in buffer */
            sym     = String2Symbol(&tokenBuffer[0]);	    
	    cod	    = WHITESPACE;
	    break;

	/* - - - - - - - - - - - -  \ symbols   - - - - - - - - - - - - - - */

	case EXTENSIONCHAR:			/* \ something		    */
	    MoveChar();

	    switch (c) {			/* the something	    */

	    /* - - - - - - - - - - - open and close - - - - - - - - - - - - */
	    case '(':				/* \(identifier		    */
		MoveChar();
		while (cClass(c) == IDCHAR) {	/* opensym: find end	    */
		    MoveChar();
		}
		*bufptr	    = '\0';		/* null terminate in buffer */
		sym	    = String2Symbol(&tokenBuffer[0]);
		cod	    = OPENSYM;
		break;

	    case ')':				/* \)identifier		    */
		MoveChar();
		while (cClass(c) == IDCHAR) {	/* closesym: find end	    */
		    MoveChar();
		}
		*bufptr	    = '\0';		/* null terminate in buffer */
		sym	    = String2Symbol(&tokenBuffer[0]);
		cod	    = CLOSESYM;
		break;

	    /* - - - - - - - - - - -  separator - - - - - - - - - - - - - - */
	    case ',':				/* \,identifier		    */
		MoveChar();
		while (cClass(c) == IDCHAR) {	/* separator: find end	    */
		    MoveChar();
		}
		*bufptr	    = '\0';		/* null terminate in buffer */
		sym	    = String2Symbol(&tokenBuffer[0]);
		cod	    = SEPSYM;
		break;

	    /* - - - - - - - - - - -  simpleid  - - - - - - - - - - - - - - */
	    case ':':				/* \:identifier		    */
		MoveChar();
		while (cClass(c) == IDCHAR) {	/* simpleid: find end	    */
		    MoveChar();
		}
		*bufptr	    = '\0';		/* null terminate in buffer */
		sym	    = String2Symbol(&tokenBuffer[0]);
		cod	    = SIMPLEID;
		break;

	    default:				/* \ anything else	    */
		if (cClass(c) == IDCHAR) {
		    do {
		    	  MoveChar();
		    } while (cClass(c) == IDCHAR);
		    *bufptr	    = '\0';	/* null terminate in buffer */
		    sym	    = String2Symbol(&tokenBuffer[0]);
		    cod	    = SIMPLEOP;
		} else {
		    /* Meets none of the above.  Take the extension	    */
		    /* character and the character following and treat	    */
		    /* together as a SINGLECHAR.  SINGLECHARs tranlate into */
		    /* SIMPLEOPs.					    */
		    MoveChar();
		    *bufptr	    = '\0';	/* null terminate in buffer */
		    sym	    = String2Symbol(&tokenBuffer[0]);
		    cod	    = SIMPLEOP;
		}
		break;
	    }
	    break;				/* end of case extensionChar*/
		
        default:

            UserError("unexpected character in input");
	    return;
        }

	/* Above code only "guessed" at token type.  Insert it into the */
	/* TokenTable.  If the token already exists, it is returned as	    */
	/* previously defined.  If it does not exist, it is inserted as the */
	/* token code computed above.					    */

	newToken = InsertToken(cod, sym, 0, FALSE);

	if (IsSyn(TextSymPart(newToken))) {
	    /* Token is a synonym.  Get the actual token and set the raw    */
	    /* text to the synonym name.				    */
	    newToken = GetTokenForSyn(TextSymPart(newToken));
	    SetRawTextSym(newToken, sym);
	}

	SetCol(newToken, startCol);
	SetLine(newToken, source_thisLineNumber(LSLScanSource()));
	SetFileName(newToken, source_fileName(LSLScanSource()));
        if (CodePart(newToken) == COMMENTSYM) {
	    /* Get text of comment. */
	    bufptr = &tokenBuffer[0];
	    while (! LSLIsEndComment(c)) {
		MoveChar();
	    }
	    if (cClass(c) != NULLCHARCLASS) {
		/* Get last character if not EOL. */
		MoveChar();
	    }
	    if (reportComments) {
		*bufptr  = '\0';
		SetRawTextSym(newToken, String2Symbol(&tokenBuffer[0]));
		FreshToken(newToken);
	    }
	} else if (CodePart(newToken) == EOLTOKEN) {
	    /* Hit end of line.  Report it if requested and return. */
	    if (reportEOL) {
		FreshToken(newToken);
	    }
	    return;
	} else if (cod != WHITESPACE) {
	    /* real token. Push it out. */
	    FreshToken(newToken);
	}
    }
}

LSLToken
LSLScanEofToken(void) {
    LSLToken t = InsertToken(EOFTOKEN, String2Symbol("E O F"), 0, TRUE);
    SetCol(t, colNumber);
    SetLine(t, source_thisLineNumber(LSLScanSource()));
    SetFileName(t, source_fileName(LSLScanSource()));
    return t;
}

void
LSLReportEolTokens(bool setting) {
    reportEOL = setting;
}

void
LSLReportCommentTokens(bool setting) {
    reportComments = setting;
}

void
LSLScanOption(int flagName, int flagValue) {
    switch (flagName) {
    case DIGITAL:
	switch (flagValue) {
	case TRUE:
	    break;
	case FALSE:
	    break;
	}
	break;
    case CXX:
	error_programFailure("LSLScanOption() C++ not implemented");
	break;
    default:
	UserError("unexpected command line option");
	break;
    }
}


static void
UserError(char *msg) {
    source *s = LSLScanSource();
    error_reportLocation(source_fileName(s), source_thisLineNumber(s), 
			 colNumber);
    error_reportAndQuit(msg);
}



/*
**++
**  FUNCTION NAME:
**
**      LSLScanLineInit()
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
**      [@description or none@]   
**   
**  DESCRIPTION:
**
**      Initialize this module (should only be called once).
**
**  IMPLICIT INPUTS/OUTPUT:
**
**      GetNextLine - (output) initialized
**	NullToken   - (output) initialized
**      PrintName   - (output) array contents initialized
**
**  EXCEPTIONS:
**
**      None   
**--
*/
void
LSLScanLineInit(void) {
    int i;

    reportEOL = FALSE;
    reportComments = FALSE;

    for (i=0;i<=LASTCHAR;i++) {
	charClass[i] = charClassDef[i];
    }

    /*
    ** NOTE: The following line ensures that all tokens have nonzero
    ** handles, so that a handle of zero can be used to indicate that a
    ** token does not have a synonym.
    */
    LSLReserveToken(SIMPLEID, "dummy token");
 
    ForallToken =   ReserveToken(QUANTIFIERSYM,	    "\\forall");
    AllToken    =   ReserveToken(QUANTIFIERSYM1,    "\\A");
    ExistsToken =   ReserveToken(QUANTIFIERSYM1,    "\\E");

    TrueToken =	    ReserveToken(SIMPLEID,		    "true");
    FalseToken =    ReserveToken(SIMPLEID,		    "false");
    NotToken =	    ReserveToken(SIMPLEOP,		    "\\not");
    AndToken =	    ReserveToken(LOGICALOP,		    "\\and");
    OrToken =	    ReserveToken(LOGICALOP,		    "\\or");
    ImpliesToken =  ReserveToken(LOGICALOP,		    "\\implies");
    IffToken =      ReserveToken(LOGICALOP,		    "\\iff");

    EqToken =	    ReserveToken(EQOP,			    "\\eq");
    NeqToken =	    ReserveToken(EQOP,			    "\\neq");

    EqualsToken =   ReserveToken(EQUATIONSYM,		    "\\equals");
    EqsepToken =    ReserveToken(EQSEPSYM,		    "\\eqsep");
    SelectToken =   ReserveToken(SELECTSYM,		    "\\select");
    OpenToken =	    ReserveToken(OPENSYM,		    "\\(");
    SepToken =	    ReserveToken(SEPSYM,		    "\\,");
    CloseToken =    ReserveToken(CLOSESYM,		    "\\)");
    SimpleidToken = ReserveToken(SIMPLEID,		    "\\:");
    ArrowToken =    ReserveToken(MAPSYM,		    "\\arrow");
    MarkerToken =   ReserveToken(MARKERSYM,		    "\\marker");
    CommentToken =  ReserveToken(COMMENTSYM,		    "\\comment");


    /* Set priorities in token table */
    SetTokenPriority(token_textSym(IfToken), 	ifPRIORITY);
    SetTokenPriority(token_textSym(IffToken), 	iffPRIORITY);
    SetTokenPriority(token_textSym(ImpliesToken), impliesPRIORITY);
    SetTokenPriority(token_textSym(AndToken), 	andorPRIORITY);
    SetTokenPriority(token_textSym(OrToken), 	andorPRIORITY);
    SetTokenPriority(token_textSym(EqToken), 	eqPRIORITY);
    SetTokenPriority(token_textSym(NeqToken), 	eqPRIORITY);

    /* And again in reserved tokens */
    SetPriority(IfToken, 	ifPRIORITY);
    SetPriority(IffToken, 	iffPRIORITY);
    SetPriority(ImpliesToken, 	impliesPRIORITY);
    SetPriority(AndToken, 	andorPRIORITY);
    SetPriority(OrToken, 	andorPRIORITY);
    SetPriority(EqToken, 	eqPRIORITY);
    SetPriority(NeqToken, 	eqPRIORITY);

    ReserveToken(LPAR,			    "(");
    ReserveToken(RPAR,			    ")");
    ReserveToken(LBRACK,		    "[");
    ReserveToken(RBRACK,		    "]");
    ReserveToken(COMMA,			    ",");
    ReserveToken(COLON,			    ":");

    ReserveToken(WHITESPACE,		    " ");
    ReserveToken(WHITESPACE,		    "\t");
    ReserveToken(WHITESPACE,		    "\n");

    ReserveToken(EOFTOKEN,		    "E O F");
    ReserveToken(EOLTOKEN,		    "E O L");

    ReserveToken(assertsTOKEN,		    "asserts");
    ReserveToken(assumesTOKEN,		    "assumes");
    ReserveToken(byTOKEN,		    "by");
    ReserveToken(convertsTOKEN,		    "converts");
    ReserveToken(elseTOKEN,		    "else");
    ReserveToken(enumerationTOKEN,	    "enumeration");
    ReserveToken(equationsTOKEN,	    "equations");
    ReserveToken(exemptingTOKEN,	    "exempting");
    ReserveToken(forTOKEN,		    "for");
    ReserveToken(freelyTOKEN,		    "freely");
    ReserveToken(generatedTOKEN,	    "generated");
    ReserveToken(ifTOKEN,		    "if");
    ReserveToken(impliesTOKEN,		    "implies");
    ReserveToken(includesTOKEN,		    "includes");
    ReserveToken(introducesTOKEN,	    "introduces");
    ReserveToken(ofTOKEN,		    "of");
    ReserveToken(partitionedTOKEN,	    "partitioned");
    ReserveToken(sortTOKEN,		    "sort");
    ReserveToken(thenTOKEN,		    "then");
    ReserveToken(traitTOKEN,		    "trait");
    ReserveToken(tupleTOKEN,		    "tuple");
    ReserveToken(unionTOKEN,		    "union");
}

void
LSLScanLineReset(void) {
}

void
LSLScanLineCleanup(void) {
}
