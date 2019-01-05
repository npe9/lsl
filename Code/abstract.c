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
**      FILENAME:   abstract.c
**
**	PURPOSE:    Builds abstract syntax tree
**	
**	Reduce actions during parsing invoke routines in this module to
**	build an abstract syntax tree from the bottom up.  During the
**	build, nodes for the tree are kept on a stack.
**
**	!!! CLEAN-UP: Break abstract.c into two pieces: constructors for nodes
**	!!! in the abstract syntax tree (which are generally useful procedures)
**	!!! and routines to be invoked from reduce.c (which checking.c does not
**	!!! need).
**	
**  MODIFICATION HISTORY:
**
**      {0} Garland  at MIT     -- 90.09.13 -- Original
**	{1} Garland  at MIT     -- 90.10.10 -- Add locators
**					       Move PopShiftStack to reduce.c
**	{2} Wild     at Digital -- 90.11.09 -- Change ## to PASTE().
**	{3} Wild     at Digital -- 90.11.09 -- Fix actual for multi-traitref
**					       error reporting.
**	{4} Wild     at Digital -- 91.02.20 -- Merge in MIT changes.
**	{5} Feldman  at Digital -- 91.03.20 -- Remove gratuitous PASTEs of the
**					       form PASTE(id1., id2).  Because
**					       of the . after id1, the PASTE is
**					       unnecessary.  The C standard
**					       appears to say that the behavior
**					       is undefined.  gcc accepts this,
**					       but DEC C doesn't.  Since the
**					       PASTE is unnecessary, we can 
**					       just get rid of it.
**	{6} Garland  at MIT	-- 91.12.20 -- Add several make*Node for lsl2lp
**	{7} Garland  at MIT     -- 92.01.09 -- Add hash key to nodes
**	{8} Garland  at MIT     -- 92.01.23 -- Add LSLAbstractParsedReplace
**      {9} Garland  at MIT     -- 92.02.19 -- Reorder #include "error.h"
**     {10} Garland  at MIT     -- 92.02.23 -- Change nameNodeMatches
**     {11} Garland  at MIT     -- 92.06.14 -- Composite sort notation A$B
**     {12} Garland  at MIT     -- 93.01.07 -- Set commandLineArg in traitBody
**     {13} Garland  at MIT     -- 93.11.09 -- Add quantified formulas
**     {14} Garland  at MIT     -- 93.11.26 -- Add iff, priority
**     {15} Garland  at MIT     -- 94.04.20 -- Add generated freely
**     {16} Garland  at MIT     -- 94.05.23 -- Composite sort notation A[B, C]
**     {17} Garland  at MIT	-- 94.05.27 -- Redo LSLAbstractFormal
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include		"general.h"
# include		"osdfiles.h"
# include		"string2sym.h"
# include		"token.h"
# include		"error.h"
# include		"parse.h"
# include		"list.h"
# include		"tokentable.h"
# include		"abstract.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define CompilerFailure	error_programFailure

# define STACKMAX	100

# define subNode(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, field, 					\
		PASTE(fieldKind, NODE), PASTE(fieldKind, Node))

# define subLeaf(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, field, 					\
		PASTE(fieldKind, LEAF), PASTE(fieldKind, Leaf))

# define subNodeList(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, field, 					\
		PASTE(fieldKind, NODELIST), PASTE(fieldKind, NodeList))

# define subLeafList(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, field, 					\
		PASTE(fieldKind, LEAFLIST), PASTE(fieldKind, LeafList))

# define subunionNode(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, choice.field, 				\
		PASTE(fieldKind, NODE), PASTE(fieldKind, Node));	\
    node->body->kind = PASTE(field, KIND);				\
    node->loc = PASTE(PASTE(nodeKind, Node_), field(node)->loc)

# define subunionLeaf(node, nodeKind, field, fieldKind)			\
    subNodeImpl(node, nodeKind, choice.field, 				\
		PASTE(fieldKind, LEAF), PASTE(fieldKind, Leaf));	\
    node->body->kind = PASTE(field, KIND);				\
    node->loc = *(PASTE(PASTE(nodeKind, Node_), field(node)))

# define subNodeImpl(node, nodeKind, field, tag, fieldType)		\
    if (top().kind != tag) {						\
	CompilerFailure(STR(abstract.c: bad nodeKind -> field));	\
	}								\
    node->body->field = (fieldType *)(popTop().ptr)

# define dummy(nodeKind)	push(nodeKind, (void *)0)

# define allocNode(nodeKind, node)					\
    node = (PASTE(nodeKind, Node) *)	LSLMoreMem((void *)0,		\
					 sizeof(PASTE(nodeKind, Node)));\
    node->key = 0;							\
    node->body = (PASTE(nodeKind, Body) *)				\
                   LSLMoreMem((void *)0, sizeof(PASTE(nodeKind, Body)))

# define makeNodeList(x) 	makeList(PASTE(x, Node), PASTE(x, NODE))

# define makeLeafList(x)	makeList(PASTE(x, Leaf), PASTE(x, LEAF))

# define makeList(t, tag)						\
    list(t) *n = list_new(t);						\
    if (top().kind != tag) {						\
	CompilerFailure(STR(abstract.c: bad start for PASTE(t, List)));	\
    }									\
    list_addh(t, n, (t *)popTop().ptr);					\
    push(PASTE(tag, LIST), (void *)n)

# define extendNodeList(x)	extendList(PASTE(x, Node), PASTE(x, NODE))

# define extendLeafList(x)	extendList(PASTE(x, Leaf), PASTE(x, LEAF))

# define extendList(t, tag)						\
    t *n;								\
    if (top().kind != tag) {						\
	CompilerFailure(STR(abstract.c: bad extension for PASTE(t, List)));\
    }									\
    n = (t *)(popTop().ptr);						\
    if (top().kind != PASTE(tag, LIST)) {				\
	CompilerFailure(STR(abstract.c: no PASTE(t, List) to extend));	\
    }									\
    list_addh(t, (PASTE(t, List) *)top().ptr, n)

# define setLoc(n, t)							\
    n->loc = t


/*
**
**  TYPEDEFS
**
*/

typedef struct {
    nodeKind	kind;
    void	*ptr;
} nodeRecord;


/*
**
**  FORWARD FUNCTIONS
**
*/

extern traitNode       *LSLAbstractParseTree();
extern replaceNode     *LSLAbstractParsedReplace();

extern void		LSLAbstractMakeLeafProc();

extern void		LSLAbstractTrait();
extern void		LSLAbstractFormal();
extern void		LSLAbstractActual();
extern void		LSLAbstractName();
extern void		LSLAbstractIfOp();
extern void		LSLAbstractPpiOp();
extern void		LSLAbstractMixfixOp();
extern void		LSLAbstractSelectOp();
extern void		LSLAbstractAnyOp();
extern void		LSLAbstractSeparator();
extern void		LSLAbstractSignature();
extern void		LSLAbstractSort();
extern void		LSLAbstractExt();
extern void		LSLAbstractEnumeration();
extern void		LSLAbstractTuple();
extern void		LSLAbstractUnion();
extern void		LSLAbstractField();
extern void		LSLAbstractIncludes();
extern void		LSLAbstractAssumes();
extern void		LSLAbstractTraitRef();
extern void		LSLAbstractReplace();
extern void		LSLAbstractOpPart();
extern void		LSLAbstractOpDcl();
extern void		LSLAbstractPropPart();
extern void		LSLAbstractGenBy();
extern void		LSLAbstractPartBy();
extern void		LSLAbstractOperator();
extern void		LSLAbstractQuantEqns();
extern void		LSLAbstractVarDcl();
extern void		LSLAbstractEquation();
extern void		LSLAbstractTerm();
extern void		LSLAbstractConsequences();
extern void		LSLAbstractConversion();
extern void		LSLAbstractExemption();

extern void		LSLAbstractSeparatorList();
extern void		LSLAbstractSeparatorListExtend();
extern void		LSLAbstractNameList();
extern void		LSLAbstractNameListExtend();
extern void		LSLAbstractTermList();
extern void		LSLAbstractTermListExtend();
extern void		LSLAbstractArgList();
extern void		LSLAbstractArgListExtend();
extern void		LSLAbstractEquationList();
extern void		LSLAbstractEquationListExtend();
extern void		LSLAbstractVarDclList();
extern void		LSLAbstractVarDclListExtend();
extern void		LSLAbstractQuantEqnsList();
extern void		LSLAbstractQuantEqnsListExtend();
extern void		LSLAbstractOperatorList();
extern void		LSLAbstractOperatorListExtend();
extern void		LSLAbstractGenPartByList();
extern void		LSLAbstractGenPartByListExtend();
extern void		LSLAbstractReplaceList();
extern void		LSLAbstractReplaceListExtend();
extern void		LSLAbstractTraitRefList();
extern void		LSLAbstractTraitRefListExtend();
extern void		LSLAbstractConversionList();
extern void		LSLAbstractConversionListExtend();
extern void		LSLAbstractPropPartList();
extern void		LSLAbstractPropPartListExtend();
extern void		LSLAbstractOpDclList();
extern void		LSLAbstractOpDclListExtend();
extern void		LSLAbstractOpPartList();
extern void		LSLAbstractOpPartListExtend();
extern void		LSLAbstractFieldList();
extern void		LSLAbstractFieldListExtend();
extern void		LSLAbstractExtList();
extern void		LSLAbstractExtListExtend();
extern void		LSLAbstractParameterList();
extern void		LSLAbstractParameterListExtend();
extern void		LSLAbstractSortList();
extern void		LSLAbstractSortListExtend();

extern void		LSLAbstractElementIdList();
extern void		LSLAbstractElementIdListExtend();
extern void		LSLAbstractFieldIdList();
extern void		LSLAbstractFieldIdListExtend();
extern void		LSLAbstractTraitIdList();
extern void		LSLAbstractTraitIdListExtend();
extern void		LSLAbstractVarIdList();
extern void		LSLAbstractVarIdListExtend();

extern traitNode	*makeDummyTraitNode();
extern sortNode		*makeBoolSort();
extern sortNode		*makeSortNode();
extern signatureNode	*makeSignature();
extern signatureNode	*makeSignature0();
extern signatureNode	*makeSignature1();
extern signatureNode	*makeSignature2();
extern signatureNode	*makeSignature3();
extern operatorNode	*makeOperatorNode();
extern termNode		*makeTermNode();
extern equationNode	*makeEquationNode();
extern enumerationNode	*makeEnumerationNode();
extern genByNode	*makeGenByNode();
extern partByNode	*makePartByNode();
extern nameNode		*makeFalseOpNode();
extern nameNode		*makeTrueOpNode();
extern nameNode		*makeAllOpNode();
extern nameNode		*makeExistsOpNode();
extern nameNode		*makeEqOpNode();
extern nameNode		*makeNeqOpNode();
extern nameNode		*makeAndOpNode();
extern nameNode		*makeOrOpNode();
extern nameNode		*makeNotOpNode();
extern nameNode		*makeImpliesOpNode();
extern nameNode		*makeIffOpNode();
extern nameNode		*makeIfOpNode();
extern anyOpNode	*makeSimpleAnyOpNode();
extern nameNode		*makeSimpleIdNameNode();
extern nameNode		*makeSelectOpNameNode();
extern nameNode		*makeMixfixOpNameNode();
extern enumerationNode	*makeEnumerationNode();
extern separatorNode	*makeSeparatorNode();
extern int		nameNodeArity();
extern bool		nameNodeMatches();
extern int		signatureNodeArity();
extern bool		LSLSameLeaf();
static bool		sameAnyOp(anyOpNode *n1, anyOpNode *n2);
static bool		samePpiOp(ppiOpNode *n1, ppiOpNode *n2,
				  bool implicitMarkers);
static bool		sameSeparator(separatorNode *n1, separatorNode *n2);
static bool		sameMixfixOp(mixfixOpNode *n1, mixfixOpNode *n2);
static bool		sameSelectOp(selectOpNode *n1, selectOpNode *n2,
    				     bool implicitMarker);
extern bool		LSLSameSignature();
extern bool		LSLSameSort();
extern bool		LSLSubsort();

static void		insertArg();
static void		push();
static nodeRecord	top();
static nodeRecord	popTop();
static void		pop();
static unsigned int	leaf_key (LSLToken *t);
static unsigned int	ops_key (operatorNodeList *ops);
static unsigned int     mash (unsigned int x, unsigned int y);
static void		UserError();

extern void		LSLAbstractInit();
extern void		LSLAbstractReset();
extern void		LSLAbstractCleanup();


/*
**
**  STATIC VARIABLES
**
*/

nodeRecord		Stack[STACKMAX];
unsigned int		stackIndex;

static sortNode		*theBoolSort;
static nameNode		*theFalseOpNode;
static nameNode		*theTrueOpNode;
static nameNode		*theEqOpNode;
static nameNode		*theNeqOpNode;
static nameNode		*theAndOpNode;
static nameNode		*theOrOpNode;
static nameNode		*theNotOpNode;
static nameNode		*theImpliesOpNode;
static nameNode		*theIfOpNode;
static nameNode		*theIffOpNode;
static nameNode		*theAllOpNode;
static nameNode		*theExistsOpNode;


/*
**
**  FUNCTION DEFINITIONS
**
*/


/*
** The following procedures are provided for use by reduce.c
*/

void
LSLAbstractMakeLeafProc(nodeKind k, LSLToken t) {
    LSLToken *pt = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
    *pt = t;
    push(k, pt);
}


traitNode	
*LSLAbstractParseTree(void) {
    if (stackIndex > 1 || top().kind != traitNODE) {
	CompilerFailure("abstract.c: bad stack after parse");
    }
   return (traitNode *)top().ptr;
}

extern replaceNode	
*LSLAbstractParsedReplace(void) {
    if (stackIndex > 1 || top().kind != replaceNODE) {
	CompilerFailure("abstract.c: bad stack after parse");
    }
   return (replaceNode *)top().ptr;
}


void
LSLAbstractTrait(LSLRuleCode rule, LSLToken loc) {
    traitNode *n;
    allocNode(trait, n);
    setLoc(n, loc);
    if (top().kind == consequencesNODE) {
	subNode(n, trait, implies, consequences);
    }
    if (top().kind == propPartNODELIST) {
	subNodeList(n, trait, asserts, propPart);
    }
    if (top().kind == opPartNODELIST) {
	subNodeList(n, trait, introduces, opPart);
    }
    if (top().kind == extNODELIST) {
	subNodeList(n, trait, exts, ext);
    }
    if (rule == TRAIT1) {
	dummy(parameterNODELIST);
    }
    subNodeList(n, traitNode, formals, parameter);
    subLeaf(n, trait, name, traitId);
    n->key = leaf_key(traitNode_name(n));
    n->body->commandLineArg = FALSE;
    push(traitNODE, (void *)n);
}

void
LSLAbstractFormal(LSLRuleCode rule) {
    parameterNode *n;
    allocNode(parameter, n);
    switch(rule) {
    case FORMAL1:
    case FORMAL2:
	subunionNode(n, parameter, operator, operator); 
	break;
    case FORMAL3:
	subunionNode(n, parameter, sort, sort);
	break;
      case FORMAL4:
	subunionLeaf(n, parameter, simpleId, simpleId);
	break;
    default:
	CompilerFailure("LSLAbstractFormal");
    }
    push(parameterNODE, (void *)n);
}

void
LSLAbstractActual(LSLRuleCode rule) {
    parameterNode *n;
    operatorNode *op;
    allocNode(parameter, n);
    switch(rule) {
    case ACTUAL1:
	allocNode(operator, op);
	subNode(op, operator, name, name);
	push(operatorNODE, (void *)op);
	subunionNode(n, parameter, operator, operator);
	break;
    case ACTUAL2:
	subunionNode(n, parameter, sort, sort);
	break;
    case ACTUAL3:
	subunionLeaf(n, parameter, simpleId, simpleId);
	break;
    default:
	CompilerFailure("LSLAbstractActual");
    }
    push(parameterNODE, (void *)n);
}


void
LSLAbstractName(LSLRuleCode rule) {
    nameNode *n;
    allocNode(name, n);
    switch(rule) {
    case NAME1:
        subunionLeaf(n, name, simpleId, simpleId);
	n->key = leaf_key(nameNode_simpleId(n));
	break;
    case OPFORM1:
        subunionNode(n, name, ifOp, ifOp);
	n->key = leaf_key(&IfToken);
	break;
    case OPFORM2:
        subunionNode(n, name, ppiOp, ppiOp);
	n->key = node_key(nameNode_ppiOp(n));
	break;
    case OPFORM6:
        subunionNode(n, name, mixfixOp, mixfixOp);
	n->key = node_key(nameNode_mixfixOp(n));
	break;
    case OPFORM10:
        subunionNode(n, name, selectOp, selectOp);
	n->key = node_key(nameNode_selectOp(n));
	break;
    default:
	CompilerFailure("LSLAbstractName");
    }
    push(nameNODE, (void *)n);
}

void
LSLAbstractIfOp(LSLToken loc) {
    ifOpNode *n;
    allocNode(ifOp, n);
    setLoc(n, loc);
    n->key = leaf_key(&IfToken);
    push(ifOpNODE, (void *)n);
    LSLAbstractName(OPFORM1);
}

void
LSLAbstractPpiOp(LSLRuleCode rule, LSLToken loc) {
    ppiOpNode *n;
    allocNode(ppiOp, n);
    subNode(n, ppiOp, op, anyOp);
    if (token_code(loc) != MARKERSYM) {
	loc = n->body->op->loc;
    }
    setLoc(n, loc);
    (n->body)->before = FALSE;
    (n->body)->after = FALSE;
    switch(rule) {
    case OPFORM2:
	break;
    case OPFORM3:
	n->body->before = TRUE;
	break;
    case OPFORM4:
	n->body->after = TRUE;
	break;
    case OPFORM5:
	n->body->before = TRUE;
	n->body->after = TRUE;
	break;
    default:
	CompilerFailure("LSLAbstractPPiOp");
    }
    /* NOTE: key does not take "before" and "after" into account so that */
    /* we can match name nodes modulo implicit markers.			 */
    n->key = node_key(ppiOpNode_op(n));
    push(ppiOpNODE, (void *)n);
    LSLAbstractName(OPFORM2);
}

void
LSLAbstractMixfixOp(LSLRuleCode rule, LSLToken open, LSLToken close, 
		    LSLToken loc) {
    mixfixOpNode      *n;
    separatorNodeList *seps;
    allocNode(mixfixOp, n);
    setLoc(n, loc);
    LSLAbstractMakeLeaf(closeSym, close);
    subLeaf(n, mixfixOp, close, closeSym);
    if (top().kind == separatorNODELIST) {
	subNodeList(n, mixfixOp, seps, separator);
    }
    LSLAbstractMakeLeaf(openSym, open);
    subLeaf(n, mixfixOp, open, openSym);
    n->body->before = FALSE;
    n->body->after = FALSE;
    switch(rule) {
    case OPFORM6:
	break;
    case OPFORM7:
	n->body->before = TRUE;
	break;
    case OPFORM8:
	n->body->after = TRUE;
	break;
    case OPFORM9:
	n->body->before = TRUE;
	n->body->after = TRUE; 
	break;
    default:
	CompilerFailure("LSLAbstractMixfixOp");
   }
    /* NOTE: key does not take "before" and "after" into account so that */
    /* we can match name nodes modulo implicit markers.			 */
    n->key = leaf_key(mixfixOpNode_open(n));
    seps = mixfixOpNode_seps(n);
    if (seps != (separatorNodeList *)0) {
	list_traverse(separatorNode, seps, s) {
	    n->key = mash(n->key, leaf_key(separatorNode_separator(s)));
	} end_traverse;
    }
    n->key =  mash(n->key, leaf_key(mixfixOpNode_close(n))); 
    push(mixfixOpNODE, (void *)n);
    LSLAbstractName(OPFORM6);
}

void
LSLAbstractSelectOp(LSLRuleCode rule, LSLToken op, LSLToken loc) {
    selectOpNode *n;
    allocNode(selectOp, n);
    setLoc(n, loc);
    LSLAbstractMakeLeaf(simpleId, op);
    subLeaf(n, selectOp, sel, simpleId);
    switch(rule) {
    case OPFORM10:
	n->body->before = FALSE;
	break;
    case OPFORM11:
	n->body->before = TRUE;
	break;
    default:
	CompilerFailure("LSLAbstractSelectOp");
    }
    n->key = leaf_key(selectOpNode_sel(n));
    push(selectOpNODE, (void *)n);
    LSLAbstractName(OPFORM10);
}

void
LSLAbstractAnyOp(LSLRuleCode rule, LSLToken op) {
    anyOpNode *n;
    allocNode(anyOp, n);
    setLoc(n, op);
    switch(rule) {
    case ANYOP1:
	LSLAbstractMakeLeaf(simpleOp, op);
        subunionLeaf(n, anyOp, simpleOp, simpleOp);
	n->key = leaf_key(anyOpNode_simpleOp(n));
	break;
    case ANYOP2:
	LSLAbstractMakeLeaf(logicalOp, op);
        subunionLeaf(n, anyOp, logicalOp, logicalOp);
	n->key = leaf_key(anyOpNode_logicalOp(n));
	break;
    case ANYOP3:
	LSLAbstractMakeLeaf(eqOp, op);
        subunionLeaf(n, anyOp, eqOp, eqOp);
	n->key = leaf_key(anyOpNode_eqOp(n));
	break;
    default:
	CompilerFailure("LSLAbstractAnyOp");
    }
    push(anyOpNODE, (void *)n);
}

void
LSLAbstractSeparator(LSLToken sep) {
    separatorNode *n;
    allocNode(separator, n);
    setLoc(n, sep);
    LSLAbstractMakeLeaf(sepSym, sep);
    subLeaf(n, separator, separator, sepSym);
    push(separatorNODE, (void *)n);
}


void
LSLAbstractSignature(LSLToken loc) {
    signatureNode *n;
    allocNode(signature, n);
    subNode(n, signature, range, sort);
    if (top().kind == sortNODELIST) {
	list_pointToFirst(sortNode, (sortNodeList *)top().ptr);
	n->loc = node_loc(list_current(sortNode, (sortNodeList *)top().ptr));
    } else {
	push(sortNODELIST, list_new(sortNode));
	setLoc(n, loc);					     /* `->' is loc */
    }
    subNodeList(n, signature, domain, sort);
    push(signatureNODE, (void *)n);
}


void
LSLAbstractSort(LSLRuleCode rule) {
    sortNode *sn;
    sortIdLeaf *s;
    allocNode(sort, sn);
    switch(rule) {
    case SORT1:
	sn->body->subsorts = (sortNodeList *)0;
	break;
    case SORT2:
	if (top().kind != sortNODELIST) {
	    CompilerFailure("LSLAbstractSort: missing sort list");
	}
	subNodeList(sn, sort, subsorts, sort);
	break;
      default:
	CompilerFailure("LSLAbstractSort: bad switch");
    }
    if (top().kind != sortIdLEAF) {	
	CompilerFailure("LSLAbstractSort: missing sortId");
    }
    s = (sortIdLeaf *)(top().ptr);
    popTop();
    sn->loc = *s;
    sn->body->id  = s;
    push(sortNODE, (void *)sn);
}


void
LSLAbstractExt(LSLRuleCode rule) {
    extNode *n;
    allocNode(ext, n);
    switch(rule) {
    case SHORTHAND1:
	subunionNode(n, ext, LSLenum, enumeration);
	break;
    case SHORTHAND2:
	subunionNode(n, ext, LSLtuple, tuple);
	break;
    case SHORTHAND3:
	subunionNode(n, ext, LSLunion, union);
	break;
    case EXTERNAL1:
	subunionNode(n, ext, includes, includes);
	break;
    case EXTERNAL2:
	subunionNode(n, ext, assumes, assumes);
	break;
    default:
	CompilerFailure("LSLAbstractExt");
    }
    push(extNODE, (void *)n);
}


void
LSLAbstractEnumeration(LSLToken loc) {
    enumerationNode *n;
    allocNode(enumeration, n);
    setLoc(n, loc);
    subLeafList(n, enumeration, elems, elementId);
    subNode(n, enumeration, sort, sort);
    push(enumerationNODE, (void *)n);
}


void
LSLAbstractTuple(LSLToken loc) {
    tupleNode *n;
    allocNode(tuple, n); 
    setLoc(n, loc);
    subNodeList(n, tuple, fields, field);
    subNode(n, tuple, sort, sort);
    push(tupleNODE, (void *)n);
}


void
LSLAbstractUnion(LSLToken loc) {
    unionNode *n;
    allocNode(union, n);
    setLoc(n, loc);
    subNodeList(n, union, fields, field);
    subNode(n, tuple, sort, sort);
    push(unionNODE, (void *)n);
}


void
LSLAbstractField(void) {
    fieldNode *n;
    allocNode(field, n);
    subNode(n, field, sort, sort);
    subLeafList(n, field, fields, fieldId);
    push(fieldNODE, (void *)n);
}


void
LSLAbstractIncludes(LSLToken loc) {
    includesNode *n;
    allocNode(includes, n);
    setLoc(n, loc);
    subNodeList(n, includes, traits, traitRef);
    push(includesNODE, (void *)n);
}


void
LSLAbstractAssumes(LSLToken loc) {
    assumesNode *n;
    allocNode(assumes, n);
    setLoc(n, loc);
    subNodeList(n, assumes, traits, traitRef);
    push(assumesNODE, (void *)n);
}


void
LSLAbstractTraitRef(LSLRuleCode rule, LSLToken loc) {
    parameterNode	 *tmp;
    traitRefNode *n;
    allocNode(traitRef, n);
    if (top().kind == replaceNODELIST) {
	subNodeList(n, traitRef, renamings, replace);
    }
    if (top().kind == parameterNODELIST) {
	subNodeList(n, traitRef, actuals, parameter);
    }
    switch(rule) {
    case TRAITREF1:
    case TRAITREF2:
	loc = *(traitIdLeaf *)top().ptr;
	LSLAbstractTraitIdList();
	break;
    case TRAITREF3:
    case TRAITREF4:
	break;
    default:
	CompilerFailure("LSLAbstractTraitRef");
    }
    setLoc(n, loc);
    subLeafList(n, traitRef, traits, traitId);
    push(traitRefNODE, (void *)n);
    if (node_exists(n->body->actuals) && 
	list_size(traitIdLeaf, n->body->traits) > 1) {
	list_pointToFirst(parameterNode, n->body->actuals);

	/* UserError takes a pointer to a LSLToken as the 1st parameter.    */
	/* Get the nameNode for the 1st actual and pass in the address of   */
	/* its LSLtoken. The LSLToken is a substructure of the nameNode	    */
	/* structure. */
	tmp = list_current(parameterNode, n->body->actuals);
	UserError(&tmp->loc,
		  "not expecting actuals for traitRef with multiple traits");
    }
}


void
LSLAbstractReplace(LSLRuleCode rule) {
    replaceNode *n;
    allocNode(replace, n);
    subNode(n, replace, formal, parameter);
    subNode(n, replace, actual, parameter);
    push(replaceNODE, (void *)n);
    n->loc = n->body->actual->loc;
}


void
LSLAbstractOpPart(LSLToken loc) {
    bool first;
    opPartNode *n;
    allocNode(opPart, n);
    setLoc(n, loc);
    subNodeList(n, opPart, introduces, opDcl);
    first = top().kind != opPartNODELIST;
    push(opPartNODE, (void *)n);
    if (first) {
	LSLAbstractOpPartList();
    } else {
	LSLAbstractOpPartListExtend();
    }
}


void
LSLAbstractOpDcl(void) {
    opDclNode *n;
    allocNode(opDcl, n);
    subNode(n, opDcl, signature, signature);
    subNodeList(n, opDcl, ops, name);
    push(opDclNODE, (void *)n);
}


void
LSLAbstractPropPart(LSLToken loc) {
    bool first;
    propPartNode *n;
    allocNode(propPart, n);
    setLoc(n, loc);
    if (top().kind == quantEqnsNODELIST) {
	subNodeList(n, propPart, quantEqns, quantEqns);
    }
    if (top().kind == equationNODELIST) {
	subNodeList(n, propPart, equations, equation);
    }
    if (top().kind == genPartByNODELIST) {
	subNodeList(n, propPart, genPartBys, genPartBy);
    }
    first = top().kind != propPartNODELIST;
    push(propPartNODE, (void *)n);
    if (first) {
	LSLAbstractPropPartList();
    } else {
	LSLAbstractPropPartListExtend();
    }
}


void
LSLAbstractGenBy(LSLToken loc, bool freely) {
    genByNode *n;
    genPartByNode *m;
    allocNode(genBy, n);
    setLoc(n, loc);
    allocNode(genPartBy, m);
    m->loc = n->loc;
    subNodeList(n, genBy, ops, operator);
    subNode(n, genBy, sort, sort);
    n->body->freely = freely;
    push(genByNODE, (void *)n);
    subunionNode(m, genPartBy, genBy, genBy);
    n->key = ops_key(genByNode_ops(n));
    push(genPartByNODE, (void *)m);
}


void
LSLAbstractPartBy(LSLToken loc) {
    partByNode *n;
    genPartByNode *m;
    allocNode(partBy, n);
    setLoc(n, loc);
    allocNode(genPartBy, m);
    m->loc = n->loc;
    subNodeList(n, partBy, ops, operator);
    subNode(n, partBy, sort, sort);
    push(partByNODE, (void *)n);
    subunionNode(m, genPartBy, partBy, partBy);
    n->key = ops_key(partByNode_ops(n));
    push(genPartByNODE, (void *)m);
}


void
LSLAbstractOperator(LSLRuleCode rule) {
    operatorNode *n;
    allocNode(operator, n);
    if (rule == OPERATOR2) {
	subNode(n, operator, signature, signature);
    }
    subNode(n, operator, name, name);
    push(operatorNODE, (void *)n);
    n->loc = n->body->name->loc;
}


void
LSLAbstractQuantEqns(void) {
    bool first;
    quantEqnsNode *n;
    allocNode(quantEqns, n);
    subNodeList(n, quantEqns, equations, equation);
    subNodeList(n, quantEqns, quantifier, varDcl);
    first = top().kind != quantEqnsNODELIST;
    push(quantEqnsNODE, (void *)n);
    if (first) {
	LSLAbstractQuantEqnsList();
    } else {
	LSLAbstractQuantEqnsListExtend();
    }
}


void
LSLAbstractVarDcl(void) {
    varDclNode *n;
    allocNode(varDcl, n);
    subNode(n, varDcl, sort, sort);
    subLeafList(n, varDcl, vars, varId);
    push(varDclNODE, (void *)n);
}


void
LSLAbstractEquation(LSLRuleCode rule) {
    equationNode *n;
    termNode *t;
    allocNode(equation, n);
    if (rule == EQUATION2) {
	subNode(n, equation, right, term);
    }
    subNode(n, equation, left, term);
    push(equationNODE, (void *)n);
    n->loc = n->body->left->loc;
    n->key = node_key(equationNode_left(n));
    t = equationNode_right(n);
    if (t != (termNode *)0) n->key = mash(n->key, node_key(t));
}


void
LSLAbstractTerm(LSLRuleCode rule, LSLToken op) {
    termNode *n;
    bool      needKey = TRUE;
    allocNode(term, n);
    switch(rule) {
    case TERM2:  				/* if t1 then t2 else t3 */
	setLoc(n, op);
	LSLAbstractIfOp(op);
	subNode(n, term, varOrOp, name);
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	insertArg(n->body->args);
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case LOGICALTERM2:				/* t1 logicalOp t2 	*/
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	LSLAbstractAnyOp(ANYOP2, op);
	LSLAbstractPpiOp(OPFORM5, op);
	subNode(n, term, varOrOp, name); 
	n->loc = ((termNode *)top().ptr)->loc;
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case LOGICALTERM3:				/* t1 eqOp t2 	     	*/
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	LSLAbstractAnyOp(ANYOP3, op);
	LSLAbstractPpiOp(OPFORM5, op);
	subNode(n, term, varOrOp, name);
	n->loc = ((termNode *)top().ptr)->loc;
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case PREFIXOPTERM2:				/* simpleOp t1       	*/
	setLoc(n, op);
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	LSLAbstractAnyOp(ANYOP1, op);
	LSLAbstractPpiOp(OPFORM4, op);
	subNode(n, term, varOrOp, name);
	push(termNODE, (void *)n);
	break;
    case POSTFIXOPS1:
    case POSTFIXOPS2: 				/* t1 simpleOp		*/
	n->body->args = list_new(termNode);
	LSLAbstractAnyOp(ANYOP1, op);
	LSLAbstractPpiOp(OPFORM3, op);
	subNode(n, term, varOrOp, name);
	n->loc = ((termNode *)top().ptr)->loc;
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case INFIXOPPART1: 
    case INFIXOPPART2: 				/* t1 simpleOp t2	*/
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	LSLAbstractAnyOp(ANYOP1, op);
	LSLAbstractPpiOp(OPFORM5, op);
	subNode(n, term, varOrOp, name);
	n->loc = ((termNode *)top().ptr)->loc;
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case SECONDARY3:				/* matched t2		*/
    case SECONDARY4:				/* t1 matched		*/
    case SECONDARY5:				/* t1 matched t2	*/
	{termNode *matched;
	 termNode *t1;
	 termNode *t2;
	 if (rule == SECONDARY3 || rule == SECONDARY5) {
	     if (top().kind != termNODE) {
		 CompilerFailure("LSLAbstractTerm: no term after bracketed");
	     }
	     t2 = (termNode *)(popTop().ptr);
	 }
	 if (top().kind != termNODE) {
	     CompilerFailure("LSLAbstractTerm: no bracketed term");
	 }
	 matched = (termNode *)(top().ptr);
	 if (matched->body->varOrOp->body->kind != mixfixOpKIND) {
	     CompilerFailure("LSLAbstractTerm: bad bracketed term");
	 }
	 if (rule == SECONDARY3 || rule == SECONDARY5) {
	     matched->body->varOrOp->body->choice.mixfixOp->body->after = TRUE;
	     list_addh(termNode, matched->body->args, t2);
	 }
	 if (rule == SECONDARY4 || rule == SECONDARY5) {
	     pop();
	     if (top().kind != termNODE) {
		 CompilerFailure("LSLAbstractTerm: no term before bracketed");
	     }
	     t1 = (termNode *)(popTop().ptr);
	     matched->body->varOrOp->body->choice.mixfixOp->body->before = TRUE;
	     list_addl(termNode, matched->body->args, t1);
	     push(termNODE, (void *)matched);
	     matched->loc = t1->loc;
	 }
	 needKey = FALSE;
     }
	break;
    case MATCHED1:				/* openSym args closeSym */
    case MATCHED2:				/* openSym closeSym	 */
	{mixfixOpNode      *m;
	 separatorNodeList *seps;
	 allocNode(mixfixOp, m);
	 setLoc(m, op);				      /* openSym	 */
	 subLeaf(m, mixfixOp, close, closeSym);	      /* already stacked */
	 if (top().kind == separatorNODELIST) {
	     list_reml(separatorNode, (separatorNodeList *)(top().ptr));
	     subNodeList(m, mixfixOp, seps, separator);
	 }
	 if (rule == MATCHED2) {
	     push(termNODELIST, (void *)list_new(term));
	 }
	 subNodeList(n, term, args, term);
	 LSLAbstractMakeLeaf(openSym, op);
	 subLeaf(m, mixfixOp, open, openSym);
	 m->body->before = FALSE;
	 m->body->after = FALSE;
	 push(mixfixOpNODE, (void *)m);
	 m->key = leaf_key(mixfixOpNode_open(m));
	 seps = mixfixOpNode_seps(m);
	 if (seps != (separatorNodeList *)0) {
	     list_traverse(separatorNode, seps, s) {
		 m->key = mash(m->key, leaf_key(separatorNode_separator(s)));
	     } end_traverse;
	 }
	 m->key =  mash(m->key, leaf_key(mixfixOpNode_close(m)));
	 LSLAbstractName(OPFORM6);
	 subNode(n, term, varOrOp, name);
     }
	push(termNODE, (void *)n);
	break;
    case PRIMARY5:
    case BRACKETED1:				/* t1: sort		*/
	{termNode *t1;
	 subNode(n, term, qual, sort);
	 if (top().kind != termNODE) {
	     CompilerFailure("LSLAbstractTerm: bad qualification");
	 }
	 t1 = (termNode *)(top().ptr);
	 if ((t1->body->qual != 0) &&
	     (!LSLSameSort(t1->body->qual, n->body->qual))) {
	     UserError(&node_loc(n->body->qual), 
		       "not expecting two different qualifications");
	 }
	 t1->body->qual = n->body->qual;
	 needKey = FALSE;
     }
	break;
    case PRIMARY3:				/* simpleId (termList)	*/
	subNodeList(n, term, args, term);
    case QUANTIFIEDVAR1:
    case PRIMARY2:				/* simpleId		*/
	setLoc(n, op);
	LSLAbstractMakeLeaf(simpleId, op);
	LSLAbstractName(NAME1);
	subNode(n, term, varOrOp, name);
	push(termNODE, (void *)n);
	break;
    case QUANTIFIEDVAR2:			/* simpleId : sort	*/
	setLoc(n, op);
	LSLAbstractMakeLeaf(simpleId, op);
	LSLAbstractName(NAME1);
	subNode(n, term, varOrOp, name);
	subNode(n, term, qual, sort);
	push(termNODE, (void *)n);
	break;
    case PRIMARY4:				/* t . simpleId		*/
  	subNode(n, term, varOrOp, name);
	n->body->args = list_new(termNode);
	n->loc = ((termNode *)top().ptr)->loc;
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    case PREFIXOPTERM3:				/* quantifiedVar prefixOpTerm */
	setLoc(n, op);
	if (LSLSameLeaf(&op, &AllToken)) {
	  push(nameNODE, (void *)makeAllOpNode());
	} else {
	  push(nameNODE, (void *)makeExistsOpNode());
	}
	subNode(n, term, varOrOp, name);
	n->body->args = list_new(termNode);
	insertArg(n->body->args);
	insertArg(n->body->args);
	push(termNODE, (void *)n);
	break;
    default:
	CompilerFailure("LSLAbstractTerm");
    }
    if (needKey) {
	n->key = node_key(termNode_varOrOp(n));
	if (termNode_args(n) != 0) {
	    list_traverse(termNode, termNode_args(n), t) {
		n->key = mash(n->key, node_key(t));
	    } end_traverse;
	}
    }
}


void
LSLAbstractConsequences(LSLToken loc) {
    consequencesNode *n;
    allocNode(consequences, n);
    setLoc(n, loc);
    if (top().kind == conversionNODELIST) {
	subNodeList(n, consequences, converts, conversion);
    }
    if (top().kind == quantEqnsNODELIST) {
	subNodeList(n, consequences, quantEqns, quantEqns);
    }
    if (top().kind == equationNODELIST) {
	subNodeList(n, consequences, eqns, equation);
    }
    if (top().kind == genPartByNODELIST) {
	subNodeList(n, consequences, genPartBys, genPartBy);
    }
    if (top().kind == traitRefNODELIST) {
	subNodeList(n, consequences, traits, traitRef);
    }
    push(consequencesNODE, (void *)n);
}


void
LSLAbstractConversion(LSLRuleCode rule, LSLToken loc) {
    bool first;
    conversionNode *n;
    allocNode(conversion, n);
    setLoc(n, loc);
    switch(rule) {
    case CONVERSION2:
	subNode(n, conversion, exempts, exemption);
    case CONVERSION1:
	subNodeList(n, conversion, converts, operator);
	break;
    default:
	CompilerFailure("LSLAbstractConversion");
    }
    first = top().kind != conversionNODELIST;
    push(conversionNODE, (void *)n);
    if (first) {
	LSLAbstractConversionList();
    } else {
	LSLAbstractConversionListExtend();
    }
}


void
LSLAbstractExemption(LSLRuleCode rule, LSLToken loc) {
    exemptionNode *n;
    allocNode(exemption, n);
    setLoc(n, loc);
    subNodeList(n, exemption, terms, term);
    if (rule == EXEMPTION2) {
	subNodeList(n, exemption, forall, varDcl);
    }
    push(exemptionNODE, (void *)n);
}


void
LSLAbstractSeparatorList(void) {
    list(separatorNode) *n = list_new(separatorNode);
    push(separatorNODELIST, (void *)n);
}


void
LSLAbstractSeparatorListExtend(void) {
    extendNodeList(separator);
}


void
LSLAbstractNameList(void) {
    makeNodeList(name);
}


void
LSLAbstractNameListExtend(void) {
    extendNodeList(name);
}


void
LSLAbstractTermList(void) {
    makeNodeList(term);
}


void
LSLAbstractTermListExtend(void) {
    extendNodeList(term);
}


void
LSLAbstractArgList(void) {
    makeNodeList(term);
    LSLAbstractSeparatorList();
    dummy(separatorNODE);
    LSLAbstractSeparatorListExtend();
}


void
LSLAbstractArgListExtend(void) {
    termNode *t;
    separatorNodeList *s;
    if (top().kind != termNODE) {
	CompilerFailure("LSLAbstractArgListExtend: expecting term");
    }
    t = (termNode *)(popTop().ptr);
    LSLAbstractSeparatorListExtend();
    s = (separatorNodeList *)(popTop().ptr);
    push(termNODE, (void *)t);
    LSLAbstractTermListExtend();
    push(separatorNODELIST, (void *)s);
}


void
LSLAbstractEquationList(void) {
    makeNodeList(equation);
}


void
LSLAbstractEquationListExtend(void) {
    extendNodeList(equation);
}


void
LSLAbstractVarDclList(void) {
    makeNodeList(varDcl);
}


void
LSLAbstractVarDclListExtend(void) {
    extendNodeList(varDcl);
}


void
LSLAbstractQuantEqnsList(void) {
    makeNodeList(quantEqns);
}


void
LSLAbstractQuantEqnsListExtend(void) {
    extendNodeList(quantEqns);
}


void
LSLAbstractOperatorList(void) {
    makeNodeList(operator);
}


void
LSLAbstractOperatorListExtend(void) {
    extendNodeList(operator);
}


void
LSLAbstractGenPartByList(void) {
    makeNodeList(genPartBy);
}


void
LSLAbstractGenPartByListExtend(void) {
    extendNodeList(genPartBy);
}


void
LSLAbstractReplaceList(void) {
    makeNodeList(replace);
}


void
LSLAbstractReplaceListExtend(void) {
    extendNodeList(replace);
}


void
LSLAbstractTraitRefList(void) {
    makeNodeList(traitRef);
}


void
LSLAbstractTraitRefListExtend(void) {
    extendNodeList(traitRef);
}


void
LSLAbstractConversionList(void) {
    makeNodeList(conversion);
}


void
LSLAbstractConversionListExtend(void) {
    extendNodeList(conversion);
}


void
LSLAbstractPropPartList(void) {
    makeNodeList(propPart);
}


void
LSLAbstractPropPartListExtend(void) {
    extendNodeList(propPart);
}


void
LSLAbstractOpDclList(void) {
    makeNodeList(opDcl);
}


void
LSLAbstractOpDclListExtend(void) {
    extendNodeList(opDcl);
}


void
LSLAbstractOpPartList(void) {
    makeNodeList(opPart);
}


void
LSLAbstractOpPartListExtend(void) {
    extendNodeList(opPart);
}


void
LSLAbstractFieldList(void) {
    makeNodeList(field);
}


void
LSLAbstractFieldListExtend(void) {
    extendNodeList(field);
}


void
LSLAbstractExtList(void) {
    makeNodeList(ext);
}


void
LSLAbstractExtListExtend(void) {
    extendNodeList(ext);
}


void
LSLAbstractParameterList(void) {
    makeNodeList(parameter);
}

void
LSLAbstractParameterListExtend(void) {
    extendNodeList(parameter);
}
    
void
LSLAbstractSortList(void) {
    makeNodeList(sort);
}

void
LSLAbstractSortListExtend(void) {
    extendNodeList(sort);
}
    
void
LSLAbstractElementIdList(void) {
    makeLeafList(elementId);
}

void
LSLAbstractElementIdListExtend(void) {
    extendLeafList(elementId);
}
    
void
LSLAbstractFieldIdList(void) {
    makeLeafList(fieldId);
}

void
LSLAbstractFieldIdListExtend(void) {
    extendLeafList(fieldId);
}
    

void
LSLAbstractTraitIdList(void) {
    makeLeafList(traitId);
}

void
LSLAbstractTraitIdListExtend(void) {
    extendLeafList(traitId);
}
    
void
LSLAbstractVarIdList(void) {
    makeLeafList(varId);
}

void
LSLAbstractVarIdListExtend(void) {
    extendLeafList(varId);
}
    
static void
insertArg(termNodeList *args) {
    if (top().kind != termNODE) {
	CompilerFailure("abstract.c: insertArg");
    }
    list_addl(termNode, args, (termNode *)popTop().ptr);
}


/*
** The following procedures are of general utility.
*/

traitNode
*makeDummyTraitNode(void) {
    traitNode *n;
    allocNode(trait, n);
    return n;
}

sortNode
*makeBoolSort(void) {
    if (theBoolSort == 0) {
	LSLToken *t = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
	*t = LSLReserveToken(SIMPLEID, "Bool");
	theBoolSort = makeSortNode(t, (sortNodeList *)0);
    }
    return theBoolSort;
}


sortNode		
*makeSortNode(sortIdLeaf *s, sortNodeList *subsorts) {
    sortNode *n;
    allocNode(sort, n);
    n->loc = *s;
    n->body->id  = s;
    n->body->subsorts = subsorts;
    return n;
}


signatureNode
*makeSignature(sortNodeList *domain, sortNode *range) {
    signatureNode *s;
    allocNode(signature, s);
    s->body->domain = domain;
    s->body->range = range;
    return s;
}

signatureNode
*makeSignature0(sortNode *range) {
    signatureNode *s;
    allocNode(signature, s);
    s->body->domain = list_new(sortNode);
    s->body->range = range;
    return s;
}

signatureNode
*makeSignature1(sortNode *dom1, sortNode *range) {
    signatureNode *s = makeSignature0(range);
    list_addh(sortNode, s->body->domain, dom1);
    return s;
}

signatureNode	
*makeSignature2(sortNode *dom1, sortNode *dom2, sortNode *range) {
    signatureNode *s = makeSignature1(dom1, range);
    list_addh(sortNode, s->body->domain, dom2);
    return s;
}

signatureNode
*makeSignature3(sortNode *dom1, sortNode *dom2, sortNode *dom3, 
		sortNode *range) {
    signatureNode *s = makeSignature2(dom1, dom2, range);
    list_addh(sortNode, s->body->domain, dom3);
    return s;
}


extern operatorNode	
*makeOperatorNode(nameNode *name) {
    operatorNode *n;
    allocNode(operator, n);
    setLoc(n, node_loc(name));
    n->body->name = name;
    return n;
}


extern termNode		
*makeTermNode(nameNode *root, termNodeList *args) {
    termNode *n;
    allocNode(term, n);
    setLoc(n, node_loc(root));
    n->body->varOrOp = root;
    n->body->args = args;
    n->key = node_key(termNode_varOrOp(n));
    if (termNode_args(n) != 0) {
	list_traverse(termNode, termNode_args(n), t) {
	    n->key = mash(n->key, node_key(t));
	} end_traverse;
    }
    return n;
}


extern equationNode	
*makeEquationNode(termNode *left, termNode *right) {
    equationNode *n;
    termNode	 *t;
    allocNode(equation, n);
    setLoc(n, node_loc(left));
    n->body->left = left;
    n->body->right = right;
    n->key = node_key(equationNode_left(n));
    t = equationNode_right(n);
    if (t != (termNode *)0) n->key = mash(n->key, node_key(t));
    return n;
}


extern enumerationNode
*makeEnumerationNode(sortNode *s, elementIdLeafList *elems, LSLToken loc) {
    enumerationNode *n;
    allocNode(enumeration, n);
    setLoc(n, loc);
    n->body->sort = s;
    n->body->elems = elems;
    return n;
}

extern separatorNode	
*makeSeparatorNode (sepSymLeaf *s) {
    separatorNode *n;
    allocNode(separator, n);
    setLoc(n, *s);
    n->body->separator = s;
    return n;
}


extern genByNode
*makeGenByNode(sortNode *s, bool freely, OpList *ops, LSLToken loc) {
    genByNode *n;
    allocNode(genBy, n);
    setLoc(n, loc);
    n->body->sort = s;
    n->body->freely = freely;
    n->body->ops = list_new(operatorNode);
    list_traverse(operator, ops, op) {
	list_addh(operatorNode, n->body->ops, makeOperatorNode(op->name));
    } end_traverse;
    n->key = ops_key(genByNode_ops(n));
    return n;
}


extern partByNode
*makePartByNode(sortNode *s, OpList *ops, LSLToken loc) {
    partByNode *n;
    allocNode(partBy, n);
    setLoc(n, loc);
    n->body->sort = s;
    n->body->ops = list_new(operatorNode);
    list_traverse(operator, ops, op) {
	list_addh(operatorNode, n->body->ops, makeOperatorNode(op->name));
    } end_traverse;
    n->key = ops_key(partByNode_ops(n));
    return n;
}


nameNode
*makeFalseOpNode(void) {
    if (theFalseOpNode == 0) {
	allocNode(name, theFalseOpNode);
	theFalseOpNode->body->kind = simpleIdKIND;
	theFalseOpNode->body->choice.simpleId = &FalseToken;
	theFalseOpNode->key = leaf_key(&FalseToken);
    }
    return theFalseOpNode;
}


nameNode
*makeTrueOpNode(void) {
    if (theTrueOpNode == 0) {
	allocNode(name, theTrueOpNode);
	theTrueOpNode->body->kind = simpleIdKIND;
	theTrueOpNode->body->choice.simpleId = &TrueToken;
	theTrueOpNode->key = leaf_key(&TrueToken);
    }
    return theTrueOpNode;
}


nameNode
*makeEqOpNode(void) {
    if (theEqOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = eqOpKIND;
	a->body->choice.eqOp = &EqToken;
	a->key = leaf_key(&EqToken);
	theEqOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theEqOpNode;
}

nameNode
*makeNeqOpNode(void) {
    if (theNeqOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = eqOpKIND;
	a->body->choice.eqOp = &NeqToken;
	a->key = leaf_key(&NeqToken);
	theNeqOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theNeqOpNode;
}


nameNode
*makeAllOpNode(void) {
    if (theAllOpNode == 0) {
	allocNode(name, theAllOpNode);
	theAllOpNode->body->kind = quantifierKIND;
	theAllOpNode->body->choice.quantifier = &AllToken;
	theAllOpNode->key = leaf_key(&AllToken);
    }
    return theAllOpNode;
}


nameNode
*makeExistsOpNode(void) {
    if (theExistsOpNode == 0) {
	allocNode(name, theExistsOpNode);
	theExistsOpNode->body->kind = quantifierKIND;
	theExistsOpNode->body->choice.quantifier = &ExistsToken;
	theExistsOpNode->key = leaf_key(&ExistsToken);
    }
    return theExistsOpNode;
}


nameNode
*makeAndOpNode(void) {
    if (theAndOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = logicalOpKIND;
	a->body->choice.logicalOp = &AndToken;
	a->key = leaf_key(&AndToken);
	theAndOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theAndOpNode;
}


nameNode
*makeOrOpNode(void) {
    if (theOrOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = logicalOpKIND;
	a->body->choice.logicalOp = &OrToken;
	a->key = leaf_key(&OrToken);
	theOrOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theOrOpNode;
}


nameNode
*makeNotOpNode(void) {
    if (theNotOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = simpleOpKIND;
	a->body->choice.simpleOp = &NotToken;
	a->key = leaf_key(&NotToken);
	theNotOpNode = makePpiOpNameNode(FALSE, a, TRUE);
    }
    return theNotOpNode;
}


nameNode
*makeImpliesOpNode(void) {
    if (theImpliesOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = logicalOpKIND;
	a->body->choice.logicalOp = &ImpliesToken;
	a->key = leaf_key(&ImpliesToken);
	theImpliesOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theImpliesOpNode;
}


nameNode
*makeIffOpNode(void) {
    if (theIffOpNode == 0) {
	anyOpNode *a;
	allocNode(anyOp, a);
	a->body->kind = logicalOpKIND;
	a->body->choice.logicalOp = &IffToken;
	a->key = leaf_key(&IffToken);
	theIffOpNode = makePpiOpNameNode(TRUE, a, TRUE);
    }
    return theIffOpNode;
}


nameNode
*makeIfOpNode(void) {
    if (theIfOpNode == 0) {
	allocNode(name, theIfOpNode);
	theIfOpNode->body->kind = ifOpKIND;
	theIfOpNode->body->choice.ifOp = 0;
    	theIfOpNode->key = leaf_key(&IfToken);
    }
    return theIfOpNode;
}


extern nameNode
*makePpiOpNameNode(bool before, anyOpNode *op, bool after) {
    nameNode *n;
    ppiOpNode *p;
    allocNode(name, n);
    n->body->kind = ppiOpKIND;
    allocNode(ppiOp, p);
    n->body->choice.ppiOp = p;
    p->body->before = before;
    p->body->op = op;
    p->body->after = after;
    setLoc(p, op->loc);
    setLoc(n, op->loc);
    n->key = node_key(op);
    p->key = n->key;
    return n;
}


anyOpNode	
*makeSimpleAnyOpNode (simpleOpLeaf *op) {
    anyOpNode *n;
    allocNode(anyOp, n);
    setLoc(n, *op);
    n->body->kind = simpleOpKIND;
    n->body->choice.simpleOp = op;
    n->key = leaf_key(anyOpNode_simpleOp(n));
    return n;
}

nameNode
*makeSimpleIdNameNode (simpleIdLeaf *s) {
    nameNode *n;
    allocNode(name, n);
    setLoc(n, *s);
    n->body->kind = simpleIdKIND;
    n->body->choice.simpleId = s;
    n->key = leaf_key(nameNode_simpleId(n));
    return n;
}


nameNode
*makeSelectOpNameNode (simpleIdLeaf *s) {
    nameNode *n;
    selectOpNode *sel;
    allocNode(selectOp, sel);
    setLoc(sel, *s);
    sel->body->before = TRUE;
    sel->body->sel = s;
    allocNode(name, n);
    setLoc(n, *s);
    n->body->kind = selectOpKIND;
    n->body->choice.selectOp = sel;
    n->key = leaf_key(s);
    return n;
}


nameNode
*makeMixfixOpNameNode (bool before, openSymLeaf *open, separatorNodeList *seps,
		       closeSymLeaf *close, bool after) {
    nameNode 		*n;
    mixfixOpNode 	*m;
    allocNode(mixfixOp, m);
    setLoc(m, *open);
    m->body->before = before;
    m->body->open = open;
    m->body->seps = seps;
    m->body->close = close;
    m->body->after = after;
    allocNode(name, n);
    setLoc(n, *open);
    n->body->kind = mixfixOpKIND;
    n->body->choice.mixfixOp = m;
    n->key = leaf_key(open);
    seps = mixfixOpNode_seps(m);
    if (seps != (separatorNodeList *)0) {
	list_traverse(separatorNode, seps, s) {
	    n->key = mash(n->key, leaf_key(separatorNode_separator(s)));
	} end_traverse;
    }
    n->key =  mash(n->key, leaf_key(close));
    m->key = n->key;
    return n;
}


int
nameNodeArity (nameNode *n) {
    switch (n->body->kind) {
    case simpleIdKIND:
	return -1;			/* unknown */
    case ifOpKIND:
	return 3;
    case ppiOpKIND:
	{ppiOpBody *p = n->body->choice.ppiOp->body;
	 return (p->before?1:0) + (p->after?1:0);
     }
    case mixfixOpKIND:
	{mixfixOpBody *m = n->body->choice.mixfixOp->body;
	 int a = (m->before?1:0) + (m->after?1:0);
	 return a + ((m->seps==0)?0:(list_size(separatorNode, m->seps) + 1));
     }
    case selectOpKIND:
	 return 1;
    case quantifierKIND:
	 return 2;
    }
    return 0;			/* keep Saber C happy */
}


bool
nameNodeMatches (nameNode *n1, nameNode *n2, bool implicitMarkers) {
    if (n1 == n2) return TRUE;
    if (n1->body->kind != n2->body->kind) return FALSE;
    if (n1->key != n2->key) return FALSE;
    switch (n1->body->kind) {
    case simpleIdKIND:
	return LSLSameLeaf(n1->body->choice.simpleId, 
			   n2->body->choice.simpleId);
    case ifOpKIND:
	return TRUE;
    case ppiOpKIND:
	return samePpiOp(n1->body->choice.ppiOp, n2->body->choice.ppiOp,
			 implicitMarkers);
    case mixfixOpKIND:
	return sameMixfixOp(n1->body->choice.mixfixOp, 
			    n2->body->choice.mixfixOp);
    case selectOpKIND:
	return sameSelectOp(n1->body->choice.selectOp, 
			    n2->body->choice.selectOp, implicitMarkers);
    case quantifierKIND:
	return LSLSameLeaf(n1->body->choice.quantifier, 
			   n2->body->choice.quantifier);
    }
    return FALSE;		/* keep Saber C happy */
}

int
signatureNodeArity (signatureNode *s) {
    return list_size(sortNode, s->body->domain);
}


bool
LSLSameLeaf(LSLToken *t1, LSLToken *t2) {
    return t1->text == t2->text;
}


static bool
sameAnyOp(anyOpNode *n1, anyOpNode *n2) {
    anyOpBody *a1 = n1->body;
    anyOpBody *a2 = n2->body;
    if (a1->kind != a2->kind) return FALSE;
    switch (a1->kind) {
    case simpleOpKIND:
	return LSLSameLeaf(a1->choice.simpleOp, a2->choice.simpleOp);
    case logicalOpKIND:
	return LSLSameLeaf(a1->choice.logicalOp, a2->choice.logicalOp);
    case eqOpKIND:
	return LSLSameLeaf(a1->choice.eqOp, a2->choice.eqOp);
    }
    return FALSE;		/* keep Saber C happy */
}

static bool
samePpiOp(ppiOpNode *n1, ppiOpNode *n2, bool implicitMarkers) {
    ppiOpBody *p1 = n1->body;
    ppiOpBody *p2 = n2->body;
    return (((implicitMarkers && !(p1->before) && !(p1->after))
	     || ((p1->before == p2->before) && (p1->after == p2->after)))
	    && sameAnyOp(p1->op, p2->op));
}

static bool
sameSeparator(separatorNode *n1, separatorNode *n2) {
    sepSymLeaf *s1 = n1->body->separator;
    sepSymLeaf *s2 = n2->body->separator;
    if (s1 == 0) {
	return (s2 == 0);
    } else {
	return ((s2 != 0) && LSLSameLeaf(s1, s2));
    }
}


static bool
sameMixfixOp(mixfixOpNode *n1, mixfixOpNode *n2) {
    mixfixOpBody *m1 = n1->body;
    mixfixOpBody *m2 = n2->body;
    return ((m1->before == m2->before)
	    && (m1->after == m2->after)
	    && LSLSameLeaf(m1->open, m2->open)
	    && LSLSameLeaf(m1->close, m2->close)
	    && ((m1->seps == 0) == (m2->seps == 0))
	    && ((m1->seps == 0) ||
		list_compare(separatorNode, m1->seps, m2->seps, 
			     sameSeparator)));
}

static bool
sameSelectOp(selectOpNode *n1, selectOpNode *n2, bool implicitMarker) {
    selectOpBody *s1 = n1->body;
    selectOpBody *s2 = n2->body;
    return (((s1->before == s2->before) || (implicitMarker && !(s1->before)))
	    && LSLSameLeaf(s1->sel, s2->sel));
}

/* ENHANCE: assign keys to signatures and use to optimize check */

bool
LSLSameSignature (signatureNode *n1, signatureNode *n2) {
    signatureBody *s1 = n1->body;
    signatureBody *s2 = n2->body;
    return (LSLSameSort(s1->range, s2->range)
	    && list_compare(sortNode, s1->domain, s2->domain, LSLSameSort));
}

bool
LSLSameSort (sortNode *n1, sortNode *n2) {
    return (LSLSameLeaf(n1->body->id, n2->body->id)
	    && list_compare(sortNode, n1->body->subsorts, n2->body->subsorts, 
			    LSLSameSort));
}

bool
LSLSubsort (sortNode *n1, sortNode *n2) {
  if (LSLSameSort(n1, n2)) return TRUE;
  if (sortNode_subsorts(n1) == (sortNodeList *)0
      && LSLSameLeaf(sortNode_id(n1), sortNode_id(n2))) return TRUE;
  if (sortNode_subsorts(n2) != (sortNodeList *)0) {
    list_traverse(sortNode, sortNode_subsorts(n2), n3) {
      if (LSLSubsort(n1, n3)) return TRUE;
    } end_traverse;
  }
  return FALSE;
}


/*
** Internal procedures for use by routines invoked from reduce.c.
*/

static void
push(nodeKind k, void *p) {
    if (stackIndex < STACKMAX) {
	Stack[stackIndex].kind = k;
	Stack[stackIndex++].ptr = p;
     } else {
	CompilerFailure("abstract.c: needs MoreMem()");
     }
}

static nodeRecord
top(void) {
    if (stackIndex < 1) {
	CompilerFailure("abstract.c -- empty stack");
    }
    return Stack[stackIndex-1];
}

static nodeRecord
popTop(void) {
    if (stackIndex < 1) {
	CompilerFailure("abstract.c -- empty stack");
    }
    return Stack[--stackIndex];
}

static void
pop(void) {
    if (stackIndex < 1) {
	CompilerFailure("abstract.c -- empty stack");
    }
    --stackIndex;
}


static unsigned int
leaf_key (LSLToken *t) {
    if (t == 0) return 0;
    return mash(0, token_textSym(*t));
}


static unsigned int
ops_key (operatorNodeList *ops) {
    int key = 0;
    list_traverse(operatorNode, ops, op) {
	key = mash(key, node_key(operatorNode_name(op)));
    } end_traverse;
    return key;
}


static unsigned int
mash (unsigned int x, unsigned int y) {
    return ((x<<1) + y) & HASHMASK;
}


/*
** General interface
*/

static void
UserError(LSLToken *t, char *msg) {
    error_reportLocation(token_fileName(*t), token_line(*t), token_col(*t));
    error_reportAndQuit(msg);
}

void
LSLAbstractInit(void) {
}

void
LSLAbstractReset(void) {
    stackIndex = 0;					/* empty stack	*/
}

void
LSLAbstractCleanup(void) {

}
