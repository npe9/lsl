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
**      FILENAME:   LSLINIT.C
**
**	PURPOSE:    Processor for Larch Shared Language Init Files
**	
**  MODIFICATION HISTORY:
**
**      {0} Wild    at Digital -- 90.08.06 -- original
**      {1} Wild    at Digital -- 90.08.27 -- make redef on same line an error.
**	{2} Garland at MIT     -- 90.09.06 -- Change #define EolToken
**	{3} Garland at MIT     -- 90.10.04 -- Change error reporting
**      {4} Wild    at Digital __ 90.11.05 -- Fix error reporting bugs and
**                                            EquationSymTok bug
**	{5} Feldman at Digital -- 91.03.14 -- Change occurrences of TextPart
**					      to token_text, to work around a
**					      bug in the DEC C compiler.
**	{6} Feldman at Digital -- 91.03.15 -- Change TextSymPart, because of
**					      same bug (gets through the
**					      compiler, but generates external
**					      references to TextSymPart instead
**					      of token_textSym).
**      {7} Garland at MIT     -- 92.02.19 -- Reorder #include "error.h"
**	{n} Who	    at Where   -- yy.mm.dd -- did what?
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include <string.h>
# include		"general.h"
# include		"string2sym.h"
# include		"token.h"
# include		"error.h"
# include		"source.h"
# include		"scan.h"
# include		"scanline.h"
# include		"tokentable.h"
# include		"syntable.h"
# include		"lslinit.h"
# include		"shift.h" 

/*
**
**  MACRO DEFINITIONS
**
*/


# if TRACING == 1
#    define Trace(rule) printf("Reducing: %s\n", rule)
# else
#    define Trace(rule)
# endif


# define Shift		    LSLGenShift
# define TopPopShiftStack   LSLGenTopPopShiftStack
# define IsEmptyShiftStack  LSLGenIsEmptyShiftStack

# define InsertToken	    LSLInsertToken
# define UpdateToken	    LSLUpdateToken
# define GetToken	    LSLGetToken

# define ScanCharClass	    LSLScanCharClass
# define SetCharClass	    LSLSetCharClass
# define SetEndCommentChar  LSLSetEndCommentChar
# define IsEndComment	    LSLIsEndComment

# define Scan		    LSLScanNextToken
# define LookAhead	    LSLScanLookAhead

# define EolToken	    LSLReportEolTokens
# define IsChar		    token_isChar
# define TextSymPart	    token_textSym
# define RawTextSymPart	    token_rawTextSym
# define TextPart	    token_text
# define CodePart	    token_code
# define ColPart	    token_col
# define DefinedPart	    token_defined
# define HasSynPart	    token_hasSyn
# define WasSyn		    token_wasSyn

# define AddSyn		    LSLAddSyn
# define IsSyn		    LSLIsSyn

# define String2Symbol	    LSLString2Symbol
/*
**
**  TYPEDEFS (none)
**
*/



/*
**
**  EXTERNAL VARIABLES (none)
**
*/



/*
**
**  FORWARD FUNCTIONS
**
*/

extern void		LSLProcessInitFile();

static void		InitFile();
static void		InitLines();
static void		InitLine();
static void		Classification();
static void		CharClass();

static void		EndCommentChars();
static void		IdChars();
static void		OpChars();
static void		ExtensionChar();
static void		SingChars();
static void		WhiteChars();
static void		EndCommentChar();
static void		IdChar();
static void		OpChar();
static void		SingChar();
static void		WhiteChar();

static void		TokenClass();
static void		QuantifierSymToks();
static void		LogicalOpToks();
static void		EqOpToks();
static void		EquationSymToks();
static void		EqSepSymToks();
static void		SelectSymToks();
static void		OpenSymToks();
static void		SepSymToks();
static void		CloseSymToks();
static void		SimpleIdToks();
static void		MapSymToks();
static void		MarkerSymToks();
static void		CommentSymToks();
static void		QuantifierSymTok();
static void		LogicalOpTok();
static void		EqOpTok();
static void		EquationSymTok();
static void		EqSepSymTok();
static void		SelectSymTok();
static void		OpenSymTok();
static void		SepSymTok();
static void		CloseSymTok();
static void		SimpleIdTok();
static void		MapSymTok();
static void		MarkerSymTok();
static void		CommentSymTok();
static void		SynClass();
static void		OldToken();
static void		NewToken();
static void		Token();

static void		InitReduce();
static void		UpdateXCharKeywords();
static void		ProcessExtensionChar();
static void		ProcessEndCommentChar();
static void		ProcessSingleChar();
static void		ProcessToken();
static void		ProcessSynonym();

static void		UserError();

extern void		LSLProcessInitFileInit();
extern void		LSLProcessInitFileReset();
extern void		LSLProcessInitFileCleanup();

/*
**
**  STATIC VARIABLES
**
*/

static LSLToken		nextToken;

/* If TRUE character has been redefined as a singleChar. */
static char		defineSingleChar[LASTCHAR+1];

static charCode		currentExtensionChar;

/* LSL init file keyword tokens.  */

static LSLToken endCommentCharToken;
static LSLToken idCharToken;
static LSLToken opCharToken;
static LSLToken extensionCharToken;
static LSLToken singleCharToken;
static LSLToken whiteCharToken;

static LSLToken quantifierSymToken;
static LSLToken logicalOpToken;
static LSLToken eqOpToken;
static LSLToken equationSymToken;
static LSLToken eqSepSymToken;
static LSLToken selectSymToken;
static LSLToken openSymToken;
static LSLToken sepSymToken;
static LSLToken closeSymToken;
static LSLToken simpleIdToken;
static LSLToken mapSymToken;
static LSLToken markerSymToken;
static LSLToken commentSymToken;

static LSLToken synonymToken;


/*
**
**  FUNCTION DEFINITIONS
**
*/

void
LSLProcessInitFile(void) {

    InitFile();
}

/*
**
**  Parsing functions for init file processing, in the same order as the
**  grammar file lslinit.cfg.  This is top-down order, as much as possible.
**
*/

static void
InitFile(void) {

    InitLines();

    InitReduce(INITFILE1);

    if (CodePart(nextToken) != EOFTOKEN) {
        UserError(nextToken, "unexpected tokens after end-of-file");
    }
}

static void
InitLines(void) {

    InitReduce(INITLINES1);

    if (CodePart(nextToken) != EOFTOKEN) {
	InitLine();
	InitReduce(INITLINES2);
    }

    while (CodePart(nextToken) != EOFTOKEN) {
    	InitLine();
	InitReduce(INITLINES3);
    }

}

static void
InitLine(void) {

    if (CodePart(nextToken) == EOLTOKEN) {
	/* Nothing on line. */
	InitReduce(INITLINE1);
    }
    else {
	Classification();
	InitReduce(INITLINE2);
    }

    if (CodePart(nextToken) != EOLTOKEN) {
        UserError(nextToken, "unexpected tokens on line");
    }

    nextToken = Scan();				    /* Discard EOL	*/
}

static void
Classification(void) {

    if (RawTextSymPart(nextToken) == token_textSym(endCommentCharToken)
    ||  RawTextSymPart(nextToken) == token_textSym(idCharToken)
    ||  RawTextSymPart(nextToken) == token_textSym(opCharToken)
    ||  RawTextSymPart(nextToken) == token_textSym(extensionCharToken)
    ||  RawTextSymPart(nextToken) == token_textSym(singleCharToken)
    ||  RawTextSymPart(nextToken) == token_textSym(whiteCharToken)) {
    	CharClass();
	InitReduce(CLASSIFICATION1);
    }
    else if (RawTextSymPart(nextToken) == token_textSym(quantifierSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(logicalOpToken)
         ||  RawTextSymPart(nextToken) == token_textSym(eqOpToken)
         ||  RawTextSymPart(nextToken) == token_textSym(equationSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(eqSepSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(selectSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(openSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(sepSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(closeSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(simpleIdToken)
         ||  RawTextSymPart(nextToken) == token_textSym(mapSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(markerSymToken)
         ||  RawTextSymPart(nextToken) == token_textSym(commentSymToken)) {
        TokenClass();
	InitReduce(CLASSIFICATION2);
    }
    else if (RawTextSymPart(nextToken) == token_textSym(synonymToken)) {
    	SynClass();
	InitReduce(CLASSIFICATION3);
    }
    else {
	UserError(nextToken,
		  "expected character, token, or synonym classification");
    }
}

static void
CharClass (void) {
    LSLToken	charClassToken;

    charClassToken = nextToken;

    nextToken = Scan();			/* Discard char class keyword. */

    if (RawTextSymPart(charClassToken) == token_textSym(endCommentCharToken)) {
    	EndCommentChars();
	InitReduce(CHARCLASS1);
    }
    else if (RawTextSymPart(charClassToken) == token_textSym(idCharToken)) {
	IdChars();
	InitReduce(CHARCLASS2);
    }
    else if (RawTextSymPart(charClassToken) == token_textSym(opCharToken)) {
    	OpChars();
	InitReduce(CHARCLASS3);
    }
    else if (RawTextSymPart(charClassToken)
	     == token_textSym(extensionCharToken)) {
    	ExtensionChar();
	InitReduce(CHARCLASS4);
    }
    else if (RawTextSymPart(charClassToken) == token_textSym(singleCharToken)) {
    	SingChars();
	InitReduce(CHARCLASS5);
    }
    else if (RawTextSymPart(charClassToken) == token_textSym(whiteCharToken)) {
    	WhiteChars();
	InitReduce(CHARCLASS6);
    }
    else {
	UserError(nextToken, "expected character classification");
    }

}

static void
EndCommentChars(void) {

    EndCommentChar();
    InitReduce(ENDCOMMENTCHARS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	EndCommentChar();
	InitReduce(ENDCOMMENTCHARS2);
    }

}

static void
IdChars(void) {

    IdChar();
    InitReduce(IDCHARS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	IdChar();
	InitReduce(IDCHARS2);
    }
}

static void
OpChars(void) {

    OpChar();
    InitReduce(OPCHARS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	OpChar();
	InitReduce(OPCHARS2);
    }
}

static void
ExtensionChar(void) {

    if (IsChar(nextToken)
    &&  ScanCharClass(*token_text(nextToken)) == SINGLECHAR) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(EXTENSIONCHAR1);
    }
    else {
    	UserError(nextToken, "expected only one character");
    }
}

static void
SingChars(void) {

    SingChar();
    InitReduce(SINGCHARS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	SingChar();
	InitReduce(SINGCHARS2);
    }
}

static void
WhiteChars(void) {

    WhiteChar();
    InitReduce(WHITECHARS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	WhiteChar();
	InitReduce(WHITECHARS2);
    }
}

static void
EndCommentChar(void) {

    if (IsChar(nextToken)) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(ENDCOMMENTCHAR1);
    }
    else {
    	UserError(nextToken, "expected only one character");
    }
}

static void
IdChar(void) {

    if (IsChar(nextToken)
    &&  ScanCharClass(*token_text(nextToken)) == SINGLECHAR) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(IDCHAR1);
    }
    else {
    	UserError(nextToken, "character is already defined, cannot redefine");
    }
}

static void
OpChar(void) {

    if (IsChar(nextToken)
    &&  ScanCharClass(*token_text(nextToken)) == SINGLECHAR) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(OPCHAR1);
    }
    else {
    	UserError(nextToken, "character is already defined, cannot redefine");
    }
}

static void
SingChar(void) {

    if (IsChar(nextToken)
    &&  ScanCharClass(*token_text(nextToken)) == SINGLECHAR) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(SINGCHAR1);
    }
    else {
    	UserError(nextToken, "character is already defined, cannot redefine");
    }
}

static void
WhiteChar(void) {

    if (IsChar(nextToken)
    &&  ScanCharClass(*token_text(nextToken)) == SINGLECHAR) {
    	Shift(nextToken);
	nextToken = Scan();
	InitReduce(WHITECHAR1);
    }
    else {
    	UserError(nextToken, "character is already defined, cannot redefine");
    }
}

static void
TokenClass (void) {
    LSLToken	tokenClassToken;

    tokenClassToken = nextToken;

    nextToken = Scan();			/* Discard token class keyword. */

    if (RawTextSymPart(tokenClassToken) == token_textSym(quantifierSymToken)) {
	QuantifierSymToks();
	InitReduce(TOKENCLASS1);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(logicalOpToken)) {
	LogicalOpToks();
	InitReduce(TOKENCLASS2);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(eqOpToken)) {
	EqOpToks();
	InitReduce(TOKENCLASS3);
    }
    else if (RawTextSymPart(tokenClassToken)
	     == token_textSym(equationSymToken)) {
	EquationSymToks();
	InitReduce(TOKENCLASS4);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(eqSepSymToken)) {
	EqSepSymToks();
	InitReduce(TOKENCLASS5);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(selectSymToken)) {
	SelectSymToks();
	InitReduce(TOKENCLASS6);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(openSymToken)) {
	OpenSymToks();
	InitReduce(TOKENCLASS7);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(sepSymToken)) {
	SepSymToks();
	InitReduce(TOKENCLASS8);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(closeSymToken)) {
	CloseSymToks();
	InitReduce(TOKENCLASS9);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(simpleIdToken)) {
	SimpleIdToks();
	InitReduce(TOKENCLASS10);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(mapSymToken)) {
	MapSymToks();
	InitReduce(TOKENCLASS11);
    }
    else if (RawTextSymPart(tokenClassToken) == token_textSym(markerSymToken)) {
	MarkerSymToks();
	InitReduce(TOKENCLASS12);
    }
    else if (RawTextSymPart(tokenClassToken)
	     == token_textSym(commentSymToken)) {
	CommentSymToks();
	InitReduce(TOKENCLASS13);
    }
    else {
	UserError(nextToken, "expected token classification");
    }
}

static void
QuantifierSymToks(void) {

    QuantifierSymTok();
    InitReduce(QUANTIFIERSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	QuantifierSymTok();
	InitReduce(QUANTIFIERSYMTOKS2);
    }
}

static void
LogicalOpToks(void) {

    LogicalOpTok();
    InitReduce(LOGICALOPTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	LogicalOpTok();
	InitReduce(LOGICALOPTOKS2);
    }
}

static void
EqOpToks(void) {

    EqOpTok();
    InitReduce(EQOPTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	EqOpTok();
	InitReduce(EQOPTOKS2);
    }
}

static void
EquationSymToks(void) {

    EquationSymTok();
    InitReduce(EQUATIONSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	EquationSymTok();
	InitReduce(EQUATIONSYMTOKS2);
    }
}

static void
EqSepSymToks(void) {

    EqSepSymTok();
    InitReduce(EQSEPSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	EqSepSymTok();
	InitReduce(EQSEPSYMTOKS2);
    }
}

static void
SelectSymToks(void) {

    SelectSymTok();
    InitReduce(SELECTSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	SelectSymTok();
	InitReduce(SELECTSYMTOKS2);
    }
}

static void
OpenSymToks(void) {

    OpenSymTok();
    InitReduce(OPENSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	OpenSymTok();
	InitReduce(OPENSYMTOKS2);
    }
}

static void
SepSymToks(void) {

    SepSymTok();
    InitReduce(SEPSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	SepSymTok();
	InitReduce(SEPSYMTOKS2);
    }
}

static void
CloseSymToks(void) {

    CloseSymTok();
    InitReduce(CLOSESYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	CloseSymTok();
	InitReduce(CLOSESYMTOKS2);
    }
}

static void
SimpleIdToks(void) {

    SimpleIdTok();
    InitReduce(SIMPLEIDTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	SimpleIdTok();
	InitReduce(SIMPLEIDTOKS2);
    }
}

static void
MapSymToks(void) {

    MapSymTok();
    InitReduce(MAPSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	MapSymTok();
	InitReduce(MAPSYMTOKS2);
    }
}

static void
MarkerSymToks(void) {

    MarkerSymTok();
    InitReduce(MARKERSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	MarkerSymTok();
	InitReduce(MARKERSYMTOKS2);
    }
}

static void
CommentSymToks(void) {

    CommentSymTok();
    InitReduce(COMMENTSYMTOKS1);

    while (CodePart(nextToken) != EOLTOKEN) {
	CommentSymTok();
	InitReduce(COMMENTSYMTOKS2);
    }
}

static void
QuantifierSymTok(void) {

    Token();
    InitReduce(QUANTIFIERSYMTOK1);
}

static void
LogicalOpTok(void) {

    Token();
    InitReduce(LOGICALOPTOK1);
}

static void
EqOpTok(void) {

    Token();
    InitReduce(EQOPTOK1);
}

static void
EquationSymTok(void) {

/* ### EquationSymTok(); ### */
    Token();
    InitReduce(EQUATIONSYMTOK1);
}

static void
EqSepSymTok(void) {

    Token();
    InitReduce(EQSEPSYMTOK1);

}

static void
SelectSymTok(void) {

    Token();
    InitReduce(SELECTSYMTOK1);
}

static void
OpenSymTok(void) {

    Token();
    InitReduce(OPENSYMTOK1);
}

static void
SepSymTok(void) {

    Token();
    InitReduce(SEPSYMTOK1);
}

static void
CloseSymTok(void) {

    Token();
    InitReduce(CLOSESYMTOK1);
}

static void
SimpleIdTok(void) {

    Token();
    InitReduce(SIMPLEIDTOK1);
}

static void
MapSymTok(void) {

    Token();
    InitReduce(MAPSYMTOK1);
}

static void
MarkerSymTok(void) {

    Token();
    InitReduce(MARKERSYMTOK1);

}

static void
CommentSymTok(void) {

    Token();
    InitReduce(COMMENTSYMTOK1);
}


static void
SynClass(void) {

    if (RawTextSymPart(nextToken) == token_textSym(synonymToken)) {
	nextToken = Scan();

	OldToken();
	NewToken();

	InitReduce (SYNCLASS1);
    }
    else {
	UserError(nextToken, "expected synonym classification");
    }

}

static void
OldToken(void) {

    Token();
    InitReduce (OLDTOKEN1);

}

static void
NewToken(void) {

    Token();
    InitReduce (NEWTOKEN1);

}

static void
Token(void) {

    if (CodePart(nextToken) == EOLTOKEN
    ||  CodePart(nextToken) == EOFTOKEN) {
	UserError(nextToken, "unexpected end-of-line or end-of-file");
    }
    else {
	Shift(nextToken);
	nextToken = Scan();
    }
}


/*
 * Init File Processing Routines, these routines use the shift-reduce sequence
 * produced by the init file parser and update the necessary tables for the
 * scanner. 
 *
 * The same shift stack is used that LSL parser uses.  A different reduce
 * procedure is used because the init file grammar is different from the LSL
 * grammar.
 * 
 */


static void
InitReduce(LSLInitRuleCode rule) {
switch (rule) {

case INITFILE1:
    Trace("INITFILE1");
    break;

case INITLINES1:
    Trace("INITLINES1");
    break;

case INITLINES2:
    Trace("INITLINES2");
    break;

case INITLINES3:
    Trace("INITLINES3");
    break;

case INITLINE1:
    Trace("INITLINE1");
    break;

case INITLINE2:
    Trace("INITLINE2");
    break;

case CLASSIFICATION1:
    Trace("CLASSIFICATION1");
    break;

case CLASSIFICATION2:
    Trace("CLASSIFICATION2");
    break;

case CLASSIFICATION3:
    Trace("CLASSIFICATION3");
    break;

case CHARCLASS1:
    Trace("CHARCLASS1");
    break;

case CHARCLASS2:
    Trace("CHARCLASS2");
    break;

case CHARCLASS3:
    Trace("CHARCLASS3");
    break;

case CHARCLASS4:
    Trace("CHARCLASS4");
    break;

case CHARCLASS5:
    Trace("CHARCLASS5");
    break;

case CHARCLASS6:
    Trace("CHARCLASS6");
    break;

case ENDCOMMENTCHARS1:
    Trace("ENDCOMMENTCHARS1");
    break;

case ENDCOMMENTCHARS2:
    Trace("ENDCOMMENTCHARS2");
    break;

case IDCHARS1:
    Trace("IDCHARS1");
    break;

case IDCHARS2:
    Trace("IDCHARS2");
    break;

case OPCHARS1:
    Trace("OPCHARS1");
    break;

case OPCHARS2:
    Trace("OPCHARS2");
    break;

case EXTENSIONCHAR1:
    Trace("EXTENSIONCHAR1");
    ProcessExtensionChar();
    break;
    
case SINGCHARS1:
    Trace("SINGCHARS1");
    break;

case SINGCHARS2:
    Trace("SINGCHARS2");
    break;

case WHITECHARS1:
    Trace("WHITECHARS1");
    break;

case WHITECHARS2:
    Trace("WHITECHARS2");
    break;

case ENDCOMMENTCHAR1:
    Trace("ENDCOMMENTCHAR1");
    ProcessEndCommentChar();
    break;
    
case IDCHAR1:
    Trace("IDCHAR1");
    ProcessSingleChar(IDCHAR);
    break;

case OPCHAR1:
    Trace("OPCHAR1");
    ProcessSingleChar(OPCHAR);
    break;

case SINGCHAR1:
    Trace("SINGCHAR1");
    ProcessSingleChar(SINGLECHAR);
    break;

case WHITECHAR1:
    Trace("CHAR1");
    ProcessSingleChar(WHITECHAR);
    break;

case TOKENCLASS1:
    Trace("TOKENCLASS1");
    break;

case TOKENCLASS2:
    Trace("TOKENCLASS2");
    break;

case TOKENCLASS3:
    Trace("TOKENCLASS3");
    break;

case TOKENCLASS4:
    Trace("TOKENCLASS4");
    break;

case TOKENCLASS5:
    Trace("TOKENCLASS5");
    break;

case TOKENCLASS6:
    Trace("TOKENCLASS6");
    break;

case TOKENCLASS7:
    Trace("TOKENCLASS7");
    break;

case TOKENCLASS8:
    Trace("TOKENCLASS8");
    break;

case TOKENCLASS9:
    Trace("TOKENCLASS9");
    break;

case TOKENCLASS10:
    Trace("TOKENCLASS10");
    break;

case TOKENCLASS11:
    Trace("TOKENCLASS11");
    break;

case TOKENCLASS12:
    Trace("TOKENCLASS12");
    break;

case TOKENCLASS13:
    Trace("TOKENCLASS13");
    break;

case QUANTIFIERSYMTOKS1:
    Trace("QUALIFERSYMTOKS1");
    break;

case QUANTIFIERSYMTOKS2:
    Trace("QUANTIFIERSYMTOKS2");
    break;

case LOGICALOPTOKS1:
    Trace("LOGICALOPTOKS1");
    break;

case LOGICALOPTOKS2:
    Trace("LOGICALOPTOKS2");
    break;

case EQOPTOKS1:
    Trace("EQOPTOKS1");
    break;

case EQOPTOKS2:
    Trace("EQOPTOKS2");
    break;

case EQUATIONSYMTOKS1:
    Trace("EQUATIONSYMTOKS1");
    break;

case EQUATIONSYMTOKS2:
    Trace("EQUATIONSYMTOKS2");
    break;

case EQSEPSYMTOKS1:
    Trace("EQSEPSYMTOKS1");
    break;

case EQSEPSYMTOKS2:
    Trace("EQSEPSYMTOKS2");
    break;

case SELECTSYMTOKS1:
    Trace("SELECTSYMTOKS1");
    break;

case SELECTSYMTOKS2:
    Trace("SELECTSYMTOKS2");
    break;

case OPENSYMTOKS1:
    Trace("OPENSYMTOKS1");
    break;

case OPENSYMTOKS2:
    Trace("OPENSYMTOKS2");
    break;

case SEPSYMTOKS1:
    Trace("SEPSYMTOKS1");
    break;

case SEPSYMTOKS2:
    Trace("SEPSYMTOKS2");
    break;

case CLOSESYMTOKS1:
    Trace("CLOSESYMTOKS1");
    break;

case CLOSESYMTOKS2:
    Trace("CLOSESYMTOKS2");
    break;

case SIMPLEIDTOKS1:
    Trace("SIMPLEIDTOKS1");
    break;

case SIMPLEIDTOKS2:
    Trace("SIMPLEIDTOKS2");
    break;

case MAPSYMTOKS1:
    Trace("MAPSYMTOKS1");
    break;

case MAPSYMTOKS2:
    Trace("MAPSYMTOKS2");
    break;

case MARKERSYMTOKS1:
    Trace("MARKERSYMTOKS1");
    break;

case MARKERSYMTOKS2:
    Trace("MARKERSYMTOKS2");
    break;

case COMMENTSYMTOKS1:
    Trace("COMMENTSYMTOKS1");
    break;

case COMMENTSYMTOKS2:
    Trace("COMMENTSYMTOKS2");
    break;

case QUANTIFIERSYMTOK1:
    Trace("QUANTIFERSYMTOK1");
    ProcessToken(QUANTIFIERSYM);
    break;

case LOGICALOPTOK1:
    Trace("LOGICALOPTOK1");
    ProcessToken(LOGICALOP);
    break;

case EQOPTOK1:
    Trace("EQOPTOK1");
    ProcessToken(EQOP);
    break;

case EQUATIONSYMTOK1:
    Trace("EQUATIONSYMTOK1");
    ProcessToken(EQUATIONSYM);
    break;

case EQSEPSYMTOK1:
    Trace("EQSEPSYMTOK1");
    ProcessToken(EQSEPSYM);
    break;

case SELECTSYMTOK1:
    Trace("SELECTSYMTOK1");
    ProcessToken(SELECTSYM);
    break;

case OPENSYMTOK1:
    Trace("OPENSYMTOK1");
    ProcessToken(OPENSYM);
    break;

case SEPSYMTOK1:
    Trace("SEPSYMTOK1");
    ProcessToken(SEPSYM);
    break;

case CLOSESYMTOK1:
    Trace("CLOSESYMTOK1");
    ProcessToken(CLOSESYM);
    break;

case SIMPLEIDTOK1:
    Trace("SIMPLEIDTOK1");
    ProcessToken(SIMPLEID);
    break;

case MAPSYMTOK1:
    Trace("MAPSYMTOK1");
    ProcessToken(MAPSYM);
    break;

case MARKERSYMTOK1:
    Trace("MARKERSYMTOK1");
    ProcessToken(MARKERSYM);
    break;

case COMMENTSYMTOK1:
    Trace("COMMENTSYMTOK1");
    ProcessToken(COMMENTSYM);
    break;

case SYNCLASS1:
    Trace("SYNCLASS1");
    ProcessSynonym();
    break;

case OLDTOKEN1:
    Trace("OLDTOKEN1");
    break;

case NEWTOKEN1:
    Trace("NEWTOKEN1");
    break;

default:
    error_programFailure("InitReduce()");
    break;

}							/* end switch	    */
}							/* end InitReduce() */



/* Reset the first character of the predefined extensionChar keywords when  */
/* the extensionChar changes.  e.g. "extensionChar @" changes "\forall" to  */
/* "@forall".								    */

static void
UpdateXCharKeywords(charCode xChar) {
    char    *str;

    str = token_text(ForallToken);
    *str = xChar;

    str = token_text(AndToken);
    *str = xChar;

    str = token_text(OrToken);
    *str = xChar;

    str = token_text(ImpliesToken);
    *str = xChar;

    str = token_text(IffToken);
    *str = xChar;

    str = token_text(EqToken);
    *str = xChar;

    str = token_text(NeqToken);
    *str = xChar;

    str = token_text(EqualsToken);
    *str = xChar;

    str = token_text(EqsepToken);
    *str = xChar;

    str = token_text(SelectToken);
    *str = xChar;

    str = token_text(OpenToken);
    *str = xChar;

    str = token_text(SepToken);
    *str = xChar;

    str = token_text(CloseToken);
    *str = xChar;

    str = token_text(SimpleidToken);
    *str = xChar;

    str = token_text(ArrowToken);
    *str = xChar;

    str = token_text(MarkerToken);
    *str = xChar;

    str = token_text(CommentToken);
    *str = xChar;

}

/* Different from ProcessCharClass because only allow one extension	    */
/* character. Therefore, the present extension character must be set to a   */
/* singleChar.								    */

static void
ProcessExtensionChar(void) {
    LSLToken stackToken;
    
    stackToken = TopPopShiftStack();
    if (!defineSingleChar[*token_text(stackToken)]
    &&  ScanCharClass(*token_text(stackToken)) == SINGLECHAR) {
	/* Is a single character that has not been defined before.	    */
	/* Can only have one extension char.  Release old one. */
	SetCharClass(*token_text(stackToken), EXTENSIONCHAR);
	SetCharClass(currentExtensionChar, SINGLECHAR);
	currentExtensionChar = *token_text(stackToken);
	UpdateXCharKeywords(currentExtensionChar);
    }
    else {
	/* Already redefined.  Don't allow to be redefined. */
	UserError(stackToken, "character is already defined, cannot redefine");
    }

}

/* Different from ProcessSingleChar because allow any characters to be	    */
/* endCommentChar and also set a different part of the scanner structure.   */

static void
ProcessEndCommentChar(void) {
    LSLToken stackToken;

    stackToken = TopPopShiftStack();
    if (IsEndComment(*token_text(stackToken))) {
	UserError(stackToken,
		  "already defined as a endCommentChar, cannot redefine");
    }
    else {
	SetEndCommentChar(*token_text(stackToken), TRUE);
    }
}

static void
ProcessSingleChar(charCode code) {
    LSLToken stackToken;
    
    stackToken = TopPopShiftStack();
    if (!defineSingleChar[*token_text(stackToken)]
    &&  ScanCharClass(*token_text(stackToken)) == SINGLECHAR) {
	/* Is a single character that has not been defined before.	    */
	/* It's OK to redefine once. */
	SetCharClass(*token_text(stackToken), code);
	/* OK to mark as a defined singleChar even if not.  Only check	    */
	/* defineSingleChar[] if defining a singleChar.			    */
	defineSingleChar[*token_text(stackToken)] = TRUE;
    }
    else {
	UserError(stackToken, "character is already defined, cannot redefine");
    }
}

static void
ProcessToken(LSLTokenCode code) {
    LSLToken stackToken;

    stackToken = TopPopShiftStack();
    if (IsSyn(token_textSym(stackToken))) {
	UserError(stackToken, "already defined as a synonym, cannot redefine");
    }

    /* Get the token from the token table, so can check if the token    */
    /* was updated by a previous token.				    */
    if (DefinedPart(GetToken(token_textSym(stackToken)))) {
	UserError(stackToken, "already defined, cannot redefine");
    }
    /* Set token code. Set defined flag to TRUE. */
    UpdateToken(code, token_textSym(stackToken), TRUE);
}


static void
ProcessSynonym(void) {
    LSLToken newToken;
    LSLToken oldToken;

    newToken = TopPopShiftStack();
    oldToken = TopPopShiftStack();

    if (WasSyn(newToken)) {
	/* The token has a synonym.  This means that the synonym was in the */
	/* init file, so complain about redefining as a synonym again	    */
	UserError(newToken, "newToken already is a synonym, cannot redefine");
    }

    if (HasSynPart(newToken)) {
	/* newToken already has a synonym defined for it.  Do not allow	    */
	/* synonyms to be chained.					    */
	UserError(newToken,
		  "newToken already has a synonym, cannot chain synonyms");
    }

    if (DefinedPart(newToken)) {
	UserError(newToken, "newToken already defined, cannot redefine");
    }

    AddSyn(token_textSym(newToken), token_textSym(oldToken));
}


/*
 * Utilities, in alphabetical order
 */

static void
UserError(LSLToken t, char *msg) {
    error_point(source_thisLine(LSLScanSource()), token_col(t));
    error_report(string_paste(msg, " in the init file"));
    /* Ignore line for now. */
}


/*
 *  Required initialization and cleanup routines
 */

void
LSLProcessInitFileInit(void) {
    int i;

    /* Insert the init file keywords into the token table as undefined	    */
    /* SIMPLEIDs.  They are defined as simpleIds since they must be treated */
    /* that way if they do not appear as the first token on a line, and	    */
    /* they must be treated that way for the actual LSL parsing. Save the   */
    /* tokens so can recognize as init file keywords when necessary.	    */

    endCommentCharToken = InsertToken(SIMPLEID, String2Symbol("endCommentChar"),
				      0, FALSE);
    idCharToken = InsertToken(SIMPLEID, String2Symbol("idChar"),
			      0, FALSE);
    opCharToken = InsertToken(SIMPLEID, String2Symbol("opChar"),
			      0, FALSE);
    extensionCharToken = InsertToken(SIMPLEID, String2Symbol("extensionChar"),
				     0, FALSE);
    singleCharToken = InsertToken(SIMPLEID, String2Symbol("singleChar"),
				  0, FALSE);
    whiteCharToken = InsertToken(SIMPLEID, String2Symbol("whiteChar"),
				 0, FALSE);

    quantifierSymToken = InsertToken(SIMPLEID, String2Symbol("quantifierSym"),
				    0, FALSE);
    logicalOpToken = InsertToken(SIMPLEID, String2Symbol("logicalOp"),
                                 0, FALSE);
    eqOpToken = InsertToken(SIMPLEID, String2Symbol("eqOp"),
                            0, FALSE);
    equationSymToken = InsertToken(SIMPLEID, String2Symbol("equationSym"),
                                   0, FALSE);
    eqSepSymToken = InsertToken(SIMPLEID, String2Symbol("eqSepSym"),
                                0, FALSE);
    selectSymToken = InsertToken(SIMPLEID, String2Symbol("selectSym"),
                                 0, FALSE);
    openSymToken = InsertToken(SIMPLEID, String2Symbol("openSym"),
                               0, FALSE);
    sepSymToken = InsertToken(SIMPLEID, String2Symbol("sepSym"),
                              0, FALSE);
    closeSymToken = InsertToken(SIMPLEID, String2Symbol("closeSym"),
                                0, FALSE);
    simpleIdToken = InsertToken(SIMPLEID, String2Symbol("simpleId"),
                                0, FALSE);
    mapSymToken = InsertToken(SIMPLEID, String2Symbol("mapSym"),
                              0, FALSE);
    markerSymToken = InsertToken(SIMPLEID, String2Symbol("markerSym"),
                                 0, FALSE);
    commentSymToken = InsertToken(SIMPLEID, String2Symbol("commentSym"),
                                  0, FALSE);

    synonymToken = InsertToken(SIMPLEID, String2Symbol("synonym"),
                               0, FALSE);

    /* Initialize defineSingleChar array to all FALSE to signal that no	    */
    /* characters have been redefined as singleChar. */
    for (i=0;i<=LASTCHAR;i++) {
        defineSingleChar[i] = FALSE;
    }

    /* Record the current extension character so can redefine back to	    */
    /* singleChar if a new extension character is redefined.		    */
    currentExtensionChar = EXTENDCHARDEF;    

    /* Init file processing needs to have EOL reported. */
    EolToken(TRUE);
}

void
LSLProcessInitFileReset(void) {
    nextToken = Scan();					/* first token	    */
}

void
LSLProcessInitFileCleanup(void) {
}


