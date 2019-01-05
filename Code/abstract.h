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
**      FILENAME:   abstract.h
**
**	PURPOSE:    Interface to LSL abstract syntax
**
**	     The abstract syntax for LSL reflects the concrete syntax, with the
**  	following exceptions:
**	(1) The abstract syntax for terms is much simpler than the concrete 
**	    syntax.  Much of the extra information in the concrete syntax
**	    (such as parentheses) can either be recovered from the parse tree
**	    or is subject to change when operators are renamed.
**	(2) An eqSeq in the consequences becomes an eqPart.
**	(3) Comments and line breaks do not appear in the abstract syntax.
**	(4) Only a single qualification is retained for terms such as "0:N:N".
**
**	     The abstract syntax could be simplified further, e.g., by
**	replacing "opPartList? propPartList?" with "opPart? propPart?" in the
**	production for a trait.  The current version of the abstract syntax
**	retains this extra structure for two reasons.
**	(1) It may make it easier to annotate where things come from when
**	    expanding traitRefs.
**	(2) It retains information that we may not wish to throw away when
**	    prettyprinting.  However, to construct a prettyprinter, we would
**	    also have to record comments in the abstract syntax, and we might
**	    also want to record other details (such as where the user typed
**	    superfluous parentheses).
**      
**
**	USAGE: 	# include "general.h"
**		# include "token.h"
**		# include "parse.h"
**		# include "list.h"
**		# include "set.h"
**		# include "abstract.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.09.12 -- Original
**	{1} Garland at MIT     -- 90.10.10 -- Add locators
**	{2} Wild    at Digital -- 90.11.09 -- Change ## to PASTE().
**	{3} Wild    at Digital -- 91.02.20 -- Merge in MIT changes.
**	{7} Garland at MIT     -- 91.12.20 -- Add several make*Node for lsl2lp
**	{8} Garland at MIT     -- 92.01.09 -- Add hash key to some nodes
**	{9} Garland at MIT     -- 92.01.09 -- Add locator for traitRef
**     {10} Garland at MIT     -- 92.01.23 -- Add LSLAbstractParsedReplace
**     {10} Garland at MIT     -- 92.02.23 -- Change nameNodeMatches
**     {11} Garland at MIT     -- 92.06.14 -- Composite sort notation A$B
**     {12} Garland at MIT     -- 93.01.07 -- Add commandLineArg to traitBody
**     {13} Garland at MIT     -- 93.11.09 -- Add quantified formulas
**     {14} Garland at MIT     -- 93.11.26 -- Add iff
**     {15} Garland at MIT     -- 94.04.20 -- Add generated freely
**     {16} Garland at MIT     -- 94.05.20 -- Composite sort notation A[B, C]
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define LSLAbstractMakeLeaf(x, tok)					\
			    LSLAbstractMakeLeafProc(PASTE(x, LEAF), tok)


/* Macros for use in typedefs */

# define AbstractLeaf(x)    typedef LSLToken PASTE(x, Leaf)

# define AbstractNode(x)    typedef struct PASTE(PASTE(_, x), Node) {	    \
				 struct PASTE(PASTE(_, x), Body)  *body;    \
				 struct PASTE(PASTE(_, x), Attr)  *attr;    \
				 unsigned int			   key;     \
				 LSLToken		           loc;	    \
				 } PASTE(x, Node)

# define LeafList(x)	    typedef list( PASTE(x, Leaf) ) PASTE(x, LeafList)
# define NodeList(x)	    typedef list( PASTE(x, Node) ) PASTE(x, NodeList)

# define HASHMASK	    255	     /* must be one less than a power of 2 */



/*
**
**  TYPEDEFS
**
*/

typedef struct {
    struct _nameNode		*name;
    struct _signatureNode	*signature;
} operator;

typedef list(operator)	OpList;

typedef enum {
    anyOpNODE,
    assumesNODE,
    closeSymLEAF,
    consequencesNODE,
    conversionNODE,
    conversionNODELIST,
    elementIdLEAF,
    elementIdLEAFLIST,
    enumerationNODE,
    eqOpLEAF,
    equationNODE,
    equationNODELIST,
    exemptionNODE,
    extNODE,
    extNODELIST,
    fieldIdLEAF,
    fieldIdLEAFLIST,
    fieldNODE,
    fieldNODELIST,
    genByNODE,
    genPartByNODE,
    genPartByNODELIST,
    ifOpNODE,
    includesNODE,
    logicalOpLEAF,
    mixfixOpNODE,
    nameNODE,
    nameNODELIST,
    opDclNODE,
    opDclNODELIST,
    opPartNODE,
    opPartNODELIST,
    openSymLEAF,
    operatorNODE,
    operatorNODELIST,
    parameterNODE,
    parameterNODELIST,
    partByNODE,
    ppiOpNODE,
    propPartNODE,
    propPartNODELIST,
    quantEqnsNODE,
    quantEqnsNODELIST,
    quantifierLEAF,
    replaceNODE,
    replaceNODELIST,
    selectOpNODE,
    sepSymLEAF,
    separatorNODE,
    separatorNODELIST,
    signatureNODE,
    simpleIdLEAF,
    simpleOpLEAF,
    simpleOpLEAFLIST,
    sortIdLEAF,
    sortNODE,
    sortNODELIST,
    termNODE,
    termNODELIST,
    traitIdLEAF,
    traitIdLEAFLIST,
    traitNODE,
    traitRefNODE,
    traitRefNODELIST,
    tupleNODE,
    unionNODE,
    varDclNODE,
    varDclNODELIST,
    varIdLEAF,
    varIdLEAFLIST
    } nodeKind;


/* Leafs in abstract syntax tree */

AbstractLeaf(closeSym);
AbstractLeaf(elementId);
AbstractLeaf(eqOp);
AbstractLeaf(fieldId);
AbstractLeaf(logicalOp);
AbstractLeaf(openSym);
AbstractLeaf(quantifier);
AbstractLeaf(sepSym);
AbstractLeaf(simpleId);
AbstractLeaf(simpleOp);
AbstractLeaf(sortId);
AbstractLeaf(traitId);
AbstractLeaf(varId);

LeafList(elementId);
LeafList(fieldId);
LeafList(simpleOp);
LeafList(traitId);
LeafList(varId);


/* Internal nodes in abstract syntax tree */

AbstractNode(trait);		/* traitId parameterList? extList? 	     */
                                /* opPartList?                               */
    				/* propPartList? consequences?		     */
AbstractNode(parameter);	/* ONEOF operator, sort, simpleId	     */
AbstractNode(name);		/* ONEOF simpleId, selectOp, mixfixOp, 	     */
				/*	 ppiOp, ifOp, quantifier	     */
AbstractNode(selectOp);		/* beforeBool simpleId			     */
AbstractNode(mixfixOp);		/* beforeBool openSym separatorList? 	     */
				/* closeSym afterBool			     */
AbstractNode(separator);	/* sepSym   (can be comma)		     */
AbstractNode(ppiOp);		/* beforeBool anyOp afterBool 		     */
AbstractNode(anyOp);		/* ONEOF simpleOp, logicalOp, eqOp	     */
AbstractNode(ifOp);		/*					     */
AbstractNode(signature);	/* sortList sort			     */
AbstractNode(sort);		/* sortId sortList?			     */
AbstractNode(ext);		/* ONEOF enumeration, tuple, union,          */
				/*       includes, assumes		     */
AbstractNode(enumeration);	/* sort elementIdList			     */
AbstractNode(tuple);		/* sort fieldList			     */
AbstractNode(union);		/* sort fieldList			     */
AbstractNode(field);		/* fieldIdList sort			     */
AbstractNode(includes);		/* traitRefList				     */
AbstractNode(assumes);		/* traitRefList				     */
AbstractNode(traitRef);		/* traitIdList nameList? replaceList?	     */
AbstractNode(replace);		/* name name signature?			     */
AbstractNode(opPart);		/* opDclList				     */
AbstractNode(opDcl);		/* nameList signature			     */
AbstractNode(propPart);		/* genPartByList? equationList?		     */
				/* quantEqnsList?			     */
AbstractNode(genPartBy);	/* ONEOF genBy, partBy	  		     */
AbstractNode(genBy);		/* sort operatorList			     */
AbstractNode(partBy);		/* sort operatorList			     */
AbstractNode(operator);		/* name signature?			     */
AbstractNode(quantEqns);	/* varDclList equationList		     */
AbstractNode(varDcl);		/* varIdList sort			     */
AbstractNode(equation);		/* term term?				     */
AbstractNode(term);		/* name termList? sort?			     */
AbstractNode(consequences);	/* traitRefList? genPartByList? 	     */
				/* equationList? quantEqnsList?		     */
				/* conversionList?			     */
AbstractNode(conversion);	/* operatorList exemption?		     */
AbstractNode(exemption);	/* varDclList? termList			     */


/* Abstract nodes: names for operators and/or sorts */

typedef struct _selectOpBody {
    bool		before;
    simpleIdLeaf	*sel;
} selectOpBody;

typedef struct _separatorBody {
    sepSymLeaf		*separator;			     /* can be comma */
} separatorBody;

NodeList(separator);

typedef enum {simpleOpKIND, logicalOpKIND, eqOpKIND} anyOpKind;

typedef struct _anyOpBody {
    anyOpKind		kind;
    union {
	simpleOpLeaf	*simpleOp;
	logicalOpLeaf	*logicalOp;
	eqOpLeaf	*eqOp;
    } choice;
} anyOpBody;

/*
** A mixfix operator is 0-ary if it has no separatorNodeList.  Otherwise
** its arity is one more than the length of the list.
*/

typedef struct _mixfixOpBody {
    bool		before;
    openSymLeaf		*open;
    separatorNodeList	*seps;				/* none if NULL */
    closeSymLeaf	*close;
    bool		after;
} mixfixOpBody;

typedef struct _ppiOpBody {
    bool		before;
    anyOpNode		*op;
    bool		after;
} ppiOpBody;

typedef struct _ifOpBody {				/* ??? */
    char		*junk;
} ifOpBody;

/* NOTE: simpleIdKIND must be first in the enum */

typedef enum {simpleIdKIND, ifOpKIND, ppiOpKIND, mixfixOpKIND, 
              selectOpKIND, quantifierKIND} nameKind;

typedef struct _nameBody {
    nameKind		kind;
    union {
        quantifierLeaf	*quantifier;
	simpleIdLeaf	*simpleId;
	ifOpNode	*ifOp;
	ppiOpNode 	*ppiOp;
	mixfixOpNode	*mixfixOp;
	selectOpNode	*selectOp;
    } choice;
} nameBody;

NodeList(name);


/* Abstract nodes: logical statements */

NodeList(term);

typedef struct _termBody {
    nameNode		*varOrOp;
    termNodeList	*args;				/* none if NULL */
    sortNode		*qual;				/* none if NULL */
} termBody;

typedef struct _equationBody {
    termNode		*left;
    termNode		*right;				/* none if NULL */
} equationBody;

NodeList(equation);

typedef struct _varDclBody {
    varIdLeafList	*vars;
    sortNode		*sort;
} varDclBody;

NodeList(varDcl);

typedef struct _quantEqnsBody {
    varDclNodeList	*quantifier;
    equationNodeList	*equations;
} quantEqnsBody;

NodeList(quantEqns);

NodeList(sort);

typedef struct _sortBody {
    sortIdLeaf		*id;
    sortNodeList	*subsorts;
} sortBody;

typedef struct _signatureBody {
    sortNodeList	*domain;
    sortNode		*range;
} signatureBody;

typedef struct _operatorBody {
    nameNode		*name;
    signatureNode	*signature;			/* none if NULL */
} operatorBody;

NodeList(operator);

typedef struct _genByBody {
    sortNode		*sort;
    bool		 freely;
    operatorNodeList	*ops;
} genByBody;

typedef struct _partByBody {
    sortNode		*sort;
    operatorNodeList	*ops;
} partByBody;

typedef enum {genByKIND, partByKIND} genPartByKind;

typedef struct _genPartByBody{
    genPartByKind	kind;
    union {
	genByNode	*genBy;
	partByNode	*partBy;
    } choice;
} genPartByBody;

NodeList(genPartBy);


/* Abstract nodes: trait references */

/* NOTE: simpleId1KIND is really simpleIdKIND; it must be first in enum */

typedef enum {simpleId1KIND, operatorKIND, sortKIND} parameterKind;

typedef struct _parameterBody {
    parameterKind	kind;
    union {
	operatorNode	*operator;	/* no signature for actual parameter */
	sortNode	*sort;
	simpleIdLeaf	*simpleId;
    } choice;
} parameterBody;

NodeList(parameter);

typedef struct _replaceBody {
    parameterNode	*actual;
    parameterNode	*formal;
} replaceBody;

NodeList(replace);


typedef struct _traitRefBody {
    traitIdLeafList	*traits;
    parameterNodeList	*actuals;			/* none if NULL */
    replaceNodeList	*renamings;			/* none if NULL */
} traitRefBody;

NodeList(traitRef);


/* Abstract nodes: trait consequences */

typedef struct _exemptionBody {
    varDclNodeList	*forall;			/* none if NULL */
    termNodeList	*terms;
} exemptionBody;

typedef struct _conversionBody {
    operatorNodeList	*converts;
    exemptionNode	*exempts;			/* none if NULL */
} conversionBody;

NodeList(conversion);

typedef struct _consequencesBody {		        /* not all NULL */
    traitRefNodeList	*traits;
    genPartByNodeList	*genPartBys;
    equationNodeList	*eqns;
    quantEqnsNodeList	*quantEqns;
    conversionNodeList	*converts;
} consequencesBody;


/* Abstract nodes: trait axioms */

typedef struct _propPartBody {
    genPartByNodeList	*genPartBys;			/* none if NULL */
    equationNodeList	*equations;			/* none if NULL */
    quantEqnsNodeList	*quantEqns;			/* none if NULL */
} propPartBody;

NodeList(propPart);

typedef struct _opDclBody {
    nameNodeList	*ops;
    signatureNode	*signature;
} opDclBody;

NodeList(opDcl);

typedef struct _opPartBody {
    opDclNodeList	*introduces;
} opPartBody;

NodeList(opPart);


/* Abstract nodes: trait externals and shorthands */

typedef struct _assumesBody {
    traitRefNodeList	*traits;
} assumesBody;

typedef struct _includesBody {
    traitRefNodeList	*traits;
} includesBody;

typedef struct _fieldBody {
    fieldIdLeafList	*fields;
    sortNode		*sort;
} fieldBody;

NodeList(field);

typedef struct _tupleBody {
    sortNode		*sort;
    fieldNodeList	*fields;
} tupleBody;

typedef struct _unionBody {
    sortNode		*sort;
    fieldNodeList	*fields;
} unionBody;

typedef struct _enumerationBody {
    sortNode		*sort;
    elementIdLeafList	*elems;
} enumerationBody;

typedef enum {LSLenumKIND, LSLtupleKIND, LSLunionKIND, includesKIND, 
		  assumesKIND} extKind;

typedef struct _extBody {
    extKind		kind;
    union {
	enumerationNode	*LSLenum;
	tupleNode	*LSLtuple;
	unionNode	*LSLunion;
	includesNode	*includes;
	assumesNode	*assumes;
    } choice;
} extBody;

NodeList(ext);


/* Abstract nodes: trait definition */

typedef struct _traitBody {
    traitIdLeaf		*name;
    parameterNodeList	*formals;			/* none if NULL  */
    extNodeList		*exts;				/* none if NULL  */
    opPartNodeList	*introduces;			/* none if NULL  */
    propPartNodeList	*asserts;			/* none if NULL  */
    consequencesNode	*implies;			/* none if NULL  */
    bool		 commandLineArg;		/* for LP output */
} traitBody;

NodeList(trait);


/*
**
**  FUNCTION PROTOTYPES
**
*/

/*
** Requires: successful parse of trait since LSLAbstractReset
** Ensures:  result = traitNode representing trait
*/

extern traitNode	*LSLAbstractParseTree(void);

/*
** Requires: successful parse of replace since LSLAbstractReset
** Ensures:  result = replaceNode representing the replace
*/

extern replaceNode	*LSLAbstractParsedReplace(void);


/*
** Procedures for use by reduce.c in constructing the abstract syntax tree
*/

extern void		LSLAbstractMakeLeafProc(nodeKind k, LSLToken t);
extern void		LSLAbstractTrait(LSLRuleCode rule, LSLToken loc);
extern void		LSLAbstractFormal(LSLRuleCode rule);
extern void		LSLAbstractActual(LSLRuleCode rule);
extern void		LSLAbstractName(LSLRuleCode rule);
extern void		LSLAbstractIfOp(LSLToken loc);
extern void		LSLAbstractPpiOp(LSLRuleCode rule, LSLToken loc);
extern void		LSLAbstractMixfixOp(LSLRuleCode rule, LSLToken open,
					    LSLToken close, LSLToken loc);
extern void		LSLAbstractSelectOp(LSLRuleCode rule, LSLToken op,
					    LSLToken loc);
extern void		LSLAbstractAnyOp(LSLRuleCode rule, LSLToken op);
extern void		LSLAbstractSeparator(LSLToken sep);
extern void		LSLAbstractSignature(LSLToken loc);
extern void		LSLAbstractSort(LSLRuleCode rule);
extern void		LSLAbstractExt(LSLRuleCode rule);
extern void		LSLAbstractEnumeration(LSLToken loc);
extern void		LSLAbstractTuple(LSLToken loc);
extern void		LSLAbstractUnion(LSLToken loc);
extern void		LSLAbstractField(void);
extern void		LSLAbstractIncludes(LSLToken loc);
extern void		LSLAbstractAssumes(LSLToken loc);
extern void		LSLAbstractTraitRef(LSLRuleCode rule, LSLToken loc);
extern void		LSLAbstractReplace(LSLRuleCode rule);
extern void		LSLAbstractOpPart(LSLToken loc);
extern void		LSLAbstractOpDcl(void);
extern void		LSLAbstractPropPart(LSLToken loc);
extern void		LSLAbstractGenBy(LSLToken loc, bool freely);
extern void		LSLAbstractPartBy(LSLToken loc);
extern void		LSLAbstractOperator(LSLRuleCode rule);
extern void		LSLAbstractQuantEqns(void);
extern void		LSLAbstractVarDcl(void);
extern void		LSLAbstractEquation(LSLRuleCode rule);
extern void		LSLAbstractTerm(LSLRuleCode rule, LSLToken op);
extern void		LSLAbstractConsequences(LSLToken loc);
extern void		LSLAbstractConversion(LSLRuleCode rule, LSLToken loc);
extern void		LSLAbstractExemption(LSLRuleCode rule, LSLToken loc);

extern void		LSLAbstractSeparatorList(void);
extern void		LSLAbstractSeparatorListExtend(void);
extern void		LSLAbstractNameList(void);
extern void		LSLAbstractNameListExtend(void);
extern void		LSLAbstractTermList(void);
extern void		LSLAbstractTermListExtend(void);
extern void		LSLAbstractArgList(void);
extern void		LSLAbstractArgListExtend(void);
extern void		LSLAbstractEquationList(void);
extern void		LSLAbstractEquationListExtend(void);
extern void		LSLAbstractVarDclList(void);
extern void		LSLAbstractVarDclListExtend(void);
extern void		LSLAbstractQuantEqnsList(void);
extern void		LSLAbstractQuantEqnsListExtend(void);
extern void		LSLAbstractOperatorList(void);
extern void		LSLAbstractOperatorListExtend(void);
extern void		LSLAbstractGenPartByList(void);
extern void		LSLAbstractGenPartByListExtend(void);
extern void		LSLAbstractReplaceList(void);
extern void		LSLAbstractReplaceListExtend(void);
extern void		LSLAbstractTraitRefList(void);
extern void		LSLAbstractTraitRefListExtend(void);
extern void		LSLAbstractConversionList(void);
extern void		LSLAbstractConversionListExtend(void);
extern void		LSLAbstractPropPartList(void);
extern void		LSLAbstractPropPartListExtend(void);
extern void		LSLAbstractOpDclList(void);
extern void		LSLAbstractOpDclListExtend(void);
extern void		LSLAbstractOpPartList(void);
extern void		LSLAbstractOpPartListExtend(void);
extern void		LSLAbstractFieldList(void);
extern void		LSLAbstractFieldListExtend(void);
extern void		LSLAbstractExtList(void);
extern void		LSLAbstractExtListExtend(void);
extern void		LSLAbstractParameterList(void);
extern void		LSLAbstractParameterListExtend(void);
extern void		LSLAbstractSortList(void);
extern void		LSLAbstractSortListExtend(void);

extern void		LSLAbstractElementIdList(void);
extern void		LSLAbstractElementIdListExtend(void);
extern void		LSLAbstractFieldIdList(void);
extern void		LSLAbstractFieldIdListExtend(void);
extern void		LSLAbstractTraitIdList(void);
extern void		LSLAbstractTraitIdListExtend(void);
extern void		LSLAbstractVarIdList(void);
extern void		LSLAbstractVarIdListExtend(void);


/*
** Constructors for abstract syntax nodes
*/

extern traitNode	*makeDummyTraitNode(void);
extern sortNode		*makeBoolSort(void);
extern sortNode		*makeSortNode(sortIdLeaf *s, sortNodeList *subsorts);
extern signatureNode	*makeSignature(sortNodeList *domain, sortNode *range);
extern signatureNode	*makeSignature0(sortNode *range);
extern signatureNode	*makeSignature1(sortNode *dom1, sortNode *range);
extern signatureNode	*makeSignature2(sortNode *dom1, sortNode *dom2, 
					sortNode *range);
extern signatureNode	*makeSignature3(sortNode *dom1, sortNode *dom2, 
					sortNode *dom3, sortNode *range);
extern operatorNode	*makeOperatorNode(nameNode *name);
extern termNode		*makeTermNode(nameNode *root, termNodeList *args);
extern equationNode	*makeEquationNode(termNode *left, termNode *right);
extern genByNode	*makeGenByNode(sortNode *s, bool freely, OpList *ops, 
				       LSLToken loc);
extern partByNode     	*makePartByNode(sortNode *s, OpList *ops, 
					LSLToken loc);
extern nameNode		*makeFalseOpNode(void);
extern nameNode		*makeTrueOpNode(void);
extern nameNode		*makeAllOpNode(void);
extern nameNode		*makeExistsOpNode(void);
extern nameNode		*makeEqOpNode(void);
extern nameNode		*makeNeqOpNode(void);
extern nameNode		*makeAndOpNode(void);
extern nameNode		*makeOrOpNode(void);
extern nameNode		*makeNotOpNode(void);
extern nameNode		*makeImpliesOpNode(void);
extern nameNode		*makeIffOpNode(void);
extern nameNode		*makeIfOpNode(void);
extern nameNode		*makeMixfixOpNameNode(bool before, openSymLeaf *open, 
					      separatorNodeList *seps, 
					      closeSymLeaf *close, bool after);
extern nameNode		*makePpiOpNameNode(bool before, anyOpNode *op, 
					   bool after);
extern anyOpNode	*makeSimpleAnyOpNode(simpleOpLeaf *op);
extern nameNode		*makeSelectOpNameNode(simpleIdLeaf *s);
extern nameNode		*makeSimpleIdNameNode(simpleIdLeaf *s);
extern enumerationNode	*makeEnumerationNode(sortNode *s, 
					     elementIdLeafList *elems,
					     LSLToken loc);
extern separatorNode	*makeSeparatorNode(sepSymLeaf *s);


/*
** Observers for abstract syntax nodes
*/

extern int		nameNodeArity(nameNode *n);
extern bool		nameNodeMatches(nameNode *n1, nameNode *n2,
					bool implicitMarkers);
extern int		signatureNodeArity(signatureNode *s);
extern bool		LSLSameLeaf(LSLToken *t1, LSLToken *t2);
extern bool		LSLSameSignature(signatureNode *s1, signatureNode *s2);
extern bool		LSLSameSort(sortNode *s1, sortNode *s2);
extern bool		LSLSubsort(sortNode *s1, sortNode *s2);


/*
** Abstract interface to abstract syntax (redefined as macros)
*/

# define		 node_key(pn)					\
    				((pn)->key)
# define		 node_loc(pn)					\
    				((pn)->loc)
# define		 node_exists(pn)				\
    				((pn) != 0)
# define		 node_null(pn)					\
    				((pn) == 0)

extern bool		 selectOpNode_before(selectOpNode *n);
# define		 selectOpNode_before(pn)			\
				((pn)->body->before)
extern simpleIdLeaf	*selectOpNode_sel(selectOpNode *n);
# define		 selectOpNode_sel(pn)				\
				((pn)->body->sel)

extern sepSymLeaf	*separatorNode_separator(separatorNode *n);
# define		 separatorNode_separator(pn)			\
				((pn)->body->separator)

extern anyOpKind	 anyOpNode_kind(anyOpNode *n);
# define		 anyOpNode_kind(pn)				\
				((pn)->body->kind)
extern simpleOpLeaf	*anyOpNode_simpleOp(anyOpNode *n);
# define 		 anyOpNode_simpleOp(pn)	    			\
				((pn)->body->choice.simpleOp)
extern logicalOpLeaf	*anyOpNode_logicalOp(anyOpNode *n);
# define		 anyOpNode_logicalOp(pn)			\
				((pn)->body->choice.logicalOp)
extern eqOpLeaf		*anyOpNode_eqOp(anyOpNode *n);
# define		 anyOpNode_eqOp(pn)				\
				((pn)->body->choice.eqOp)

extern bool		  mixfixOpNode_before(mixfixOpNode *n);
# define		  mixfixOpNode_before(pn)			\
				((pn)->body->before)
extern openSymLeaf	 *mixfixOpNode_open(mixfixOpNode *n);
# define		  mixfixOpNode_open(pn)			\
				((pn)->body->open)
extern separatorNodeList *mixfixOpNode_seps(mixfixOpNode *n);
# define		  mixfixOpNode_seps(pn)			\
				((pn)->body->seps)
extern closeSymLeaf	 *mixfixOpNode_close(mixfixOpNode *n);
# define		  mixfixOpNode_close(pn)			\
				((pn)->body->close)
extern bool		  mixfixOpNode_after(mixfixOpNode *n);
# define		  mixfixOpNode_after(pn)			\
				((pn)->body->after)

extern bool		 ppiOpNode_before(ppiOpNode *n);
# define		 ppiOpNode_before(pn)				\
				((pn)->body->before)
extern anyOpNode	*ppiOpNode_op(ppiOpNode *n);
# define		 ppiOpNode_op(pn)				\
				((pn)->body->op)
extern bool		 ppiOpNode_after(ppiOpNode *n);
# define		 ppiOpNode_after(pn)				\
				((pn)->body->after)

extern nameKind		 nameNode_kind(nameNode *n);
# define		 nameNode_kind(pn)				\
				((pn)->body->kind)
extern quantifierLeaf	*nameNode_quantifier(nameNode *n);
# define		 nameNode_quantifier(pn)			\
				((pn)->body->choice.quantifier)
extern simpleIdLeaf	*nameNode_simpleId(nameNode *n);
# define		 nameNode_simpleId(pn)				\
				((pn)->body->choice.simpleId)
extern ifOpNode		*nameNode_ifOp(nameNode *n);
# define		 nameNode_ifOp(pn)				\
				((pn)->body->choice.ifOp)
extern ppiOpNode	*nameNode_ppiOp(nameNode *n);
# define		 nameNode_ppiOp(pn)				\
				((pn)->body->choice.ppiOp)
extern mixfixOpNode	*nameNode_mixfixOp(nameNode *n);
# define		 nameNode_mixfixOp(pn)				\
				((pn)->body->choice.mixfixOp)
extern selectOpNode	*nameNode_selectOp(nameNode *n);
# define		 nameNode_selectOp(pn)				\
				((pn)->body->choice.selectOp)

extern bool		*nameNode_selectOpBefore(nameNode *n);
# define		 nameNode_selectOpBefore(pn)			\
				(nameNode_selectOp(pn)->body->before)

extern nameNode		*termNode_varOrOp(termNode *n);
# define		 termNode_varOrOp(pn)				\
				((pn)->body->varOrOp)
extern termNodeList	*termNode_args(termNode *n);
# define		 termNode_args(pn)				\
				((pn)->body->args)
extern sortNode		*termNode_qual(termNode *n);
# define		 termNode_qual(pn)				\
				((pn)->body->qual)

extern termNode		*equationNode_left(equationNode *n);
# define		 equationNode_left(pn)				\
				((pn)->body->left)
extern termNode		*equationNode_right(equationNode *n);
# define		 equationNode_right(pn)				\
				((pn)->body->right)

extern varIdLeafList	*varDclNode_vars(varDclNode *n);
# define		 varDclNode_vars(pn)				\
				((pn)->body->vars)
extern sortNode		*varDclNode_sort(varDclNode *n);
# define		 varDclNode_sort(pn)				\
				((pn)->body->sort)

extern varDclNodeList	*quantEqnsNode_quantifier(quantEqnsNode *n);
# define		 quantEqnsNode_quantifier(pn)			\
				((pn)->body->quantifier)
extern equationNodeList	*quantEqnsNode_equations(quantEqnsNode *n);
# define		 quantEqnsNode_equations(pn)			\
				((pn)->body->equations)

extern sortIdLeaf	*sortNode_id(sortNode *n);
# define		 sortNode_id(pn)				\
    				((pn)->body->id)

extern sortNodeList	*sortNode_subsorts(sortNode *n);
# define		 sortNode_subsorts(pn)				\
    				((pn)->body->subsorts)

extern sortNodeList	*signatureNode_domain(signatureNode *n);
# define		 signatureNode_domain(pn)			\
				((pn)->body->domain)
extern sortNode		*signatureNode_range(signatureNode *n);
# define		 signatureNode_range(pn)			\
				((pn)->body->range)

extern nameNode		*operatorNode_name(operatorNode *n);
# define		 operatorNode_name(pn)				\
				((pn)->body->name)
extern signatureNode	*operatorNode_signature(operatorNode *n);
# define		 operatorNode_signature(pn)			\
				((pn)->body->signature)

extern sortNode		*genByNode_sort(genByNode *n);
# define		 genByNode_sort(pn)				\
				((pn)->body->sort)
extern bool		 genByNode_freely(genByNode *n);
# define		 genByNode_freely(pn)				\
				((pn)->body->freely)
extern operatorNodeList	*genByNode_ops(genByNode *n);
# define		 genByNode_ops(pn)				\
				((pn)->body->ops)

extern sortNode		*partByNode_sort(partByNode *n);
# define		 partByNode_sort(pn)				\
				((pn)->body->sort)
extern operatorNodeList	*partByNode_ops(partByNode *n);
# define		 partByNode_ops(pn)				\
				((pn)->body->ops)

extern genPartByKind	 genPartByNode_kind(genPartByNode *n);
# define		 genPartByNode_kind(pn)				\
				((pn)->body->kind)
extern genByNode	*genPartByNode_genBy(genPartByNode *n);
# define		 genPartByNode_genBy(pn)			\
				((pn)->body->choice.genBy)
extern partByNode	*genPartByNode_partBy(genPartByNode *n);
# define		 genPartByNode_partBy(pn)			\
				((pn)->body->choice.partBy)

extern parameterNode	*replaceNode_actual(replaceNode *n);
# define		 replaceNode_actual(pn)				\
				((pn)->body->actual)
extern parameterNode	*replaceNode_formal(replaceNode *n);
# define		 replaceNode_formal(pn)				\
				((pn)->body->formal)
extern signatureNode	*replaceNode_qual(replaceNode *n);
# define		 replaceNode_qual(pn)				\
				((pn)->body->qual)

extern traitIdLeafList	*traitRefNode_traits(traitRefNode *n);
# define		 traitRefNode_traits(pn)			\
				((pn)->body->traits)
extern parameterNodeList *traitRefNode_actuals(traitRefNode *n);
# define		 traitRefNode_actuals(pn)			\
				((pn)->body->actuals)
extern replaceNodeList	*traitRefNode_renamings(traitRefNode *n);
# define		 traitRefNode_renamings(pn)			\
				((pn)->body->renamings)

extern varDclNodeList	*exemptionNode_forall(exemptionNode *n);
# define		 exemptionNode_forall(pn)			\
				((pn)->body->forall)
extern termNodeList	*exemptionNode_terms(exemptionNode *n);
# define		 exemptionNode_terms(pn)			\
				((pn)->body->terms)

extern operatorNodeList	*conversionNode_converts(conversionNode *n);
# define		 conversionNode_converts(pn)			\
				((pn)->body->converts)
extern exemptionNode	*conversionNode_exempts(conversionNode *n);
# define		 conversionNode_exempts(pn)			\
				((pn)->body->exempts)

extern traitRefNodeList	  *consequencesNode_traits(consequencesNode *n);
# define		   consequencesNode_traits(pn)			\
				((pn)->body->traits)
extern genPartByNodeList  *consequencesNode_genPartBys(consequencesNode *n);
# define		   consequencesNode_genPartBys(pn)		\
				((pn)->body->genPartBys)
extern equationNodeList	  *consequencesNode_eqns(consequencesNode *n);
# define		   consequencesNode_eqns(pn)			\
				((pn)->body->eqns)
extern quantEqnsNodeList  *consequencesNode_quantEqns(consequencesNode *n);
# define		   consequencesNode_quantEqns(pn)		\
				((pn)->body->quantEqns)
extern conversionNodeList *consequencesNode_converts(consequencesNode *n);
# define		   consequencesNode_converts(pn)		\
				((pn)->body->converts)

extern genPartByNodeList *propPartNode_genPartBys(propPartNode *n);
# define		  propPartNode_genPartBys(pn)			\
				((pn)->body->genPartBys)
extern equationNodeList	 *propPartNode_equations(propPartNode *n);
# define		  propPartNode_equations(pn)			\
				((pn)->body->equations)
extern quantEqnsNodeList *propPartNode_quantEqns(propPartNode *n);
# define		  propPartNode_quantEqns(pn)			\
				((pn)->body->quantEqns)

extern nameNodeList	*opDclNode_ops(opDclNode *n);
# define		 opDclNode_ops(pn)				\
				((pn)->body->ops)
extern signatureNode	*opDclNode_signature(opDclNode *n);
# define		 opDclNode_signature(pn)			\
				((pn)->body->signature)

extern opDclNodeList	*opPartNode_introduces(opPartNode *n);
# define		 opPartNode_introduces(pn)			\
				((pn)->body->introduces)

extern traitRefNodeList	*assumesNode_traits(assumesNode *n);
# define		 assumesNode_traits(pn)				\
				((pn)->body->traits)

extern traitRefNodeList	*includesNode_traits(includesNode *n);
# define		 includesNode_traits(pn)			\
				((pn)->body->traits)

extern fieldIdLeafList	*fieldNode_fields(fieldNode *n);
# define		 fieldNode_fields(pn)				\
				((pn)->body->fields)
extern sortNode		*fieldNode_sort(fieldNode *n);
# define		 fieldNode_sort(pn)				\
				((pn)->body->sort)

extern sortNode		*tupleNode_sort(tupleNode *n);
# define		 tupleNode_sort(pn)				\
				((pn)->body->sort)
extern fieldNodeList	*tupleNode_fields(tupleNode *n);
# define		 tupleNode_fields(pn)				\
				((pn)->body->fields)

extern sortNode		*unionNode_sort(unionNode *n);
# define		 unionNode_sort(pn)				\
				((pn)->body->sort)
extern fieldNodeList	*unionNode_fields(unionNode *n);
# define		 unionNode_fields(pn)				\
				((pn)->body->fields)

extern sortNode	 	*enumerationNode_sort(enumerationNode *n);
# define		 enumerationNode_sort(pn)			\
				((pn)->body->sort)
extern elementIdLeafList *enumerationNode_elems(enumerationNode *n);
# define		  enumerationNode_elems(pn)			\
				((pn)->body->elems)

extern extKind		 extNode_kind(extNode *n);
# define		 extNode_kind(pn)				\
				((pn)->body->kind)
extern enumerationNode	*extNode_LSLenum(extNode *n);
# define		 extNode_LSLenum(pn)				\
				((pn)->body->choice.LSLenum)
extern tupleNode	*extNode_LSLtuple(extNode *n);
# define		 extNode_LSLtuple(pn)				\
				((pn)->body->choice.LSLtuple)
extern unionNode	*extNode_LSLunion(extNode *n);
# define		 extNode_LSLunion(pn)				\
				((pn)->body->choice.LSLunion)
extern includesNode	*extNode_includes(extNode *n);
# define		 extNode_includes(pn)				\
				((pn)->body->choice.includes)
extern assumesNode	*extNode_assumes(extNode *n);
# define		 extNode_assumes(pn)				\
				((pn)->body->choice.assumes)

extern parameterKind	 parameterNode_kind(parameterNode *n);
# define		 parameterNode_kind(pn)				\
				((pn)->body->kind)
extern sortNode		*parameterNode_sort(parameterNode *n);
# define		 parameterNode_sort(pn)				\
				((pn)->body->choice.sort)
extern operatorNode	*parameterNode_operator(parameterNode *n);
# define		 parameterNode_operator(pn)			\
				((pn)->body->choice.operator)
extern simpleIdLeaf	*parameterNode_simpleId(parameterNode *n);
# define		 parameterNode_simpleId(pn)			\
				((pn)->body->choice.simpleId)

extern traitIdLeaf	*traitNode_name(traitNode *n);
# define 		 traitNode_name(pn) 				\
				((pn)->body->name)
extern parameterNodeList *traitNode_formals(traitNode *n);
# define 		 traitNode_formals(pn) 				\
	    			((pn)->body->formals)
extern extNodeList	*traitNode_exts(traitNode *n);
# define 		 traitNode_exts(pn) 				\
				((pn)->body->exts)
extern opPartNodeList	*traitNode_introduces(traitNode *n);
# define 		 traitNode_introduces(pn) 			\
   				((pn)->body->introduces)
extern propPartNodeList	*traitNode_asserts(traitNode *n);
# define 		 traitNode_asserts(pn) 				\
	    			((pn)->body->asserts)
extern consequencesNode	*traitNode_implies(traitNode *n);
# define 		 traitNode_implies(pn) 				\
				((pn)->body->implies)
extern bool		 traitNode_commandLineArg(traitNode *n);
# define	         traitNode_commandLineArg(pn)			\
				((pn)->body->commandLineArg)
    


/*
** Initialization and cleanup
*/

extern void		LSLAbstractInit(void);
extern void		LSLAbstractReset(void);
extern void		LSLAbstractCleanup(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
