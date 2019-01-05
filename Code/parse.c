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
**      FILENAME:   parse.c
**
**	PURPOSE:    Parser for Larch Shared Language
**	
**	The parser is a standard recursive descent algorithm.
**	The grammar is LALR(1) according to yacc.
**
**	DESIGN:
**	
**	There are two places where long lookahead had to be employed to match
**	the power of LALR(1): over an actual in a renaming and over a secondary
**	in a simpleOpTerm.  The trick is to parse some common left part of the
**	several rules for renamings and simpleOpTerms, and then, after the 
**	correct recursion is known, call it, but with the recursion `knowing' 
**	that some initial actual or secondary has already been processed.  See
**	Renaming(), SimpleOpTerm(), and related functions.  The Replace() and
**	PrefixOpTerm() functions, which are used by Renaming() and
**	SimpleOpTerm(), take a boolean parameter to indicate whether or not 
**	the lookahead has been done.
**	
**	The only output is the shift/reduce sequence, reported via
**	    LSLGenShift()
**	    LSLGenReduce()
**	All tokens are reported to LSLGenShift(), which pushes them on a
**	stack.  The reduction actions in LSLGenReduce() must take this into 
**	account by popping the appropriate number of tokens off the stack.
**
**	Productions that use empty alternatives for lists that are completely
**	defined by the production, such as propParts and conversion, always
**	Reduce() the empty alternative first.  Productions, such as middle and
**	domain, that don't completely specify a list do not Reduce() the empty
**	alternative unless the other alternatives are absent.
**
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at Digital -- 90.03.06 -- Original
**      {0} McKeeman at Digital -- 90.05.11 -- Working
**	{1} Feldman  at Digital	-- 90.07.19 -- Convert to 11 Jul 90 grammar
**	{2} Feldman  at Digital	-- 90.08.10 -- Correct typo of eqPart in Props
**	{3} Feldman  at Digital	-- 90.08.14 -- Correct syntax of TraitContexts
**	{4} Feldman  at Digital	-- 90.08.14 -- Correct syntax of NameList
**	{5} Feldman  at Digital	-- 90.08.14 -- Correct syntax of Union
**	{6} Feldman  at Digital	-- 90.08.14 -- Add missing break's
**	{7} Garland  at MIT	-- 90.09.06 -- Updated LSLParseInit
**	{8} Garland  at MIT     -- 90.09.23 -- Scan simpleId after SELECTSYM
**      {9} Garland  at MIT	-- 90.09.23 -- Fix props, conseqProps
**     {10} Garland  at MIT     -- 90.09.24 -- Check that multiple ops are same
**						 in LogicalTerm, InfixOpPart
**     {11} Garland  at MIT     -- 90.10.10 -- Shift all tokens
**     {12} Garland  at MIT     -- 90.11.06 -- Allow null eqPart in conseqProps
**     {13} Garland  at MIT     -- 92.01.13 -- Allow eqSepSym at end of eqSeq,
**					       fix (traitRef) in conseqProps
**     {14} Garland  at MIT     -- 92.01.24 -- Add LSLParseReplace, fix {14}
**     {15} Garland  at MIT     -- 92.02.19 -- Reorder #include "error.h"
**     {16} Garland  at MIT     -- 92.06.14 -- Composite sort notation A[B]
**     {17} Garland  at MIT     -- 92.06.14 -- Fix composite sort in shorthand
**     {18} Garland  at MIT     -- 93.11.09 -- Quantified formulas
**     {19} Garland  at MIT     -- 94.04.20 -- Generated freely
**     {20} Garland  at MIT	-- 94.05.20 -- Composite sort notation A[B, C]
**     {21} Garland  at MIT	-- 94.05.27 -- Add "," in opDcls, redo formals
**					       Fix composite sorts in opDcls
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include		"general.h"
# include		"osdfiles.h"
# include		"string2sym.h"
# include		"token.h"
# include		"error.h"
# include		"source.h"
# include		"scan.h"
# include		"scanline.h"
# include		"parse.h"
# include		"shift.h"
# include		"reduce.h"
# include		"list.h"
# include		"tokentable.h"
# include		"abstract.h"

/*
**
**  MACRO DEFINITIONS
**
*/

# define Scan			LSLScanNextToken
# define LookAhead		LSLScanLookAhead

# define Shift			LSLGenShift
# define Reduce			LSLGenReduce

# define CodePart		token_code

# define CompilerFailure 	error_programFailure


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

extern void		LSLParse();
extern bool		LSLParseReplace();

static void		Trait();
static void		FormalList();
static void		Formal();
static void		Name();
static void		OpForm();
static void		AnyOp();
static void		Middle();
static void		PlaceList();
static void		Separator();
static void		Signature();
static void		Domain();
static void		SortList();
static void		Range();
static void		Sort();
static void		TraitBody();
static void		TraitContexts();
static void		TraitContext();
static void		Shorthand();
static void		Enumeration();
static void		ElementIdList();
static void		Tuple();
static void		Union();
static void		FieldList();
static void		Fields();
static void		FieldIdList();
static void		External();
static void		TraitRefList();
static void		TraitRef();
static void		TraitIdList();
static void		Renaming();
static void		Actual();
static void		ActualList();
static void		NameList();
static void		ReplaceList();
static void		Replace(bool actualParsed);
static void		SimpleTrait();
static void		OpParts();
static void		OpDcls();
static void		OpDcl();
static void		PropParts();
static void		Props();
static void		GenPartProps();
static void		GenPartProp();
static void		EqPart();
static void		OperatorList();
static void		Operator();
static void		QuantifiedEqSeqs();
static void		EqSeq();
static void		EqSeqHead();
static void		Quantifier();
static void		VarDcl();
static void		VarIdList();
static void		Equation();
static void		Term();
static void		LogicalTerm();
static void		SimpleOpTerm();
static void		PrefixOpTerm();
static void		QuantifiedVar();
static void		PostfixOps();
static void		InfixOpPart();
static void		Secondary();
static void		Bracketed();
static void		Matched();
static void		Args();
static void		Primary();
static void		TermList();
static void		Consequences();
static void		ConseqProps();
static void		Conversions();
static void		Conversion();
static void		Exemption();
static void		TraitId();
static void		SortId();
static void		VarId();
static void		FieldId();
static void		ElementId();
static void		SortOrOpId();

static void		Accept();
static void		UserError(char *msg);

extern void		LSLParseInit();
extern void		LSLParseReset();
extern void		LSLParseCleanup();

/*
**
**  STATIC VARIABLES
**
*/

static LSLToken		nextToken;

/*
**
**  FUNCTION DEFINITIONS
**
*/

void
LSLParse(void) {
    Trait();
}


bool
LSLParseReplace(void) {
    if (CodePart(nextToken) == EOFTOKEN) return FALSE;
    Replace(FALSE);
    return TRUE;
}


/*
**
**  Parsing functions, in the same order as the grammar file lsl.cfg.  This is
**  top-down order, as much as possible.
**
*/

static void
Trait(void) {
    TraitId();
    switch ( CodePart(nextToken) ) {
    case COLON:
	Shift(nextToken);				/* shift `:'	    */
	nextToken = Scan();
	Accept(traitTOKEN, "expecting `trait' in trait header");
	TraitBody();
	Reduce(TRAIT1);
	break;
    case LPAR:
	Shift(nextToken);				/* shift `(')	    */
	nextToken = Scan();
	FormalList();
	Accept(RPAR, "expecting `)' terminating formal parameters");
	Accept(COLON, "expecting `:' after trait formal parameters");
	Accept(traitTOKEN, "expecting `trait' in trait header");
	TraitBody();
	Reduce(TRAIT2);
	break;
    default:
	UserError("expecting `: trait' or `(' to start formal parameters");
	break;
    }
    if (CodePart(nextToken) != EOFTOKEN) {
        UserError("not expecting tokens after trait definition");
    }
}

static void
FormalList(void) {
    Formal();
    Reduce(FORMALLIST1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);			        /* shift `,'	    */
	nextToken = Scan();
	Formal();
	Reduce(FORMALLIST2);
    }
}

static void
Formal(void) {
    if ( CodePart(nextToken) == SIMPLEID ) {
	if ( CodePart(LookAhead()) == LBRACK ) {
	    Sort();
	    Reduce(FORMAL3);
	} else if ( CodePart(LookAhead()) == COLON ) {
	    Operator();
	    Reduce(FORMAL1);
	} else {
	    SortOrOpId();
	    Reduce(FORMAL4);
	}
    } else {
	Operator();
	Reduce(FORMAL2);
    }
}


static void
Name(void) {
    switch ( CodePart(nextToken) )    {
    case SIMPLEID:
	Shift(nextToken);			/* shift simpleId	    */
	nextToken = Scan();
	Reduce(NAME1);
	break;

    case ifTOKEN:
    case MARKERSYM:
    case SIMPLEOP:
    case LOGICALOP:
    case EQOP:
    case OPENSYM:
    case LBRACK:
    case SELECTSYM: 
	OpForm();
	Reduce(NAME2);
	break;

    default:
	UserError("expecting operator identifier or operator form");
	break;

    }
}

static void
OpForm(void) {
    switch (CodePart(nextToken)) {
    case ifTOKEN:
	Shift(nextToken);		/* shift `if'   */
	nextToken = Scan();
	Accept(MARKERSYM, "expecting __ after IF");
	Accept(thenTOKEN, "expecting THEN after IF __");
	Accept(MARKERSYM, "expecting __ after IF __ THEN");
	Accept(elseTOKEN, "expecting ELSE after IF __ THEN __");
	Accept(MARKERSYM, "expecting __ after IF __ THEN __ ELSE");
	Reduce(OPFORM1);			    /* IF __ THEN __ ELSE __ */
	break;

    case MARKERSYM:
	Shift(nextToken);			    /* shift `__'	    */
	nextToken = Scan();
	switch (CodePart(nextToken)) {
	case LBRACK:
	case OPENSYM:
	    Shift(nextToken);		    	    /* shift open sym	    */
	    nextToken = Scan();
	    Middle();
	    if (CodePart(nextToken) == CLOSESYM 
		|| CodePart(nextToken) == RBRACK) {
		Shift(nextToken);		    /* shift close sym	    */
		nextToken = Scan();
	    } else {
		UserError("expecting closeSym in operator form");
	    }
	    if (CodePart(nextToken) == MARKERSYM) {
		Shift(nextToken);		    /* shift `__'	    */
		nextToken = Scan();
		Reduce(OPFORM9);		    /* __(middle)__	    */
	    } else {
		Reduce(OPFORM7);		    /* __(middle)	    */
	    }
	    break;

	case SELECTSYM:
	    Shift(nextToken);			    /* shift `.'	    */
	    nextToken = Scan();
	    if ( CodePart(nextToken) == SIMPLEID) {
		 Shift(nextToken);		    /* shift simpleId	    */
		 nextToken = Scan();
		 Reduce(OPFORM11);		    /* __ . simpleId	    */
	    } else {
		 UserError ("expecting identifier after selectSym");
	    }
	    break;

	default:
	    AnyOp();
	    if ( CodePart(nextToken) == MARKERSYM) {
		Shift(nextToken);		    /* shift `__'	    */
		nextToken = Scan();
		Reduce(OPFORM5);		    /* __ anyOp __	    */
	    } else {
		Reduce(OPFORM3);		    /* __ anyOp		    */
	    }
	    break;
	}
	break;

    case LBRACK:
    case OPENSYM:
	Shift(nextToken);				/* shift open sym   */
	nextToken = Scan();
	Middle();
	if (CodePart(nextToken) == CLOSESYM
	    || CodePart(nextToken) == RBRACK) {
	    Shift(nextToken);				/* shift close sym  */
	    nextToken = Scan();
	} else {
	    UserError("expecting closeSym in operator form");
	}
	if (CodePart(nextToken) == MARKERSYM) {
	    Shift(nextToken);		    		/* shift `__'	    */
	    nextToken = Scan();
	    Reduce(OPFORM8);
	} else {
	    Reduce(OPFORM6);
	}
	break;

    case SELECTSYM:
	Shift(nextToken);				/* shift selectsym  */
	nextToken = Scan();
	if ( CodePart(nextToken) == SIMPLEID) {
	     Shift(nextToken);		    		/* shift simpleId   */
	     nextToken = Scan();
	     Reduce(OPFORM10);		    		/* . simpleId       */
	} else {
	     UserError ("expecting identifier after selectSym");
	}
	break;
	
    default:
	AnyOp();
	if ( CodePart(nextToken) == MARKERSYM) {
	    Shift(nextToken);		    		/* shift `__'	    */
	    nextToken = Scan();
	    Reduce(OPFORM4);
	} else {
	    Reduce(OPFORM2);
	}
	break;
    }
}

static void
AnyOp(void) {
    switch (CodePart(nextToken)) {

    case SIMPLEOP:
	Shift(nextToken);		    /* shift simpleOp		    */
	nextToken = Scan();
	Reduce(ANYOP1);
	break;

    case LOGICALOP:
	Shift(nextToken);		    /* shift logical symbol	   */
	nextToken = Scan();
	Reduce(ANYOP2);
	break;

    case EQOP:
	Shift(nextToken);		    /* shift eq symbol		    */
	nextToken = Scan();
	Reduce(ANYOP3);
	break;

    default:
	UserError("expecting operator");
	break;
    }    
}

static void
Middle(void) {
    switch (CodePart(nextToken)) {

    case MARKERSYM:
	PlaceList();
	Reduce(MIDDLE2);
	break;

    default:
	Reduce(MIDDLE1);				/*  empty	    */
	break;
    }
}

static void
PlaceList(void) {

    Accept(MARKERSYM, "expecting `__' in operator form");
    Reduce(PLACELIST1);

    while ( CodePart(nextToken) == COMMA
	||  CodePart(nextToken) == SEPSYM)  {
	Separator();
	Accept(MARKERSYM, 
	       "expecting `__' after separator or `,' in operator form");
	Reduce(PLACELIST2);
    }
}

static void
Separator(void) {
    switch (CodePart(nextToken)) {
    case SEPSYM:
	Shift(nextToken);	    /* shift separator			    */
	nextToken = Scan();
	Reduce(SEPARATOR1);	    /* sepSym				    */
	break;

    case COMMA:
	Shift(nextToken);	    /* shift `,'			    */
	nextToken = Scan();
	Reduce(SEPARATOR2);	    /* ,				    */
	break;

    default:
	UserError("expecting separator or comma");
	break;

    }
}

static void
Signature(void) {
    Domain();
    Accept(MAPSYM, "expecting mapping symbol");
    Range();
    Reduce(SIGNATURE1);
}

static void
Domain(void) {
    switch (CodePart(nextToken)) {
    case SIMPLEID:
	SortList();
	Reduce(DOMAIN2);
	break;
    default:
	Reduce(DOMAIN1);				/* empty	    */
	break;
    }
}

static void
SortList(void) {
    Sort();
    Reduce(SORTLIST1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	Sort();
	Reduce(SORTLIST2);
    }
}

static void
Range(void) {
    Sort();
    Reduce(RANGE1);
}

static void
Sort(void) {
    SortId();
    if (CodePart(nextToken) == LBRACK && CodePart(LookAhead()) == SIMPLEID) {
      Shift(nextToken);				/* shift `['	    */
      nextToken = Scan();
      SortList();
      Accept(RBRACK, "expecting `]' in composite sort");
      Reduce(SORT2);
    } else {
      Reduce(SORT1);
    }
  }

static void
TraitBody(void) {
    switch (CodePart(nextToken)) {

    case introducesTOKEN:				/* op-parts	*/
    case assertsTOKEN:					/* prop-parts	*/
    case impliesTOKEN:					/* empty	*/
    case EOFTOKEN:					/* empty	*/
	SimpleTrait();
	if (CodePart(nextToken) == impliesTOKEN) {
	    Consequences();
	    Reduce(TRAITBODY2);		    /* simpleTrait consequences	*/
	} else {
	    Reduce(TRAITBODY1);		    /* simpleTrait		*/
	}
	break;
 
   default:
	TraitContexts();
	SimpleTrait();
	if (CodePart(nextToken) == impliesTOKEN) {
	    Consequences();
	    Reduce(TRAITBODY4);	    /*	traitContexts simpleTrait consequences*/
	} else {
	    Reduce(TRAITBODY3);	    /*  traitContexts simpleTrait */
	}
	break;
    }
}

static void
TraitContexts(void) {

    TraitContext();
    Reduce(TRAITCONTEXTS1);
    while ( CodePart(nextToken) == SIMPLEID
	||  CodePart(nextToken) == assumesTOKEN
	||  CodePart(nextToken)	== includesTOKEN) {
    	TraitContext();
	Reduce(TRAITCONTEXTS2);
    }
}

static void
TraitContext(void) {

    switch (CodePart(nextToken)) {
    case SIMPLEID:
	Shorthand();
        Reduce(TRAITCONTEXT1);
	break;

    case includesTOKEN:
    case assumesTOKEN:
	External();
	Reduce(TRAITCONTEXT2);
	break;

    default:
	UserError("expecting shorthand or external");
	break;
    }
}

static void
Shorthand(void) {
    Sort();
    switch ( CodePart(nextToken) ) {
    case enumerationTOKEN:
	Enumeration();
	Reduce(SHORTHAND1);
	break;
    case tupleTOKEN:
	Tuple();
	Reduce(SHORTHAND2);
	break;
    case unionTOKEN:
	Union();
	Reduce(SHORTHAND3);
	break;
    default:
	nextToken = Scan();
	UserError("expecting `tuple', `union' or `enumeration' in shorthand");
	break;
    }
}

static void
Enumeration(void) {
    Accept(enumerationTOKEN, "expecting `enumeration'");
    Accept(ofTOKEN, "expecting `of' after `enumeration'");
    ElementIdList();
    Reduce(ENUMERATION1);
}

static void
ElementIdList(void) {
    ElementId();
    Reduce(ELEMENTIDLIST1);				/* first one	    */

    while ( CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	ElementId();
	Reduce(ELEMENTIDLIST2);				/* subsequent elems */
    }
}

static void
Tuple(void) {
    Accept(tupleTOKEN, "expecting `tuple'");
    Accept(ofTOKEN, "expecting `of' after `tuple'");
    FieldList();
    Reduce(TUPLE1);
}

static void
Union(void) {
    Accept(unionTOKEN, "expecting `union'");
    Accept(ofTOKEN, "expecting `of' after `union'");
    FieldList();
    Reduce(UNION1);
}

static void
FieldList(void) {
    Fields();
    Reduce(FIELDLIST1);

    while ( CodePart(nextToken) == COMMA ) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	Fields();
	Reduce(FIELDLIST2);
    }
}

static void
Fields(void) {
    FieldIdList();
    Accept(COLON, "expecting `:' after fields");
    Sort();
    Reduce(FIELDS1);
}

static void
FieldIdList(void) {
    FieldId();
    Reduce(FIELDIDLIST1);

    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	FieldId();
	Reduce(FIELDIDLIST2);
    }
}

static void
External(void) {
    switch ( CodePart(nextToken) ) {
    case includesTOKEN:
	Shift(nextToken);				/* shift `includes'  */
	nextToken = Scan();
	TraitRefList();
	Reduce(EXTERNAL1);
	break;
    case assumesTOKEN:
	Shift(nextToken);				/* shift `assumes'  */
	nextToken = Scan();
	TraitRefList();
	Reduce(EXTERNAL2);
	break;
    default:
	CompilerFailure("External()");
	break;
    }
}

static void
TraitRefList(void) {
    TraitRef();
    Reduce(TRAITREFLIST1);

    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	TraitRef();
	Reduce(TRAITREFLIST2);
    }
}

static void
TraitRef(void) {
    switch ( CodePart(nextToken) ) {

    case SIMPLEID:
	TraitId();
	if (CodePart(nextToken) == LPAR) {
	    Shift(nextToken);				/* shift `('	    */
	    nextToken = Scan();
	    Renaming();
	    Accept(RPAR, "expecting `)' after renaming");
	    Reduce(TRAITREF2);
	} else {
	    Reduce(TRAITREF1);
	}
	break;

    case LPAR:
	Shift(nextToken);				/* shift `('	    */
	nextToken = Scan();
	TraitIdList();
	Accept(RPAR, "expecting `)' after trait identifier list");
	if (CodePart(nextToken) == LPAR) {
	    Shift(nextToken);				/* shift `('	    */
	    nextToken = Scan();
	    Renaming();
	    Accept(RPAR, "expecting `)' after renaming");
	    Reduce(TRAITREF4);
	} else {
	    Reduce(TRAITREF3);
	}
	break;

    default:
	UserError("expecting trait identifier");
	break;
    }
}

static void
TraitIdList(void) {
    TraitId();
    Reduce(TRAITIDLIST1);				/* first one	    */

    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	TraitId();
	Reduce(TRAITIDLIST2);
    }
}

static void
Renaming(void) {
    Actual();						/* long lookahead   */
    switch (CodePart(nextToken)) {
    case forTOKEN:
	ReplaceList();
	Reduce(RENAMING1);
	break;
    default:
	ActualList();
	if (CodePart(nextToken) == RPAR) {
	    Reduce(RENAMING2);
	} else if (CodePart(nextToken) == forTOKEN) {
	    ReplaceList();				/* no , or op name  */
	    Reduce(RENAMING3);
	} else {
	    UserError("expecting `for' or `)' after actual list");
	}
	break;
    }
}

static void
Actual(void) {
    if ( CodePart(nextToken) == SIMPLEID ) {
	if ( CodePart(LookAhead()) == LBRACK ) {
	    Sort();
	    Reduce(ACTUAL2);
	} else {
	    SortOrOpId();
	    Reduce(ACTUAL3);
	}
    } else {
        Name();
	Reduce(ACTUAL1);
    }
}


/* NOTE:								    */
/*	Because of the long lookahead needing to pass over an actual,       */
/* 	ActualList() is called with an actual already parsed and positioned */
/*	on the `,' or at the end.					    */
static void
ActualList(void) {
    Reduce(ACTUALLIST1);				/* first actual	    */

    for (;;) {
	if (CodePart(nextToken) == COMMA) {
	    Shift(nextToken);				/* shift `,'	    */
	    nextToken = Scan();
	    Actual();
	    if (CodePart(nextToken) == forTOKEN) {
		break;					/* ready for for-list*/
            } else {
		Reduce(ACTUALLIST2);
	    }
	} else if (CodePart(nextToken) == RPAR) {
	    break;
	} else {
	    UserError("expecting `,' or end of actual list");
	    break;
        }
    }
}


/* NOTE: Because of the long lookahead needing to pass over an actual,      */
/*	 ReplaceList() is called with an actual already parsed and 	    */
/*	 positioned on the `for'.					    */

static void
ReplaceList(void) {
    Replace(TRUE);
    Reduce(REPLACELIST1);				/* first one	    */

    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	Replace(FALSE);
	Reduce(REPLACELIST2);
    }
}

/*  NOTE:								    */
/*	Because of the long lookahead needing to pass over an actual, 	    */
/*	Replace is called with a boolean flag indicating whether or not the */
/*	actual has already been parsed.					    */
static void
Replace(bool actualParsed) {
    if (! actualParsed) {
	Actual();
    }
    Accept(forTOKEN, "expecting `for' in replacement");
    Formal();
    Reduce(REPLACE1);
}

static void
SimpleTrait(void) {
    OpParts();
    PropParts();
    Reduce(SIMPLETRAIT1);
}

static void
OpParts(void) {

    Reduce(OPPARTS1);

    while (CodePart(nextToken) == introducesTOKEN) {
	Shift(nextToken);			       /* shift `introduces' */
	nextToken = Scan();
	OpDcls();
	Reduce(OPPARTS2);
    }
}

static void
OpDcls(void) {
    OpDcl();
    Reduce(OPDCLS1);

    for (;;) {
        if (CodePart(nextToken) == COMMA) nextToken = Scan();
	if (CodePart(nextToken) == SIMPLEID
	||  CodePart(nextToken) == ifTOKEN
	||  CodePart(nextToken) == MARKERSYM
	||  CodePart(nextToken) == OPENSYM
        ||  CodePart(nextToken) == LBRACK
	||  CodePart(nextToken) == SELECTSYM
	||  CodePart(nextToken) == SIMPLEOP
	||  CodePart(nextToken) == LOGICALOP
	||  CodePart(nextToken) == EQOP
           ) {
	    OpDcl();
	    Reduce(OPDCLS2);
        } else {
	    break;
        }
    }
}

static void
OpDcl(void) {
    NameList();
    Accept(COLON, "expecting `:' before domain");
    Signature();
    Reduce(OPDCL1);
}

static void
NameList(void) {
    Name();
    Reduce(NAMELIST1);				/* first name		    */
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	Name();
	Reduce(NAMELIST2);
    }
}

static void
PropParts(void) {
    Reduce(PROPPARTS1);

    while (CodePart(nextToken) == assertsTOKEN) {
	Shift(nextToken);				/* shift `asserts'  */
        nextToken = Scan();
        Props();
        Reduce(PROPPARTS2);
    }
}

/* It takes a bit of reading to determine that it's safe to decide that the */
/* next token has to be a SIMPLEID in order to apply the second reduction.  */
/* It isn't necessary to use LookAhead() to check for GENERATED or	    */
/* PARTITIONED, although that would certainly work, too.		    */
static void
Props(void) {
    switch (CodePart(nextToken)) {
    case sortTOKEN:
    case SIMPLEID:
	GenPartProps();
	EqPart();
	Reduce(PROPS3);
	break;
    case QUANTIFIERSYM:
    case equationsTOKEN:
	EqPart();
	Reduce(PROPS2);
	break;
    default:
	Reduce(PROPS1);
	break;
    }
}

static void
GenPartProps(void) {
    GenPartProp();
    Reduce(GENPARTPROPS1);

    while (    CodePart(nextToken) == sortTOKEN
	    || CodePart(LookAhead()) == generatedTOKEN
	    || CodePart(LookAhead()) == partitionedTOKEN ) {
	GenPartProp();
	Reduce(GENPARTPROPS2);
    }
}

static void
GenPartProp(void) {
    if (CodePart(nextToken) == sortTOKEN) nextToken = Scan();
    Sort();
    switch (CodePart(nextToken)) {
	case generatedTOKEN:
	    Shift(nextToken);			    /* shift `generated'    */
	    nextToken = Scan();
	    if (CodePart(nextToken) == freelyTOKEN) {
	    Shift(nextToken);			    /* shift `freely'	    */
	      nextToken = Scan();
	      Accept(byTOKEN, "expecting BY after GENERATED FREELY");
	      OperatorList();
	      Reduce(GENPARTPROP1);
	    } else {
	      Accept(byTOKEN, "expecting BY after GENERATED");
	      OperatorList();
	      Reduce(GENPARTPROP2);
	    }
	    break;

	case partitionedTOKEN:
	    Shift(nextToken);			    /* shift `partitioned'  */
	    nextToken = Scan();
	    Accept(byTOKEN, "expecting BY after PARTITIONED");
	    OperatorList();
	    Reduce(GENPARTPROP3);
	    break;

	default:
	    UserError("expecting GENERATED BY or PARTITIONED BY");
	    break;
	}
}

static void
EqPart(void) {
    switch (CodePart(nextToken)) {
    case equationsTOKEN:
	Shift(nextToken);			    /* shift `equations'    */
	nextToken = Scan();
	EqSeq();
	QuantifiedEqSeqs();
	Reduce(EQPART2);
	break;

    default:	
	QuantifiedEqSeqs();
	Reduce(EQPART1);
	break;
    }
}

static void
OperatorList(void) {
    Operator();
    Reduce(OPERATORLIST1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
	nextToken = Scan();
	Operator();
	Reduce(OPERATORLIST2);
    }
}

static void
Operator(void) {
    Name();
    switch ( CodePart(nextToken) ) {
    case COLON:
	Shift(nextToken);				/* shift `:'	    */
	nextToken = Scan();
	Signature();
	Reduce(OPERATOR2);
	break;
    default:
	Reduce(OPERATOR1);
	break;
    }
}

static void
QuantifiedEqSeqs(void) {
    Reduce(QUANTIFIEDEQSEQS1);
    while (CodePart(nextToken) == QUANTIFIERSYM) {
	Quantifier();
	EqSeq();
	Reduce(QUANTIFIEDEQSEQS2);
    }
}

static void
EqSeq(void) {
    EqSeqHead();
    if (CodePart(nextToken) == EQSEPSYM) {
	Shift(nextToken);			/* shift `\eqsep'	    */
	nextToken = Scan();
	Reduce(EQSEQ2);
    } else {
	Reduce(EQSEQ1);
    }
}

static void
EqSeqHead(void) {
    Equation();
    Reduce(EQSEQHEAD1);
    while (CodePart(nextToken) == EQSEPSYM) {
	if (CodePart(LookAhead()) == ifTOKEN
	||  CodePart(LookAhead()) == SIMPLEOP
	||  CodePart(LookAhead()) == OPENSYM
	||  CodePart(LookAhead()) == LBRACK
	||  CodePart(LookAhead()) == LPAR
	||  CodePart(LookAhead()) == SIMPLEID
	||  CodePart(LookAhead()) == QUANTIFIERSYM1
	    ) {
	    Shift(nextToken);			/* shift `\eqsep'	    */
	    nextToken = Scan();
	    Equation();
	    Reduce(EQSEQHEAD2);
	} else {
	  break;
	}
      }
  }


static void
Quantifier() {
    Accept(QUANTIFIERSYM, "expecting quantifer");
    VarDcl();
    Reduce(QUANTIFIER1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);			/* shift `,'		    */
	nextToken = Scan();
	VarDcl();
	Reduce(QUANTIFIER2);
    }
}

static void
VarDcl(void) {
    VarIdList();
    Accept(COLON, "expecting `:' in variable declaration");
    Sort();
    Reduce(VARDCL1);
}

static void
VarIdList(void) {
    VarId();
    Reduce(VARIDLIST1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
        nextToken = Scan();
        VarId();
        Reduce(VARIDLIST2);
    }
}

static void
Equation(void) {
    Term();
    if (CodePart(nextToken) == EQUATIONSYM) {
	Shift(nextToken);				/* shift `=='	    */
	nextToken = Scan();
	Term();
	Reduce(EQUATION2);
    } else {
	Reduce(EQUATION1);
    }
}

static void
Term(void) {
    switch (CodePart(nextToken)) {
    case ifTOKEN:
	Shift(nextToken);				/* shift `if'	    */
	nextToken = Scan();
	Term();
	Accept(thenTOKEN, "expecting THEN");
	Term();
	Accept(elseTOKEN, "expecting ELSE");
	Term();
	Reduce(TERM2);
	break;
    default:
	LogicalTerm(TOPLOGICALPRIORITY);
	Reduce(TERM1);
	break;
    }
}

static void
LogicalTerm(int priority) {
  LSLToken op;
  if (priority < LOWLOGICALPRIORITY) {
    SimpleOpTerm();
    Reduce(LOGICALTERM1);
    return;
  }
  LogicalTerm(priority-1);
  if (token_priority(nextToken) > priority
      || !(CodePart(nextToken) == SIMPLEOP
	   || CodePart(nextToken) == EQOP
	   || CodePart(nextToken) == LOGICALOP)) {
    return;
  }
  op = nextToken;
  for (;;) {
    Shift(nextToken);				/* shift logical operator */
    nextToken = Scan();
    LogicalTerm(priority-1);
    if (CodePart(op) == LOGICALOP)
      Reduce(LOGICALTERM2);
    else
      Reduce(LOGICALTERM3);
    if (token_priority(nextToken) > priority
	|| !(CodePart(nextToken) == SIMPLEOP
	     || CodePart(nextToken) == EQOP
	     || CodePart(nextToken) == LOGICALOP)) {
      return;
    }
    if (!LSLSameLeaf(&op, &nextToken) 
	|| LSLSameLeaf(&nextToken, &EqToken)
	|| LSLSameLeaf(&nextToken, &NeqToken)
	|| LSLSameLeaf(&nextToken, &ImpliesToken)) {
      UserError("expecting parentheses to specify associativity");
    }
  }
}

static void
SimpleOpTerm() {
    if 
      (CodePart(nextToken) == SIMPLEOP     
    || CodePart(nextToken) == QUANTIFIERSYM1
       ) {
	PrefixOpTerm(FALSE);
	Reduce(SIMPLEOPTERM1);
    } else if
       (CodePart(nextToken) == OPENSYM
    ||	CodePart(nextToken) == LPAR
    ||	CodePart(nextToken) == LBRACK
    ||	CodePart(nextToken) == SIMPLEID
       ) {
	Secondary();					/* long lookahead   */
	if (CodePart(nextToken) == SIMPLEOP) {
	    if (CodePart(LookAhead()) == LPAR
	    ||	CodePart(LookAhead()) == LBRACK
	    ||	CodePart(LookAhead()) == SIMPLEID
	    ||	CodePart(LookAhead()) == OPENSYM
	       ) {	    /* If it's followed by a secondary, it's infix  */
		InfixOpPart();
		Reduce(SIMPLEOPTERM3);
	    } else {	    /* Otherwise, it's a postfix operator sequence  */
		PostfixOps();
		Reduce(SIMPLEOPTERM2);
	    }
	} else {	    /* No operator after the current secondary	    */
	    PrefixOpTerm(TRUE);
	    Reduce(SIMPLEOPTERM1);
	}
    } else {
	UserError("expecting variable, simple operator, or term");
    }
}

/* NOTE:								    */
/*	Because of the long lookahead required by SimpleOpTerm, we pass a   */
/*	boolean flag to PrefixOpTerm to indicate whether or not a Secondary */
/*	has already been parsed.  This could be optimized by having	    */
/*	SimpleOpTerm do the Reduce(PREFIXOPTERM1) in addition to the	    */
/*	Reduce(SIMPLEOPTERM1) in this case, thus obviating the need for	    */
/*	passing a parameter to a routine that's called recursively.	    */

static void
PrefixOpTerm(bool secondaryParsed) {
    if (secondaryParsed) {
	Reduce(PREFIXOPTERM1);
    } else if (CodePart(nextToken) == SIMPLEOP) {
	Shift(nextToken);				/* shift SIMPLEOP  */
	nextToken = Scan();
	PrefixOpTerm(FALSE);				/* Right recursion  */
	Reduce(PREFIXOPTERM2);
    } else if (CodePart(nextToken) == QUANTIFIERSYM1) {
	QuantifiedVar();
	PrefixOpTerm(FALSE);				/* Right recursion  */
	Reduce(PREFIXOPTERM3);
    } else {
	Secondary();
	Reduce(PREFIXOPTERM1);
    }
}	

static void
QuantifiedVar(void) {
  Shift(nextToken);					/* shift QUANTIFIER */
  nextToken = Scan();
  if (CodePart(nextToken) == SIMPLEID) {
    Shift(nextToken);
    nextToken = Scan();
  } else {
    UserError("expecting variable after quantifier");
  }
  if (CodePart(nextToken) == COLON) {
    Shift(nextToken);					/* shift `:'	  */
    nextToken = Scan();
    Sort();
    Reduce(QUANTIFIEDVAR2);
  } else {
    Reduce(QUANTIFIEDVAR1);
  }
}

static void
PostfixOps(void) {
    if (CodePart(nextToken) == SIMPLEOP) {
	Shift(nextToken);				/* shift SIMPLEOP  */
	nextToken = Scan();
	Reduce(POSTFIXOPS1);
	while (CodePart(nextToken) == SIMPLEOP) {
	    Shift(nextToken);				/* shift SIMPLEOP  */
	    nextToken = Scan();
	    Reduce(POSTFIXOPS2);
	}
    } else {
	CompilerFailure("PostfixOps()");
    }
}

static void
InfixOpPart(void) {
    LSLToken op;
    if (CodePart(nextToken) == SIMPLEOP) {
	op = nextToken;					/* remember SIMPLEOP */
	Shift(nextToken);				/* shift SIMPLEOP    */
	nextToken = Scan();
	Secondary();
	Reduce(INFIXOPPART1);
    } else {
	CompilerFailure("InfixOps()");
    }
    while (CodePart(nextToken) == SIMPLEOP) {
	if (nextToken.text != op.text) {
	    UserError("expecting parentheses to specify associativity");
	}
	Shift(nextToken);				/* shift SIMPLEOP    */
	nextToken = Scan();
	Secondary();
	Reduce(INFIXOPPART2);
    }
}


static void
Secondary(void) {
    switch (CodePart(nextToken)) {

    case LBRACK:
    case OPENSYM:
	Bracketed();
	if (CodePart(nextToken) == LPAR
	||  CodePart(nextToken) == SIMPLEID
           ) {
	    Primary();
	    Reduce(SECONDARY3);
        } else {
	    Reduce(SECONDARY2);
	}
	break;
	
    case LPAR:
    case SIMPLEID:
	Primary();
	if (CodePart(nextToken) == OPENSYM || CodePart(nextToken) == LBRACK) {
	    Bracketed();
	    if (CodePart(nextToken) == LPAR
	    ||  CodePart(nextToken) == SIMPLEID
	       ) {
		Primary();
		Reduce(SECONDARY5);
	    } else {
		Reduce(SECONDARY4);
	    }
	} else {
	    Reduce(SECONDARY1);
        }
	break;

    default:
	UserError("expecting primary or secondary operand");
	break;
    }
}

static void
Bracketed(void) {
    Matched();
    if (CodePart(nextToken) == COLON) {
	Shift(nextToken);				/* shift `:'	    */
	nextToken = Scan();
	Sort();
	Reduce(BRACKETED1);
    } else {
	Reduce(BRACKETED2);
    }
}

static void
Matched(void) {
    if (CodePart(nextToken) == OPENSYM || CodePart(nextToken) == LBRACK) {
	Shift(nextToken);				/* shift opensym    */
        nextToken = Scan();
	if (CodePart(nextToken) == CLOSESYM
	    || CodePart(nextToken) == RBRACK) {
	    Shift(nextToken);				/* shift closesym   */
	    nextToken = Scan();
	    Reduce(MATCHED2);
	} else {
	    Args();
	    if (CodePart(nextToken) == CLOSESYM
		|| CodePart(nextToken) == RBRACK) {
		Shift(nextToken);			/* shift closesym   */
		nextToken = Scan();
		Reduce(MATCHED1);
            } else {
		UserError("expecting closeSym");
            }
	}
    } else {
	CompilerFailure("Matched()");
    }
}

static void
Args(void) {
    Term();
    Reduce(ARGS1);
    for (;;) {
	if (CodePart(nextToken) == COMMA
	||  CodePart(nextToken) == SEPSYM) {
	    Separator();
	    Term();
	    Reduce(ARGS2);
	} else {
	    break;
	}	
    }
}

static void
Primary(void) {
    switch ( CodePart(nextToken) ) {
    case LPAR:
	Shift(nextToken);				/* shift `('	    */
	nextToken = Scan();
	Term();
	Accept(RPAR, "expecting `)' in primary");
	Reduce(PRIMARY1);
	break;
    case SIMPLEID:
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
	if (CodePart(nextToken) == LPAR ) {
	    Shift(nextToken);				/* shift `('	    */
	    nextToken = Scan();
	    TermList();
	    Accept(RPAR, "expecting `)' in primary");
	    Reduce(PRIMARY3);
        } else {
	    Reduce(PRIMARY2);
	}
	break;
    default:
	CompilerFailure("Primary()");
	break;
    }

    for (;;) {
        if (CodePart(nextToken) == SELECTSYM) {
	    Shift(nextToken);				/* shift selectsym */
            nextToken = Scan();
	    if (CodePart(nextToken) == SIMPLEID) {
		Shift(nextToken);			/* shift id	    */
		nextToken = Scan();
		Reduce(PRIMARY4);
	    } else {
		UserError("expecting identifier after selection symbol");
	    }
	} else if (CodePart(nextToken) == COLON) {
	    Shift(nextToken);				/* shift `:'	    */
            nextToken = Scan();
	    Sort();
	    Reduce(PRIMARY5);
	} else {
	    break;
	}	
    }
}

static void
TermList(void) {
    Term();
    Reduce(TERMLIST1);
    while (CodePart(nextToken) == COMMA) {
	Shift(nextToken);				/* shift `,'	    */
        nextToken = Scan();
	Term();
	Reduce(TERMLIST2);
    }
}

static void
Consequences(void) {
    Accept(impliesTOKEN, "expecting consequences to begin with `implies'");
    ConseqProps();
    Conversions();
    Reduce(CONSEQUENCES1);
}

static void
ConseqProps(void) {
    bool traitRefs;
    bool genParts;
    traitRefs = ((CodePart(nextToken) == SIMPLEID &&
		  CodePart(LookAhead()) != generatedTOKEN &&
		  CodePart(LookAhead()) != partitionedTOKEN)
		 || CodePart(nextToken) == LPAR);
    if (traitRefs) {
	TraitRefList();
    }
    genParts = ((CodePart(nextToken) == SIMPLEID &&
		(CodePart(LookAhead()) == generatedTOKEN ||
		 CodePart(LookAhead()) == partitionedTOKEN))
		|| (CodePart(nextToken) == sortTOKEN));
    if (genParts) {
	GenPartProps();
    }
    if (genParts &
	(CodePart(nextToken) == SIMPLEID
	 || CodePart(nextToken) == LPAR
	 || CodePart(nextToken) == OPENSYM
	 || CodePart(nextToken) == LBRACK
	 || CodePart(nextToken) == SIMPLEOP
	 || CodePart(nextToken) == QUANTIFIERSYM1
	 || CodePart(nextToken) == ifTOKEN)) {
	EqSeq();
	if (traitRefs) {
	    Reduce(CONSEQPROPS6); /* traitRefList genPartProps eqSeq  */
	} else {
	    Reduce(CONSEQPROPS5); /* 		genPartProps eqSeq  */
	} 
    } else {
	EqPart();
	if (traitRefs) {
	    if (genParts) {
		Reduce(CONSEQPROPS4); /* traitRefList genPartProps eqPart */
	    } else {
		Reduce(CONSEQPROPS2); /* traitRefList		     eqPart */
	    }
	} else if (genParts) {
	    Reduce(CONSEQPROPS3); /*              genPartProps eqPart */
	} else {
	    Reduce(CONSEQPROPS1); /*			     eqPart */
	}
    }
}

static void
Conversions(void) {
    Reduce(CONVERSIONS1);
    while (CodePart(nextToken) == convertsTOKEN) {
	Conversion();
	Reduce(CONVERSIONS2);
    }
}

static void
Conversion(void) {
    Shift(nextToken);				     	/* shift `converts'  */
    nextToken = Scan();
    OperatorList();
    if (CodePart(nextToken) == exemptingTOKEN) {
	Exemption();
	Reduce(CONVERSION2);
    } else {
	Reduce(CONVERSION1);
    }
}

static void
Exemption(void) {
    if (CodePart(nextToken) == exemptingTOKEN) {
	Shift(nextToken);				/* shift `exempting' */
	nextToken = Scan();
	if (CodePart(nextToken) == QUANTIFIERSYM) {
	    Quantifier();
	    TermList();
	    Reduce(EXEMPTION2);
	} else {
	    TermList();
	    Reduce(EXEMPTION1);
	}
    } else {
	CompilerFailure("Exemption()");
    }
}

static void
TraitId(void) {
    switch ( CodePart(nextToken) ) {
    case SIMPLEID:
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
	break;
    default:
	UserError("expecting trait identifier");
	break;
    }
    Reduce(TRAITID1);
}

static void
SortId(void) {
    switch ( CodePart(nextToken) ) {
    case SIMPLEID:
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
	break;
    default:
	UserError("expecting sort identifier");
	break;
    }
    Reduce(SORTID1);
}

static void
VarId(void) {
    switch ( CodePart(nextToken) ) {
    case SIMPLEID:
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
	break;
    default:
	UserError("expecting variable identifier");
	break;
    }
    Reduce(VARID1);
}

static void
FieldId(void) {
    switch ( CodePart(nextToken) ) {
    case SIMPLEID:
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
	break;
    default:
	UserError("expecting field identifier");
	break;
    }
    Reduce(FIELDID1);
}

static void
ElementId(void) {
    if (CodePart(nextToken) == SIMPLEID) {
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
    } else {
	UserError("expecting identifier in element list");
    }
    Reduce(ELEMENTID1);
}

static void
SortOrOpId(void) {
    if (CodePart(nextToken) == SIMPLEID) {
	Shift(nextToken);				/* shift id	    */
	nextToken = Scan();
    } else {
	UserError("expecting identifier for sort or operator");
    }
    Reduce(SORTOROPID1);
}

/*
** Utilities, in alphabetical order
*/

/* Accept looks for the given token, shifts it if it's there, and	    */
/* produces a user error message if it isn't.				    */

static void
Accept(LSLTokenCode t, char *msg) {
    if (CodePart(nextToken) == t) {
	Shift(nextToken);
	nextToken = Scan();
    } else {
	UserError(msg);
    }
}

static void
UserError(char *msg) {
    error_reportLocation(token_fileName(nextToken), token_line(nextToken),
			 token_col(nextToken));
    error_reportAndQuit(msg);
}


/*
 *  Required initialization and cleanup routines
 */

void
LSLParseInit(void) {
    LSLReportEolTokens(FALSE);				/* Don't report EOL */
    LSLReportCommentTokens(FALSE);			/* or comments      */
}

void
LSLParseReset(void) {
    nextToken = Scan();					/* first token	    */
}

void
LSLParseCleanup(void) {
}
