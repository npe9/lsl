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
**      FILENAME:   parse.h
**
**	PURPOSE:    Interface to Larch Shared Language Parser
**	
**
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at Digital -- 90.04.20 -- Original
**	{1} Feldman  at Digital	-- 90.07.19 -- Convert to 19 July grammar
**	{2} Feldman  at Digital	-- 90.08.07 -- Add comments describing shifts
**	{3} Garland  at MIT	-- 90.09.12 -- Tidy up
**	{4} Garland  at MIT	-- 90.09.23 -- Fix props, conseqprops
**	{5} Garland  at MIT	-- 90.10.10 -- Shift all tokens
**      {6} Garland  at MIT     -- 90.11.06 -- Remove redundant null conseqprops
**	{7} Garland  at MIT     -- 92.01.13 -- Allow eqSepSym at end of eqSeq
**	{8} Garland  at MIT	-- 92.06.14 -- Composite sort notation A$B
**	{9} Garland  at MIT	-- 93.11.09 -- Quantified formulas
**     {10} Garland  at MIT	-- 93.11.26 -- Iff
**     {11} Garland  at MIT	-- 94.04.20 -- Generated freely
**     {12} Garland  at MIT	-- 94.05.20 -- Composite sort notation A[B, C]
**     {13} Garland  at MIT	-- 94.05.27 -- Add "," in opDcls, redo formals
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS (none)
**
*/

/*
**
**  TYPEDEFS
**
*/


/*  The following enum documents the grammar, the productions, and the	    */
/*  Shift/Reduce behavior of the parser.  Each and every production in the  */
/*  grammar is assigned an identifier in the enum.			    */
/*									    */
/*  Whenever a production is applied, the parser emits a Reduce(ruleId).    */
/*  Productions that reduce the empty string for optional fragments are	    */
/*  only applied if the fragment is missing, while productions that reduce  */
/*  the empty string for lists are always applied once.  For example, the   */
/*  two productions for middle are:					    */
/*	middle								    */
/*	    <empty>							    */
/*	    placeList							    */
/*  Either placeList is present, in which case Reduce(MIDDLE2) is invoked,  */
/*  or placeList is not present, in which case Reduce(MIDDLE1) is invoked,  */
/*  but not both.  On the other hand, the two productions for opParts are:  */
/*	opParts								    */
/*	    <empty>							    */
/*	    opParts INTRODUCES opDcls					    */
/*  In this case, Reduce(OPPARTS1) is always called first, followed by	    */
/*  however many instances of Reduce(OPPARTS2) are needed (possibly zero).  */


typedef enum {
			/* trait:					    */
    TRAIT1,		/*    traitId                : TRAIT traitBody EOF  */
    TRAIT2,		/*    traitId ( formalList ) : TRAIT traitBody EOF  */

			/* formalList:					    */
    FORMALLIST1,	/*                   formal			    */
    FORMALLIST2,	/*      formalList , formal			    */

			/* formal:					    */
    FORMAL1,		/*      name : signature			    */
    FORMAL2,		/*	opForm					    */
    FORMAL3,		/*	compoundSort				    */
    FORMAL4,		/*	sortOrOpId				    */

			/* operator:					    */
    OPERATOR1,		/*	name					    */
    OPERATOR2,		/*	name : signature			    */

			/* name:					    */
    NAME1,		/*	simpleId				    */
    NAME2,		/*	opForm					    */

			/* opForm:					    */
    OPFORM1,		/*	IF markerSym THEN markerSym ELSE markerSym  */
    OPFORM2,		/*	          anyOp				    */
    OPFORM3,		/*	markerSym anyOp				    */
    OPFORM4,		/*	          anyOp markerSym		    */
    OPFORM5,		/*	markerSym anyOp markerSym		    */
    OPFORM6,		/*	          openSym middle closeSym	    */
    OPFORM7,		/*	markerSym openSym middle closeSym	    */
    OPFORM8,		/*	          openSym middle closeSym markerSym */
    OPFORM9,		/*	markerSym openSym middle closeSym markerSym */
    OPFORM10,		/*	          selectSym simpleId		    */
    OPFORM11,		/*	markerSym selectSym simpleId		    */

			/* anyOp:					    */
    ANYOP1,		/*	simpleOp				    */
    ANYOP2,		/*	logicalOp				    */
    ANYOP3,		/*	eqOp					    */

			/* middle:					    */
    MIDDLE1,		/*						    */
    MIDDLE2,		/*      placeList				    */

			/* placeList:					    */
    PLACELIST1,		/*                          markerSym		    */
    PLACELIST2,		/*      placeList separator markerSym		    */

			/* separator:					    */
    SEPARATOR1,		/*	sepSym					    */
    SEPARATOR2,		/*	,					    */

			/* signature:					    */
    SIGNATURE1,		/*	domain mapSym range			    */

			/* domain:					    */
    DOMAIN1,		/*						    */
    DOMAIN2,		/*	sortList				    */

			/* sortList:					    */
    SORTLIST1,		/*	sort					    */
    SORTLIST2,		/*	sortList , sort				    */

			/* range					    */
    RANGE1,		/*	sort					    */

			/* sort:					    */
    SORT1,		/*	sortId					    */
    SORT2,		/*	sortId [ sortList ]			    */

			/* traitBody:					    */
    TRAITBODY1,		/*                    simpleTrait		    */
    TRAITBODY2,		/*                    simpleTrait consequences	    */
    TRAITBODY3,		/*      traitContexts simpleTrait		    */
    TRAITBODY4,		/*      traitContexts simpleTrait consequences	    */

			/* traitContexts:				    */
    TRAITCONTEXTS1,	/*	              traitContext		    */
    TRAITCONTEXTS2,	/*	traitContexts traitContext		    */

			/* traitContext:				    */
    TRAITCONTEXT1,	/*      shorthand				    */
    TRAITCONTEXT2,	/*      external				    */

			/* shorthand:					    */
    SHORTHAND1,		/*      enumeration				    */
    SHORTHAND2,		/*      tuple					    */
    SHORTHAND3,		/*      union					    */

			/* enumeration:					    */
    ENUMERATION1,	/*      sort ENUMERATION OF elementIdList	    */

			/* elementIdList:				    */
    ELEMENTIDLIST1,	/*      	        elementId		    */
    ELEMENTIDLIST2,	/*      elementIdList , elementId		    */

			/* tuple:					    */
    TUPLE1,		/*      sort TUPLE OF fieldList			    */

			/* union:					    */
    UNION1,		/*      sort UNION OF fieldList			    */

			/* fieldList:					    */
    FIELDLIST1,		/*      	    fields			    */
    FIELDLIST2,		/*      fieldList , fields			    */

			/* fields:					    */
    FIELDS1,		/*      fieldIdList : sort			    */

			/* fieldIdList:					    */
    FIELDIDLIST1,	/*      fieldId					    */
    FIELDIDLIST2,	/*      fieldIdList , fieldId			    */

			/* external:					    */
    EXTERNAL1,		/*      INCLUDES traitRefList			    */
    EXTERNAL2,		/*      ASSUMES traitRefList			    */

			/* traitRefList:				    */
    TRAITREFLIST1,	/*      	       traitRef			    */
    TRAITREFLIST2,	/*      traitRefList , traitRef			    */

			/* traitRef:					    */
    TRAITREF1,		/*      traitId					    */
    TRAITREF2,		/*      traitId ( renaming )			    */
    TRAITREF3,		/*      ( traitIdList )				    */
    TRAITREF4,		/*      ( traitIdList ) ( renaming )		    */

			/* traitIdList:					    */
    TRAITIDLIST1,	/*      	      traitId			    */
    TRAITIDLIST2,	/*      traitIdList , traitId			    */

			/* renaming:					    */
    RENAMING1,		/*		     replaceList		    */
    RENAMING2,		/*      actualList				    */
    RENAMING3,		/*      actualList , replaceList		    */

			/* actualList:					    */
    ACTUALLIST1,	/*		     actual			    */
    ACTUALLIST2,	/*	actualList , actual			    */

    			/* actual:					    */
    ACTUAL1,		/*	name					    */
    ACTUAL2,		/*	sort					    */
    ACTUAL3,		/*	sortOrOpId				    */

    			/* In case of ambiguity, ACTUAL3 is chosen.	    */

			/* replaceList:					    */
    REPLACELIST1,	/*		      replace			    */
    REPLACELIST2,	/*	replaceList , replace			    */

			/* replace:					    */
    REPLACE1,		/*	actual FOR formal			    */

			/* simpleTrait:					    */
    SIMPLETRAIT1,	/*      opParts propParts			    */

			/* opParts:					    */
    OPPARTS1,		/*						    */
    OPPARTS2,		/*      opParts INTRODUCES opDcls		    */

			/* opDcls:					    */
    OPDCLS1,		/*             opDcl				    */
    OPDCLS2,		/*      opDcls ',' opDcl			    */
    OPDCLS3,		/*	opDcls     opDcl			    */

			/* opDcl:					    */
    OPDCL1,		/*      NameList : signature			    */

			/* nameList:					    */
    NAMELIST1,		/*		   name				    */
    NAMELIST2,		/*	nameList , name				    */

			/* propParts:					    */
    PROPPARTS1,		/*						    */
    PROPPARTS2,		/*      propParts ASSERTS props			    */

			/* props:					    */
    PROPS1,		/*						    */
    PROPS2,		/*      	     eqPart			    */
    PROPS3,		/*      genPartProps eqPart			    */

			/* genPartProps:				    */
    GENPARTPROPS1,	/*      	     genPartProp		    */
    GENPARTPROPS2,	/*      genPartProps genPartProp		    */

			/* genPartProp:					    */
    GENPARTPROP1,	/*	sort GENERATED FREELY BY operatorList	    */
    GENPARTPROP2,	/*	sort GENERATED BY operatorList		    */
    GENPARTPROP3,	/*	sort PARTITIONED BY operatorList	    */

			/* eqPart:					    */
    EQPART1,		/*      		quantifiedEqSeqs	    */
    EQPART2,		/*      EQUATIONS eqSeq quantifiedEqSeqs	    */

			/* operatorList:				    */
    OPERATORLIST1,	/*      	       operator			    */
    OPERATORLIST2,	/*      operatorList , operator			    */

			/* quantifiedEqSeqs:				    */
    QUANTIFIEDEQSEQS1,	/*						    */
    QUANTIFIEDEQSEQS2,	/*      quantifiedEqSeqs quantifier eqSeq	    */

			/* eqSeq:					    */
    EQSEQ1,		/* 	eqSeqHead				    */
    EQSEQ2,		/* 	eqSeqHead eqSepSym			    */

			/* eqSeqHead:					    */
    EQSEQHEAD1,		/*      equation				    */
    EQSEQHEAD2,		/*      eqSeqHead eqSepSym equation		    */

			/* quantifier:					    */
    QUANTIFIER1,	/*      quantifierSym varDcl			    */
    QUANTIFIER2,	/*      quantifier , varDcl			    */

			/* varDcl:					    */
    VARDCL1,		/*      varIdList : sort			    */

			/* varIdList:					    */
    VARIDLIST1,		/*      	    varId			    */
    VARIDLIST2,		/*      varIdList , varId			    */

			/* equation:					    */
    EQUATION1,		/*      term					    */
    EQUATION2,		/*      term equationSym term			    */

			/* term:					    */
    TERM1,		/*      logicalTerm				    */
    TERM2,		/*      IF term THEN term ELSE term		    */

			/* logicalTerm:					    */
    LOGICALTERM1,	/*			      simpleOpTerm	    */
    LOGICALTERM2,	/*	logicalTerm logicalOp simpleOpTerm	    */
    LOGICALTERM3,	/*	logicalTerm eqOp simpleOpTerm 		    */


			/* simpleOpTerm:				    */
    SIMPLEOPTERM1,	/*	prefixOpTerm				    */
    SIMPLEOPTERM2,	/*	secondary postfixOps			    */
    SIMPLEOPTERM3,	/*	secondary infixOpPart			    */

			/* prefixOpTerm:				    */
    PREFIXOPTERM1,	/*      secondary				    */
    PREFIXOPTERM2,	/*      simpleOp prefixOpTerm			    */
    PREFIXOPTERM3,	/*      quantifiedVar prefixOpTerm		    */

    			/* quantifiedVar:				    */
    QUANTIFIEDVAR1,	/*	quantifierSym1 simpleId			    */
    QUANTIFIEDVAR2,	/*	quantifierSym1 simpleId : sort		    */


			/* postfixOps:					    */
    POSTFIXOPS1,	/*      	   simpleOp			    */
    POSTFIXOPS2,	/*      postfixOps simpleOp			    */

			/* infixOpPart:					    */
    INFIXOPPART1,	/*      simpleOp secondary			    */
    INFIXOPPART2,	/*      infixOpPart simpleOp secondary		    */

			/* secondary:					    */
    SECONDARY1,		/*      primary					    */
    SECONDARY2,		/*      	bracketed			    */
    SECONDARY3,		/*      	bracketed primary		    */
    SECONDARY4,		/*      primary bracketed			    */
    SECONDARY5,		/*      primary bracketed primary		    */

			/* bracketed:					    */
    BRACKETED1,		/*      matched : sort				    */
    BRACKETED2,		/*      matched					    */

			/* matched:					    */
    MATCHED1,		/*      openSym args closeSym			    */
    MATCHED2,		/*      openSym      closeSym			    */

			/* args:					    */
    ARGS1,		/*      	       term			    */
    ARGS2,		/*      args separator term			    */

			/* primary:					    */
    PRIMARY1,		/*      ( term )				    */
    PRIMARY2,		/*      simpleId				    */
    PRIMARY3,		/*      simpleId ( termList )			    */
    PRIMARY4,		/*      primary selectSym simpleId		    */
    PRIMARY5,		/*      primary : sort				    */

			/* termList:					    */
    TERMLIST1,		/*      	   term				    */
    TERMLIST2,		/*      termList , term				    */

			/* consequences:				    */
    CONSEQUENCES1,	/*      IMPLIES conseqProps conversions		    */

			/* conseqProps:					    */
    CONSEQPROPS1,	/*		     		  eqPart	    */
    CONSEQPROPS2,	/*	traitRefList		  eqPart	    */
    CONSEQPROPS3,	/*		     genPartProps eqPart	    */
    CONSEQPROPS4,	/*	traitRefList genPartProps eqPart	    */
    CONSEQPROPS5,	/*		     genPartProps eqSeq		    */
    CONSEQPROPS6,	/*	traitRefList genPartProps eqSeq		    */

			/* conversions:					    */
    CONVERSIONS1,	/*						    */
    CONVERSIONS2,	/*      conversions conversion			    */

			/* conversion:					    */
    CONVERSION1,	/*      CONVERTS operatorList			    */
    CONVERSION2,	/*      CONVERTS operatorList exemption		    */

			/* exemption:					    */
    EXEMPTION1,		/*      EXEMPTING 	     termList		    */
    EXEMPTION2,		/*      EXEMPTING quantifier termList		    */

			/* traitId:					    */
    TRAITID1,		/*      simpleId				    */

			/* sortId:					    */
    SORTID1,		/*	simpleId				    */

			/* varId:					    */
    VARID1,		/*      simpleId				    */

			/* fieldId:					    */
    FIELDID1,		/*      simpleId				    */

			/* elementId:					    */
    ELEMENTID1,		/*      simpleId				    */

			/* sortOrOpId:					    */
    SORTOROPID1		/*      simpleId				    */

}	    LSLRuleCode;

/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void		LSLParse(void);			/* parser entry	    */
extern bool             LSLParseReplace(void);


extern void		LSLParseInit(void);
extern void		LSLParseReset(void);
extern void		LSLParseCleanup(void);

/*
**
**  EXTERNAL VARIABLES (none)
**
*/

