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
**      FILENAME:   token.h
**
**	PURPOSE:    LSL token abstraction
**
**	A token is a struct that includes a unique code (an enum) and a
**	of means of discovering the textual spelling of the token.
**
**	Some tokens are identified uniquely by their token code (they
**	are reserved symbols and words below).  Others are in a class
**	(such as identifier) and therefore can be distinguished only by
**	comparing their texts as well.  The implementation provides a
**	particularly efficient way of comparing tokens
**	    if ( token_equal(tok1, tok2) == TRUE ) ... 
**	which which uses a string abstraction to manage token texts.
**	Texts are registered in the string abstraction by
**	    str = LSLString2Symbol("abc");
**	and later recovered by
**	    txt = LSLSymbol2String(str);
**	The purpose is to compact the (char *) value to a small unsigned int.
**	The assumption is that there are never more than 64K different
**	tokens in any one LSL program.
**
**	USAGE:	# include "general.h"
**		# include "string2sym.h"
**		# include "token.h"
**	
**  AUTHORS:
**
**	Gary Feldman, Technical Languages and Environments, DECspec project
**	Steve Garland, MIT
**	Bill McKeeman, Technical Languages and Environments, RapidCase project
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  CREATION DATE:
**
**	20 April 1990
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	WM	90.04.20	Original
**  X0.1-2	SJG	90.05.18	Add eol token
**  X0.1-3	GAF	90.07.23	Rename __TOKEN to MARKERSYM
**					Add SELECTSYM
**  X0.1-4	JPW	90.08.15	Use 2.3 token names for enums
**  X0.1-5	JPW	90.08.23	Add hasSyn field to LSLToken
**  X0.1-6	SJG	90.10.03	Split token.h from scan.h
**  					Add fileName, line to struct
**					Add TrueToken, ..., NotToken
**  X0.1-7	GAF	90.12.21	Add NOTTOKEN
**  X0.1-8	SJG	91.12.20	Add IfToken
**  X0.1-9	SJG	92.06.14	Add COMPOSESYM
**  X0.1-10	SJG	93.11.09	Add QUANTIFIERSYM1
**					Add ExistsToken, AllToken
**  X0.1-11	SJG	93.11.26	Add IffToken, priority
**  X0.1-12	SJG	94.04.20	Add freelyTOKEN
**  X0.1-13	SJG	94.05.20	Remove COMPOSESYM
**					Add sortTOKEN, LBRACK, RBRACK
**  [@modification history entry@]...
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

#define TOPLOGICALPRIORITY	iffPRIORITY
#define ifPRIORITY		9
#define iffPRIORITY		8
#define impliesPRIORITY		7
#define andorPRIORITY		6
#define eqPRIORITY		5
#define somefixPRIORITY		4
#define mixfixPRIORITY		3
#define selectPRIORITY		2
#define simpleIdPRIORITY	1
#define LOWLOGICALPRIORITY	eqPRIORITY

/*
**
**  TYPEDEFS
**
*/

/* LSLTokenCode -- token codes						    */
typedef enum {

    NOTTOKEN,						/* uninitialized    */
    /* symbol classes */                                /*   examples	    */

    QUANTIFIERSYM,					/* \forall          */
    QUANTIFIERSYM1,					/* \A, \E	    */
    LOGICALOP,						/* \and \or \implies*/
    EQOP,						/* \eq \neq	    */
    EQUATIONSYM,					/* \equals	    */
    EQSEPSYM,						/* \eqsep	    */
    SELECTSYM,						/* \select	    */
    OPENSYM,						/* \(        	    */
    SEPSYM,						/* \,        	    */
    CLOSESYM,						/* \)        	    */
    SIMPLEID,						/* \: id-char+	    */
    MAPSYM,						/* \arrow	    */
    MARKERSYM,						/* \marker	    */
    COMMENTSYM,						/* \comment	    */

    SIMPLEOP,						/* opSym - reserved */

    /* reserved symbols */
    COMMA,						/* ,		    */
    COLON,						/* :		    */
    LPAR,						/* (		    */
    RPAR,						/* )		    */
    LBRACK,						/* [		    */
    RBRACK,						/* ]		    */

    WHITESPACE,
    EOFTOKEN,
    EOLTOKEN,

    /* reserved words */
    assertsTOKEN,					/* asserts	    */
    assumesTOKEN,					/* assumes          */
    byTOKEN,						/* by               */
    convertsTOKEN,					/* converts         */
    elseTOKEN,						/* else             */
    enumerationTOKEN,					/* enumeration      */
    equationsTOKEN,					/* equations        */
    exemptingTOKEN,					/* exempting        */
    forTOKEN,						/* for              */
    freelyTOKEN,					/* freely	    */
    generatedTOKEN,					/* generated        */
    ifTOKEN,						/* if               */
    impliesTOKEN,					/* implies	    */
    includesTOKEN,					/* includes         */
    introducesTOKEN,					/* introduces       */
    ofTOKEN,						/* of               */
    partitionedTOKEN,					/* partitioned      */
    sortTOKEN,						/* sort		    */
    thenTOKEN,						/* then             */
    traitTOKEN,						/* trait            */
    tupleTOKEN,						/* tuple            */
    unionTOKEN,						/* union            */

    BADTOKEN						/* undefined	    */
} LSLTokenCode;

typedef struct {					/*  --private--	    */
    unsigned int    code: 8;				/* LSLTokenCode	    */
    unsigned int    col:  8;				/* column number    */
    unsigned int    text: 16;				/* string handle    */
    unsigned int    fileName: 16;			/* source handle    */
    unsigned int    line: 16;				/* line number	    */
    unsigned int    rawText: 16;			/* original text    */
    unsigned int    priority: 8;			/* parsing priority */
    unsigned int    defined: 1;				/* token predefined */
    unsigned int    hasSyn: 1;				/* synonym exists   */
    unsigned int    unused: 6;
} LSLToken;


/*
**
**  FUNCTION PROTOTYPES (mostly redefined to macros)
**
*/

extern LSLToken	     	token_null(void);

extern LSLTokenCode	token_code(LSLToken tok);
# define		token_code(tok)				    	    \
			    ((tok).code)

extern int		token_col(LSLToken tok);
# define		token_col(tok)					    \
			    ((tok).col)

extern int		token_priority(LSLToken tok);
# define		token_priority(tok)				    \
			    ((tok).priority)

extern int		token_defined(LSLToken tok);
# define		token_defined(tok)				    \
			    ((tok).defined)

extern int		token_fileName(LSLToken tok);
# define		token_fileName(tok)				    \
			    (LSLSymbol2String((tok).fileName))

extern int		token_hasSyn(LSLToken tok);
# define		token_hasSyn(tok)				    \
			    ((tok).hasSyn)

extern bool		token_isChar(LSLToken tok);
# define		token_isChar(tok)				    \
			    ( *(LSLSymbol2String(tok.text) + 1) == '\0' )

extern int		token_line(LSLToken tok);
# define		token_line(tok)					    \
			    ((tok).line)

extern char	       *token_rawText(LSLToken tok);
# define		token_rawText(tok)				    \
			    (LSLSymbol2String((tok).rawText == 0            \
			                      ? (tok).text : (tok).rawText)
extern char	       *token_rawTextSym(LSLToken tok);
# define		token_rawTextSym(tok)				    \
			    ((tok).rawText == 0 ? (tok).text : (tok).rawText)

extern char	       *token_text(LSLToken tok);
# define		token_text(tok)					    \
			    (LSLSymbol2String((tok).text))

extern char	       *token_textSym(LSLToken tok);
# define		token_textSym(tok)				    \
			    ((tok).text)

extern bool	        token_wasSyn(LSLToken tok);
# define		token_wasSyn(tok)      				    \
			    ((tok).rawText != 0)


extern bool		token_similar(LSLToken tok1, LSLToken tok2);
# define		token_similar(tok1, tok2)			    \
			    ((tok1).code == (tok2).code &&		    \
			     (tok1).text == (tok2).text			    \
			    )


extern void	        token_setCode(LSLToken tok, LSLTokenCode cod);
# define		token_setCode(tok, cod)			    \
			    ((tok).code = (cod))

extern void		token_setCol(LSLToken tok, int col);
#define			token_setCol(tok, coln)				    \
			    ((tok).col = (coln))

extern void		token_setDefined(LSLToken tok, bool def);
# define		token_setDefined(tok, def)			    \
			    ((tok).defined = (def))

extern void		token_setPriority(LSLToken tok, int p);
# define		token_setPriority(tok, p)			    \
			    ((tok).priority = (p))

extern void		token_setFileName(LSLToken tok, char *fn);
#define			token_setFileName(tok, fn)			    \
			    ((tok).fileName = LSLString2Symbol(fn))

extern void		token_setHasSyn(LSLToken tok, bool def);
# define		token_setHasSyn(tok, def)			    \
			    ((tok).hasSyn = (def))

extern void		token_setLine(LSLToken tok, int lineNo);
#define			token_setLine(tok, lineNo)			    \
			    ((tok).line = (lineNo))

extern void	        token_setRawTextSym(LSLToken tok, LSLSymbol sym);
# define		token_setRawTextSym(tok, sym)			    \
			    ((tok).rawText = (sym))

extern void	        token_setTextSym(LSLToken tok, LSLSymbol sym);
# define		token_setTextSym(tok, sym)			    \
			    ((tok).text = (sym))


/*
**
**  EXTERNAL VARIABLES
**
*/

extern LSLToken	ForallToken;
extern LSLToken	AllToken;
extern LSLToken	ExistsToken;
extern LSLToken TrueToken;
extern LSLToken FalseToken;
extern LSLToken NotToken;
extern LSLToken AndToken;
extern LSLToken OrToken;
extern LSLToken ImpliesToken;
extern LSLToken IffToken;
extern LSLToken EqToken;
extern LSLToken NeqToken;
extern LSLToken EqualsToken;
extern LSLToken EqsepToken;
extern LSLToken SelectToken;
extern LSLToken OpenToken;
extern LSLToken SepToken;
extern LSLToken CloseToken;
extern LSLToken SimpleidToken;
extern LSLToken ArrowToken;
extern LSLToken MarkerToken;
extern LSLToken CommentToken;
extern LSLToken IfToken;
extern LSLToken ComposeToken;
