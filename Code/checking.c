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
**      FILENAME:   checking.c
**
**	PURPOSE:    Context-sensitive checking for traits
**
**  AUTHORS:
**
**	Gary Feldman, Technical Languages and Environments, DECspec project
**	Steve Garland, Massachusetts Institute of Technology
**	James Rauen, Massachusetts Institute of Technology
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	Rauen	89.06.05	Original CLU version
**  X0.1-2	SJG	90.09.27	Translate into C, revise for LSL2.3
**  X0.1-3	JPW	90.11.09	Change ## to PASTE().
**  X0.1-4	JPW	90.11.19	Fix accvio in checkFormals.
**  X0.1-5	JPW	90.12.11	Print symtable, tree, and renaming
**					for all traits.
**  X0.1-6	JPW	91.02.20	Merge in MIT changes.
**  X0.1-7	JPW	91.03.18	Change SPEC_LSL_PATH to SPEC_PATH.
**  X0.1-8	GAF	91.03.19	Use SPECPATH macro from LSL instead
**					of "SPEC_PATH" literal.
**  X0.1-9	JPW	91.05.07	Don't try to open included file if
**					can't find with source_getPath.
**  X0.1-10	GAF	91.05.20	Print sorts of arguments when we issue
**  					"not declared with matching domain
**					sorts" message.
**  X0.1-11	JPW	91.07.29	Add extra parameters to source_create.
**  X0.1-12	SJG	91.12.20	Compute lsl2lp attributes, other hooks,
**					redo external interface, fix bug in
**					term attribution (mark OPTERM) and
**					checkConversion (for constants)
**  X0.1-13     SJG     92.01.24        Expose adjustMarkers
**  X0.1-14     SJG     92.02.12	Remove check that range of at least one
**					operator in a partitions differ from
**					partitioned sort (and the accompanying
**					clever disambiguation) [Version 2.4]
**  X0.1-15     SJG     92.02.16	Add DecimalLiterals
**  X0.1-16	SJG	92.02.23	Fix uses of nameNodeMatches, add
**					recheckTraitRefs
**  X0.1-17	SJG	92.03.22	Skip trait/file name check for .lcs
**  X0.1-18	SJG	92.06.14	Composite sort notation A$B
**  X0.1-19	SJG	92.09.30	Add printSorts, improve messages for
**					undeclared ops or ambiguous equations
**  X0.1-20	SJG	93.11.02	Changed plus1 to succ in DecimalLiterals
**  X0.1-21	SJG	93.11.10	Added quantified terms
**  X0.1-22	SJG	93.12.05	Added #includes for clean compile
**  X0.1-23	SJG	94.04.20	Added "generated freely"
**  X0.1-24	SJG	94.05.23	Composite sort notation A[B, C]
**  X0.1-25 	SJG	99.12.09	Changed decimal literals trait
**  [@modification history entry@]...
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <string.h>
# include	"general.h"
# include	"osdfiles.h"
# include	STDLIB
# include	"string2sym.h"
# include	"token.h"
# include 	"error.h"
# include	"print.h"
# include	"lsl.h"
# include	"list.h"
# include	"set.h"
# include	"tokentable.h"
# include	"source.h"
# include	"scan.h"
# include	"parse.h"
# include 	"abstract.h"
# include	"operator.h"
# include	"renaming.h"
# include	"symtable.h"
# include	"vartable.h"
# include	"prop.h"
# include	"propSet.h"
# include	"checking.h"
# include	"recheck.h"
# include	"unparse.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define LBRACKET	"["
# define RBRACKET	"]"

# define nullAttr(x)			\
    struct PASTE(PASTE(_, x), Attr) { 	\
	char	* junk;			\
    } PASTE(x, Attr)

# define allocAttr(nodeKind, node)					\
    node->attr = (PASTE(nodeKind, Attr) *)				\
                   LSLMoreMem((void *)0, sizeof(PASTE(nodeKind, Attr)))

# define DecLitsTEXT \
    "DecimalLiterals(N, succ): trait\n\
       introduces\n\
         succ: N ->N\n\
         times10plus: N, N -> N\n\
         0, 1, 2, 3, 4, 5, 6, 7, 8, 9: -> N\n\
       asserts \\forall i, j: N\n\
         times10plus(0, j) = j \\eqsep\n\
         times10plus(i, succ(j)) = succ(times10plus(i, j)) \\eqsep\n\
         times10plus(succ(i), 0) = succ(times10plus(i, 9)) \\eqsep\n\
         1 = succ(0) \\eqsep\n\
         2 = succ(1) \\eqsep\n\
         3 = succ(2) \\eqsep\n\
         4 = succ(3) \\eqsep\n\
         5 = succ(4) \\eqsep\n\
         6 = succ(5) \\eqsep\n\
         7 = succ(6) \\eqsep\n\
         8 = succ(7) \\eqsep\n\
         9 = succ(8)\n"


/*
**
**  TYPEDEFS
**
*/

typedef nullAttr(name);
typedef nullAttr(ifOp);
typedef nullAttr(ppiOp);
typedef nullAttr(mixfixOp);
typedef nullAttr(selectOp);
typedef nullAttr(anyOp);
typedef nullAttr(separator);
typedef nullAttr(signature);
typedef nullAttr(ext);
typedef nullAttr(field);
typedef nullAttr(includes);
typedef nullAttr(assumes);
typedef nullAttr(opPart);
typedef nullAttr(opDcl);
typedef nullAttr(propPart);
typedef nullAttr(genPartBy);
typedef nullAttr(varDcl);
typedef nullAttr(equation);
typedef nullAttr(consequences);
typedef nullAttr(conversion);


/*
**
**  FORWARD FUNCTIONS
**
*/

extern void		attributeVarTerm();
extern void		attributeOpTerm();
extern void		attributeQuantifiedTerm ();
extern genByNode       *buildGenByNode();
extern partByNode      *buildPartByNode();

extern operator        *parameter_operator();
extern operator        *operator_declaration();
extern Sort	       *term_sort();
extern symtable	       *trait_symtable();
extern traitNode       *parseAndCheckTrait(traitIdLeaf *id, 
					   traitNodeList *allTraits);

static traitNode       *makeBadTrait(traitIdLeaf *id, 
				     traitNodeList *allTraits);
static void		checkTrait(traitNode *t, traitNodeList *allTraits);
static void		checkAxioms(traitNode *t, traitNodeList *allTraits);
static void		checkAssumes(traitNode *t, traitRefNodeList *refs, 
				     traitNodeList *allTraits);
static void		checkIncludes(traitNode *t, traitRefNodeList *refs, 
				      traitNodeList *allTraits);
static void 		checkTraitRefs(traitNode *t, traitRefNodeList *refs, 
				       traitNodeList *allTraits, bool implied);
static void		checkTraitRef(traitNode *t, traitRefNode *ref, 
				      traitNodeList *allTraits, bool implied);
static void		checkActuals(renaming *r, traitRefNode *ref);
static void		checkRenamings(renaming *r, symtable *stab, 
				       replaceNodeList *renamings);
static void		checkConsequences(traitNode *t, 
					  traitNodeList *allTraits);
static void		checkEnum(traitNode *t, enumerationNode *e, 
				  bool checkDups);
static void		checkTuple(traitNode *t, tupleNode *tu);
static void		checkUnion(traitNode *t, unionNode *u);
static void		checkOpDclList(traitNode *t, opDclNodeList *decs);
static void		checkOpDcl(traitNode *t, opDclNode *dec);
static void		checkPropPart(traitNode *t, propPartNode *ppn);
static void		checkFormals(traitNode *t, parameterNodeList *formals);
static void		checkGenPartBys(genPartByNodeList *g, symtable *stab);
static void		checkGeneratedBy(genByNode *g, symtable *stab);
static void		checkPartitionedBy(partByNode *p, symtable *stab);
static void	        checkOperator(operatorNode *op, symtable *stab);
static void		checkEquations(equationNodeList *eqs, symtable *stab,
				       vartable *vars);
static void		checkQuantEqns(quantEqnsNodeList *qes, symtable *stab);
static vartable	       *checkQuantifier(varDclNodeList *decs, symtable *stab);
static void		checkEquation(equationNode *e, symtable *stab,
				      vartable *vars);
static void		checkTerm(termNode *t, symtable *stab, vartable *vars);
static void		checkConversion(conversionNode *c, symtable *stab);

extern nameNode	       *adjustMarkers(nameNode *newName, operator *oldOp);
static void		assignSorts(termNode *t, Sort *s);
static bool		variableFreeIn (variable *v, termNode *t);
static bool	        checkMarkers(operatorNode *op);
static SortSet	       *findPossibleSorts(termNode *t, symtable *stab, 
					  vartable *vars);
static LSLToken	       *makeToken(LSLTokenCode cod, char *text, LSLToken loc);
static void		printSorts (char *label, SortSet *ss);
static sortNode	       *tagSortNode(sortNode *n);
static OpSet	       *termOps(termNode *t);
static void		attributeOps(operatorNodeList *opn, OpList *ops);


# define badNode(n, msg)						\
    badLeaf(node_loc(n), msg)

/* *** Replace printf below with a routine in the error module. *** */
# define badLeaf(tok, msg) 						\
    {error_reportLocation(token_fileName(tok), token_line(tok), 	\
			  token_col(tok));				\
     printf("`%s\' ", token_text(tok));                                 \
     error_report(msg);}

# define badLoc(tok, msg)						\
    {error_reportLocation(token_fileName(tok), token_line(tok), 	\
			  token_col(tok));				\
     error_report(msg);}

# define badName(n, msg)						\
    {error_reportLocation(token_fileName(node_loc(n)), 			\
			  token_line(node_loc(n)), 			\
			  token_col(node_loc(n)));			\
     print_char('`');							\
     unparse_name(n);							\
     print_str("'");							\
     print_newline(0);							\
     printf("%s", " ");							\
     error_report(msg);}


/*
**
**  STATIC VARIABLES (none)
**
*/


/*
**
**  FUNCTION DEFINITIONS
**
*/


extern void
attributeVarTerm (termNode *n, Sort *s) {
    variable *v;
    if (node_exists(n->attr)) return;
    v = variable_new(nameNode_simpleId(termNode_varOrOp(n)), s);
    allocAttr(term, n);
    termAttr_kind(n) = VARTERM;
    termAttr_possibleVars(n) = set_new(variable, variable_equal);
    set_insert(variable, termAttr_possibleVars(n), v);
    termAttr_sort(n) = s;
}


extern void
attributeOpTerm (termNode *n, operator *op) {
    if (node_exists(n->attr)) return;
    allocAttr(term, n);
    termAttr_kind(n) = OPTERM;
    termAttr_possibleOps(n) = OpSet_new();
    set_insert(operator, termAttr_possibleOps(n), op);
    termAttr_sort(n) = signatureNode_range(operator_signature(op));
}


extern void
attributeQuantifiedTerm (termNode *n) {
    if (node_exists(n->attr)) return;
    allocAttr(term, n);
    termAttr_kind(n) = QUANTIFIEDTERM;
    termAttr_sort(n) = makeBoolSort();
}


extern genByNode
*buildGenByNode (sortNode *s, bool freely, OpList *ops, LSLToken loc) {
    genByNode *n = makeGenByNode(s, freely, ops, loc);
    allocAttr(genBy, n);
    n->attr->ops = ops;
    attributeOps(genByNode_ops(n), ops);
    return n;
}


extern partByNode
*buildPartByNode (sortNode *s, OpList *ops, LSLToken loc) {
    partByNode *n = makePartByNode(s, ops, loc);
    allocAttr(partBy, n);
    n->attr->ops = ops;
    attributeOps(partByNode_ops(n), ops);
    return n;
}

extern operator 
*parameter_operator (parameterNode *n) {
    return (n->attr == 0 || parameterAttr_kind(n) != operatorKIND)
            ?0:parameterAttr_op(n);
}


extern operator 
*operator_declaration (operatorNode *n) {
    return (n->attr == 0)?0:operatorAttr_op(n);
}


extern Sort
*term_sort (termNode *n) {
    return (n->attr == 0)?0:termAttr_sort(n);
}


extern symtable
*trait_symtable (traitNode *n) {
    return (n->attr == 0)?0:traitAttr_symtab(n);
}



/*
** Loads, parses, and checks the trait named "*id", as well as all traits
** referenced in it.  Adds all new traits to "*allTraits".
*/

traitNode
*parseAndCheckTrait (traitIdLeaf *id, traitNodeList *allTraits) {
    source 	*sourceFile;
    traitNode 	*theTrait = (traitNode *)0;
    int		 rc;
    bool	 literals = (strcmp(token_text(*id), "DecimalLiterals") == 0);
    if (literals) {
	sourceFile = source_fromString(DecLitsNAME, DecLitsTEXT);
    } else {
	sourceFile = source_create(token_text(*id), LSL_SUFFIX, FALSE, FALSE);
	if (!source_getPath(SPECPATH, sourceFile, *id)) {
	    badLeaf(*id, string_paste("can't find file `",
				      string_paste(source_fileName(sourceFile),
						   "' containing trait")));
	    return makeBadTrait(id, allTraits);
	}
	if (!source_open(sourceFile)) {
	    badLeaf(*id, "can't open file containing trait");
	    return makeBadTrait(id, allTraits);
	}
    }
    rc = error_setQuit();
    if (rc == 0) {
	LSLScanReset(sourceFile);
	LSLParseReset();
	LSLAbstractReset();
	LSLParse();
	theTrait = LSLAbstractParseTree();
	list_addh(traitNode, allTraits, theTrait);
    }
    source_close(sourceFile);
    if (!node_exists(theTrait)) return makeBadTrait(id, allTraits);
    if (!literals) {
	char *fullname = token_text(*id);
	char *name = (char *)LSLMoreMem((void *)0, strlen(fullname)+1);
	char *ps = strrchr(fullname, '/');
	if (ps == (char *)0) { ps = fullname; } else { ps++; }
	strcpy(name, ps);
	ps = strrchr(name, '.');
	if (ps == (char *)0 || strcmp(ps, ".lcs") != 0) {
	    if (ps != (char *)0) *ps = '\0';
	    if (strcmp(name, token_text(*traitNode_name(theTrait))) != 0) {
		badLeaf(*traitNode_name(theTrait), 
			"trait name does not match file name");
	    }
	}
	free(name);
    }
    checkTrait(theTrait, allTraits);
    if (literals) {
	parameterNodeList *f = traitNode_formals(theTrait);
	list_pointToLast(parameterNode, f);
	symtable_enableLiterals(traitAttr_symtab(theTrait),
				parameterAttr_op(list_current(parameterNode, f)));
    }
    return theTrait;
}


/*
** Returns a dummy trait corresponding to a nonexistent or unparsable
** source file.  Adds this trait to "*allTraits".
*/

traitNode
*makeBadTrait (traitIdLeaf *name, traitNodeList *allTraits) {
    traitNode *t = makeDummyTraitNode();
    t->body->name = name;
    allocAttr(trait, t);
    t->attr->parseError = TRUE;
    t->attr->axiomsCorrect = DANGLE;
    t->attr->consequencesCorrect = DANGLE;
    list_addh(traitNode, allTraits, t);
    return t;
}



/*
** Checks the axioms of the trait "*t".  If this succeeds, also checks the
** consequences.
*/

static void
checkTrait (traitNode *t, traitNodeList *allTraits) {
    if (node_null(t->attr)) {
	allocAttr(trait, t);
	t->attr->parseError = FALSE;
	t->attr->axiomsCorrect = UNCHECKED;
	t->attr->consequencesCorrect = UNCHECKED;
	t->attr->symtab = symtable_new();
    }
    if (t->attr->parseError) return;
    checkAxioms(t, allTraits);
    if (traitAttr_axiomsChecked(t)) checkConsequences(t, allTraits);

    /* Print out the checked trait if asked. */
    
    if (GetPrintTreeFlag()) {
	unparse_setForLP(FALSE);
	unparse_trait(t);
	print_newline(1);
    }
    
    if (GetDebugFlag()) {
	symtable_unparse(trait_symtable(t));
    }
}


/*
** Checks the axioms of "*t".
*/

static void
checkAxioms(traitNode *t, traitNodeList *allTraits) {
    int initialErrorCount = error_count();
    switch (traitAttr_axiomsCorrect(t)) {
    case CHECKING:
	error_programFailure("checkAxioms");
    case CORRECT:
    case DANGLE:
    case INCORRECT: 
	return;
    case UNCHECKED:
	break;
    }
    traitAttr_axiomsCorrect(t) = CHECKING;
    if (node_exists(traitNode_exts(t))) {
	list_traverse(extNode, traitNode_exts(t), en) {
	    switch (extNode_kind(en)) {
	    case LSLenumKIND:
		checkEnum(t, extNode_LSLenum(en), TRUE);
		break;
	    case LSLtupleKIND:
		checkTuple(t, extNode_LSLtuple(en));
		break;
	    case LSLunionKIND:
		checkUnion(t, extNode_LSLunion(en));
		break;
	    case assumesKIND:
		checkAssumes(t, assumesNode_traits(extNode_assumes(en)),
			     allTraits);
		break;
	    case includesKIND:
		checkIncludes(t, includesNode_traits(extNode_includes(en)),
			      allTraits);
		break;
	    }
	} end_traverse;
    }
    if (traitAttr_axiomsCorrect(t) == DANGLE) return;
    if (node_exists(traitNode_introduces(t))) {
	list_traverse(opPartNode, traitNode_introduces(t), opn) {
	    checkOpDclList(t, opPartNode_introduces(opn));
	} end_traverse;
    }
    if (node_exists(traitNode_exts(t))) {
	list_traverse(extNode, traitNode_exts(t), en) {
	    switch (extNode_kind(en)) {
	    case LSLenumKIND:
	    case LSLtupleKIND:
	    case LSLunionKIND:
		break;
	    case assumesKIND:
		recheckTraitRefs(traitAttr_symtab(t), TRUE,
				 assumesNode_traits(extNode_assumes(en)));
		break;
	    case includesKIND:
		recheckTraitRefs(traitAttr_symtab(t), TRUE,
				 includesNode_traits(extNode_includes(en)));
		break;
	    }
	} end_traverse;
    }
    if (node_exists(traitNode_asserts(t))) {
	list_traverse(propPartNode, traitNode_asserts(t), ppn) {
	    checkPropPart(t, ppn);
	} end_traverse;
    }
    if (node_exists(traitNode_formals(t))) {
	checkFormals(t, traitNode_formals(t));
    }
    if (initialErrorCount == error_count()) {
        traitAttr_axiomsCorrect(t) = CORRECT; 
    } else {
	traitAttr_axiomsCorrect(t) = INCORRECT;
    }
}


/*
** Checks the assumption of "*refs" by "*t".
*/

static void
checkAssumes (traitNode *t, traitRefNodeList *refs, traitNodeList *allTraits) {
    checkTraitRefs(t, refs, allTraits, FALSE);
}


/*
** Checks the inclusion of "*refs" by "*"t.
*/

static void
checkIncludes(traitNode *t, traitRefNodeList *refs, traitNodeList *allTraits) {
    checkTraitRefs(t, refs, allTraits, FALSE);
}


/*
** Checks the traitRefs in "*refs".
*/

static void 
checkTraitRefs (traitNode *t, traitRefNodeList *refs, 
		traitNodeList *allTraits, bool implied) {
    list_traverse(traitRefNode, refs, ref) {
	checkTraitRef(t, ref, allTraits, implied);
    } end_traverse;
}


/*
** Checks the traitRef "*ref", which is implied by "*t" if "implied" is TRUE
** and which is included in, or assumed by, "*t" if "implied" is FALSE.
*/

static void
checkTraitRef (traitNode *t, traitRefNode *ref, traitNodeList *allTraits,
	       bool implied) {
    bool 		 checkedBelow 	= TRUE;
    traitNodeList 	*subtraits 	= list_new(traitNode);
    symtable		*stab;
    renaming		*rename		= renaming_new();
    allocAttr(traitRef, ref);
    traitRefAttr_traits(ref) = subtraits;
    traitRefAttr_rename(ref) = rename;
    list_traverse(traitIdLeaf, traitRefNode_traits(ref), id) {
	traitNode *subtrait;
	subtrait = (traitNode *)0;
	list_traverse(traitNode, allTraits, t1) {
	    if (token_textSym(*id) == token_textSym(*traitNode_name(t1))) {
		subtrait = t1;
		if (traitAttr_axiomsCorrect(t1) == CHECKING) {
		    traitAttr_axiomsCorrect(t1) = INCORRECT;
		    badLeaf(*id, "cycle in trait hierarchy");
		} else if (traitAttr_axiomsCorrect(t1) != CORRECT) {
		    badLeaf(*id, "reference to trait containing errors");
		}
		break;
	    }
	} end_traverse;
	if (node_null(subtrait)) {
	    subtrait = parseAndCheckTrait(id, allTraits);
	    if (strcmp(token_text(*id), "DecimalLiterals") == 0
		&& !node_null(traitRefNode_renamings(ref))) {
		list_traverse(replaceNode, traitRefNode_renamings(ref), rep) {
		    parameterNode *formal;
		    nameNode	  *nm;
		    formal = replaceNode_formal(rep);
		    switch (parameterNode_kind(formal)) {
		    case operatorKIND:
		        nm = operatorNode_name(parameterNode_operator(formal));
			if (!isLiteralName(nm)) continue;
			break;
		    case sortKIND:
			continue;
		    case simpleIdKIND:
			if (!isLiteralToken(parameterNode_simpleId(formal)))
			    continue;
			break;
		    }
		    badNode(formal, "decimal literal cannot be renamed");
		} end_traverse;
	    }
	}
	if (traitAttr_parseError(subtrait) || 
	    traitAttr_axiomsCorrect(subtrait) == DANGLE) {
	    traitAttr_axiomsCorrect(t) = DANGLE;
	}
	list_addh(traitNode, subtraits, subtrait);
	checkedBelow = checkedBelow && !traitAttr_parseError(subtrait)
	    && traitAttr_axiomsChecked(subtrait); 
    } end_traverse;
    if (!checkedBelow) return;
    stab = symtable_blank();
    list_traverse(traitNode, subtraits, subtrait) {
	symtable_extend(stab, traitAttr_symtab(subtrait), rename);
    } end_traverse;
    checkActuals(rename, ref);
    checkRenamings(rename, stab, traitRefNode_renamings(ref));

    /* Print out the renamings if asked. */
    if (GetPrintRenameFlag()) {
	renaming_unparse(rename);
    }

    if (implied) {
	symtable_contains(traitAttr_symtab(t), stab, rename, &node_loc(ref));
    } else {
	symtable_extend(traitAttr_symtab(t), stab, rename);
    }
}


static void
checkActuals (renaming *r, traitRefNode *ref) {
    traitIdLeafList	*traitIds = traitRefNode_traits(ref);
    traitIdLeaf		*traitId;
    traitNodeList  	*traits   = traitRefAttr_traits(ref);
    parameterNodeList 	*actuals  = traitRefNode_actuals(ref);
    parameterNodeList	*formals;
    parameterNode	*formal;
    nameNode 		*newName;
    operator		*oldOp;
    sortNode 		*newSort;
    if (node_null(actuals)) return;
    if (list_size(traitNode, traits) > 1) {
	/* abstract.c ensures that only single traits have actuals */
	error_programFailure("checkActuals");
    }
    list_pointToFirst(traitIdLeaf, traitIds);
    traitId = list_current(traitIdLeaf, traitIds);
    /* Check that formals exist */
    list_pointToFirst(traitNode, traits);
    formals = traitNode_formals(list_current(traitNode, traits));
    if (node_null(formals) || list_size(parameterNode, formals) == 0) {
	badLeaf(*traitId, "no formals for traitRef");
	return;
    } 
    if (list_size(parameterNode, actuals) != 
	list_size(parameterNode, formals)) {
	badLeaf(*traitId, "wrong number of actual parameters for traitRef");
	/* Note: rather than return, we pair up what we can */
    }
    /* Pair actuals with formals */
    list_pointToFirst(parameterNode, formals);
    list_traverse(parameterNode, actuals, actual) {
	formal = list_current(parameterNode, formals);
	if (node_null(formal)) break;		/* no more left */
	switch (parameterAttr_kind(formal)) {
	case operatorKIND:
	    /* Check for duplicate oldOps done by checkFormals */
	    oldOp = parameterAttr_op(formal);
	    switch (parameterNode_kind(actual)) {
	    case operatorKIND:
		newName = operatorNode_name(parameterNode_operator(actual));
		break;
	    case sortKIND:
		badNode(actual, "actual parameter must be an operator");
		newName = (nameNode *)0;
		break;
	    case simpleIdKIND:
		newName = makeSimpleIdNameNode(parameterNode_simpleId(actual));
	    }
	    if (newName != (nameNode *)0) {
		newName = adjustMarkers(newName, oldOp);
		if (node_exists(newName)) {
		    renaming_addOp(r, oldOp, newName);
		} /* else checkMarkers has already noted this error */
	    }
	    break;
	case sortKIND:
	    /* Check for duplicate oldSorts done by checkFormals */
	    switch (parameterNode_kind(actual)) {
	    case operatorKIND:
		badName(operatorNode_name(parameterNode_operator(actual)), 
			"actual parameter must be a sort");
		newSort = (sortNode *)0;
		break;
	    case sortKIND:
		newSort = parameterNode_sort(actual);
		break;
	    case simpleIdKIND:
		newSort = makeSortNode(parameterNode_simpleId(actual),
				       (sortNodeList *)0);
		break;
	    }
	    if (newSort != (sortNode *)0) {
		renaming_addSort(r, parameterAttr_sort(formal), newSort);
	    }
	    break;
	case simpleIdKIND:
	    /* checkTraitRef has already noted this error */
	    break;
	}
	list_pointToNext(parameterNode, formals);
    } end_traverse;
}


/*
** !!! RECODE: Share code between checkFormals, checkRenamings.
*/

static void
checkRenamings (renaming *r, symtable *stab, replaceNodeList *renamings) {
    nameNode *n;
    if (node_null(renamings)) return;
    list_traverse(replaceNode, renamings, replace) {
	parameterNode 	*actual, *formal;
	operatorNode	*oldOp;
	nameNode	*newName;
	sortNode	*newSort, *oldSort;
	bool		 canBeSort, canBeOp;
	actual 	  = replaceNode_actual(replace);
	formal 	  = replaceNode_formal(replace);
	allocAttr(replace, replace);
	replaceAttr_kind(replace) = simpleIdKIND;
	switch (parameterNode_kind(actual)) {
	case operatorKIND:
	    canBeSort = FALSE;
	    canBeOp = TRUE;
	    newName = operatorNode_name(parameterNode_operator(actual));
	    switch (parameterNode_kind(formal)) {
	    case operatorKIND:
		oldOp = parameterNode_operator(formal);
		break;
	    case sortKIND:
		badNode(actual, "sort cannot be renamed as an operator");
		canBeOp = FALSE;
	    case simpleIdKIND:
		n = makeSimpleIdNameNode(parameterNode_simpleId(formal));
		oldOp = makeOperatorNode(n);
		break;
	    }
	    break;
	case sortKIND:
	    canBeSort = TRUE;
	    canBeOp = FALSE;
	    newSort = parameterNode_sort(actual);
	    switch (parameterNode_kind(formal)) {
	    case operatorKIND:
		badNode(formal, "sort cannot be renamed as an operator");
		canBeSort = FALSE;
		break;
	    case sortKIND:
		oldSort = parameterNode_sort(formal);
		break;
	    case simpleIdKIND:
		oldSort = makeSortNode(parameterNode_simpleId(formal),
				       (sortNodeList *)0);
		break;
	    }
	    break;
	case simpleIdKIND:
	    switch (parameterNode_kind(formal)) {
	    case operatorKIND:
		canBeSort = FALSE;
		canBeOp = TRUE;
		oldOp = parameterNode_operator(formal);
		newName = makeSimpleIdNameNode(parameterNode_simpleId(actual));
		break;
	    case sortKIND:
		canBeSort = TRUE;
		canBeOp = FALSE;
		newSort = makeSortNode(parameterNode_simpleId(actual),
				       (sortNodeList *)0);
		oldSort = parameterNode_sort(formal);
		break;
	    case simpleIdKIND:
		canBeSort = TRUE;
		canBeOp = TRUE;
		oldSort = makeSortNode(parameterNode_simpleId(formal),
				       (sortNodeList *)0);
		newSort = makeSortNode(parameterNode_simpleId(actual),
				       (sortNodeList *)0);
		newName = makeSimpleIdNameNode(parameterNode_simpleId(actual));
		n = makeSimpleIdNameNode(parameterNode_simpleId(formal));
		oldOp = makeOperatorNode(n);
		break;
	    }
	    break;
	}
	if (canBeSort && symtable_sortExists(stab, oldSort)) {
	    if (renaming_addSort(r, oldSort, newSort)) {
		replaceAttr_kind(replace) = sortKIND;
		replaceAttr_sort(replace) = oldSort;
	    } else {
		badNode(oldSort, "old sort already used in renaming");
	    }
	} else if (canBeOp) {
	    operator *op;
	    OpSet *ops;
	    n = operatorNode_name(oldOp);
	    ops = symtable_matchingOps(stab, n, operatorNode_signature(oldOp));
	    switch (set_size(operator, ops)) {
	    case 0:
		if (canBeSort) {
		    badName(n, "nonexistent old sort or operator in renaming");
		} else {
		    badName(n, "nonexistent old operator in renaming");
		}
		break;
	    case 1:
		op = set_choose(operator, ops);
		newName = adjustMarkers(newName, op);
		if (node_exists(newName) && 
		    !renaming_addOp(r, op, newName)) {
		    badName(n, "old operator already used in renaming");
		} else {
		    replaceAttr_kind(replace) = operatorKIND;
		    replaceAttr_op(replace) = op;
		}
		break;
	    default:
		badName(n, "ambiguous old operator in renaming");
		break;
	    }
	} else if (canBeSort) {
	    badNode(oldSort, "nonexistent old sort or operator in renaming");
	}
    } end_traverse;
}


/*
** Checks the consequences of "*t", unless they have already been checked or
** are being checked.
*/

static void
checkConsequences (traitNode *t, traitNodeList *allTraits) {
    int 		 initialErrorCount 	= error_count();
    consequencesNode 	*c			= traitNode_implies(t);
    symtable 		*stab			= traitAttr_symtab(t);
    if (traitAttr_consequencesCorrect(t) != UNCHECKED) return;
    if (node_exists(c)) {
	conversionNodeList 	*cvts = consequencesNode_converts(c);
	traitRefNodeList	*refs = consequencesNode_traits(c);
	checkGenPartBys(consequencesNode_genPartBys(c), stab);
	checkEquations(consequencesNode_eqns(c), stab, vartable_new());
	checkQuantEqns(consequencesNode_quantEqns(c), stab);
	if (node_exists(cvts)) {
	    list_traverse(conversionNode, cvts, cvt) {
		checkConversion(cvt, stab);
	    } end_traverse;
	}
	if (node_exists(refs)) {
	    checkTraitRefs(t, refs, allTraits, TRUE);
	}
    }
    if (initialErrorCount == error_count()) {
	traitAttr_consequencesCorrect(t) = CORRECT; 
    } else {
	traitAttr_consequencesCorrect(t) = INCORRECT;
    }
}


static void
checkEnum (traitNode *t, enumerationNode *e, bool checkDups) {
    Sort		*S 	= enumerationNode_sort(e);
    symtable		*stab 	= traitAttr_symtab(t);
    signatureNode	*cSig	= makeSignature0(S);
    signatureNode	*sig	= makeSignature1(S, S);
    nameNode		*succ;
    allocAttr(enumeration, e);
    /* Normalization: introduces c_1, ..., c_n: -> S 			    */
    e->attr->elems = list_new(operator);
    list_traverse(elementIdLeaf, enumerationNode_elems(e), c) {
	operator *op1;
	op1 = operator_new(makeSimpleIdNameNode(c), cSig);
	if (checkDups) { /* Skip if invoked by checkUnion */
	    list_traverse(operator, e->attr->elems, op) {
		if (operator_equal(op, op1)) {
		    badLeaf(*c, "duplicate element in enumeration");
		    break;
		}
	    } end_traverse;
	    }
	symtable_declareOpAndSorts(stab, operator_name(op1), cSig);
	list_addh(operator, e->attr->elems, op1);
    } end_traverse;
    /* Normalization: introduces succ: S -> S 				    */
    succ = makeSimpleIdNameNode(makeToken(SIMPLEID, "succ", node_loc(e)));
    symtable_declareOpAndSorts(stab, succ, sig);
    e->attr->succ = operator_new(succ, sig);
}


static void
checkTuple (traitNode *t, tupleNode *tu) {
    Sort		*S 	= tupleNode_sort(tu);
    symtable		*stab 	= traitAttr_symtab(t);
    list(Sort)		*sorts  = list_new(Sort);
    nameNode		*make;
    signatureNode	*sig;
    separatorNode	*sep;
    separatorNodeList	*seps	= list_new(separatorNode);
    int 		 i;
    allocAttr(tuple, tu);
    /* Normalization: introduces .f_i:S->S_i				    */
    /* 				 set_f_i:S,S_i->S			    */
    tu->attr->gets = list_new(operator);
    tu->attr->sets = list_new(operator);
    list_traverse(fieldNode, tupleNode_fields(tu), fl) {
	Sort 		*fieldSort;
	signatureNode	*getSig;
	signatureNode	*setSig;
	fieldSort = fieldNode_sort(fl);
	getSig = makeSignature1(S, fieldSort);
	setSig = makeSignature2(S, fieldSort, S);
	if (sameSort(S, fieldSort)) {
	    badNode(fieldSort, "field sort must differ from tuple sort");
	}
	list_traverse(fieldIdLeaf, fieldNode_fields(fl), f) {
	    nameNode *fget;
	    nameNode *fset;
	    char     *setString;
	    fget = makeSelectOpNameNode(f);
	    list_traverse(operator, tu->attr->gets, op) {
		if (nameNodeMatches(fget, operator_name(op), FALSE)) {
		    badLeaf(*f, "duplicate field in tuple");
		    break;
		}
	    } end_traverse;
	    list_addh(Sort, sorts, fieldSort);
	    symtable_declareOpAndSorts(stab, fget, getSig);
	    list_addh(operator, tu->attr->gets, operator_new(fget, getSig));
	    setString = string_paste("set_", token_text(*f));
	    fset = makeSimpleIdNameNode(makeToken(SIMPLEID, setString, *f));
	    symtable_declareOpAndSorts(stab, fset, setSig);
	    list_addh(operator, tu->attr->sets, operator_new(fset, setSig));
	} end_traverse;
    } end_traverse;
    /* Normalization: introduces [__, ..., __]: S_1, ..., S_n -> S	    */
    sep = makeSeparatorNode(makeToken(SEPSYM, ",", node_loc(tu)));
    for (i = 0; i < list_size(operator, tu->attr->gets) - 1; i++) {
	list_addh(separatorNode, seps, sep);
    }
    make = makeMixfixOpNameNode(FALSE,
				makeToken(LBRACK, LBRACKET, node_loc(tu)),
				seps,
				makeToken(RBRACK, RBRACKET, node_loc(tu)),
				FALSE);
    sig = makeSignature(sorts, S);
    symtable_declareOpAndSorts(stab, make, sig);
    tu->attr->make = operator_new(make, sig);
}


static void
checkUnion (traitNode *t, unionNode *u) {
    Sort		*S 	= tupleNode_sort(u);
    elementIdLeafList	*fields = list_new(elementIdLeaf);
    symtable		*stab 	= traitAttr_symtab(t);
    list(Sort)		*sorts  = list_new(Sort);
    nameNode		*tag;
    signatureNode	*sig;
    allocAttr(union, u);
    /* Normalization: introduces f_i:S_i->S				    */
    /*				 .f_i:S->S_i				    */
    u->attr->toS	= list_new(operator);
    u->attr->fromS	= list_new(operator);
    list_traverse(fieldNode, unionNode_fields(u), fl) {
	Sort 		*fieldSort;
	signatureNode	*toSig;
	signatureNode	*fromSig;
	fieldSort = fieldNode_sort(fl);
	toSig = makeSignature1(fieldSort, S);
	fromSig = makeSignature1(S, fieldSort);
	if (sameSort(S, fieldSort)) {
	    badNode(fieldSort, "field sort must differ from union sort");
	}
	list_traverse(fieldIdLeaf, fieldNode_fields(fl), f) {
	    nameNode *fFrom;
	    nameNode *fTo;
	    list_traverse(elementIdLeaf, fields, f1) {
		if (LSLSameLeaf(f, f1)) {
		    badLeaf(*f, "duplicate field in union");
		    break;
		}
	    } end_traverse;
	    list_addh(elementIdLeaf, fields, f);
	    list_addh(Sort, sorts, fieldSort);
	    fFrom = makeSelectOpNameNode(f);
	    symtable_declareOpAndSorts(stab, fFrom, fromSig);
	    list_addh(operator, u->attr->fromS, operator_new(fFrom, fromSig));
	    fTo = makeSimpleIdNameNode(f);
	    symtable_declareOpAndSorts(stab, fTo, toSig);
	    list_addh(operator, u->attr->toS, operator_new(fTo, toSig));
	} end_traverse;
    } end_traverse;
    /* Normalization: S_tag enumeration of f_1, ..., f_n		    */
    u->attr->tagS = tagSortNode(S);
    u->attr->labels = makeEnumerationNode(u->attr->tagS, fields, node_loc(u));
    checkEnum(t, u->attr->labels, FALSE);
    /* Normalization: introduces tag: S -> S_tag			    */
    tag = makeSimpleIdNameNode(makeToken(SIMPLEID, "tag", node_loc(u)));
    sig = makeSignature1(S, u->attr->tagS);
    symtable_declareOpAndSorts(stab, tag, sig);
    u->attr->tag = operator_new(tag, sig);
}


static void
checkOpDclList (traitNode *t, opDclNodeList *decs) {
    if (node_null(decs)) return;
    list_traverse(opDclNode, decs, dec) {
	checkOpDcl(t, dec);
    } end_traverse;
}


static void
checkOpDcl (traitNode *t, opDclNode *dec) {
    signatureNode *s = opDclNode_signature(dec);
    int arity = signatureNodeArity(s);
    list_traverse(nameNode, opDclNode_ops(dec), n) {
	int arity1;
	arity1 = nameNodeArity(n);
	if ((nameNode_kind(n) == selectOpKIND) && 
	    !nameNode_selectOpBefore(n)) {
	    badName(n, "missing __ in declaration of selectOp");
	} else if ((arity1 >= 0) && (arity1 != arity)) {
	    badName(n, "number of __'s in opForm does not match signature");
	} else {
	    symtable_declareOpAndSorts(traitAttr_symtab(t), n, s);
	}
    } end_traverse;
}


static void
checkPropPart (traitNode *t, propPartNode *ppn) {
    symtable *stab = traitAttr_symtab(t);
    checkGenPartBys(propPartNode_genPartBys(ppn), stab);
    /*
    ** !!! ENHANCE: replace vartable_new() by vartable_null(), which always
    ** !!! returns the same empty set.  The present vartable_null() returns a
    ** !!! null pointer, which causes problems later on.
    */
    checkEquations(propPartNode_equations(ppn), stab, vartable_new());
    checkQuantEqns(propPartNode_quantEqns(ppn), stab);
}


static void
checkFormals (traitNode *t, parameterNodeList *formals) {
    symtable *stab = traitAttr_symtab(t);
    list_traverse(parameterNode, formals, formal) {
	bool		 canBeSort, canBeOp;
	operatorNode	*oldOp;
	sortNode	*oldSort;
	simpleIdLeaf	*id;
	allocAttr(parameter, formal);
	parameterAttr_kind(formal) = simpleIdKIND;
	switch (parameterNode_kind(formal)) {
	case operatorKIND:
	    canBeOp = TRUE;
	    canBeSort = FALSE;
	    oldOp = parameterNode_operator(formal);
	    break;
	case sortKIND:
	    canBeOp = FALSE;
	    canBeSort = TRUE;
	    oldSort = parameterNode_sort(formal);
	    break;
	case simpleIdKIND:
	    id = parameterNode_simpleId(formal);
	    canBeOp = TRUE;
	    canBeSort = TRUE;
	    oldSort = makeSortNode(id, (sortNodeList *)0);
	    oldOp = makeOperatorNode(makeSimpleIdNameNode(id));
	    break;
	}
	if (canBeSort && symtable_sortExists(stab, oldSort)) {
	    parameterAttr_kind(formal) = sortKIND;
	    parameterAttr_sort(formal) = oldSort;
	    list_traverse(parameterNode, formals, formal1) {
		if (formal == formal1) {
		    /* formal1 has met itself.  Leave the loop because rest */
		    /* of list has not been set up (attr block not	    */
		    /* allocated). */
		    break;
		} else if ((parameterAttr_kind(formal1) == sortKIND) &&
		           sameSort(parameterAttr_sort(formal1), oldSort)) {
		    /* Found duplicate sort in formal list.  Complain. */
		    badNode(oldSort, "sort used twice as formal parameter");
		    parameterAttr_kind(formal) = simpleIdKIND;
		    break;
		}
	    } end_traverse;
	} else if (canBeOp && checkMarkers(oldOp)) {
	    nameNode *n = operatorNode_name(oldOp);
	    OpSet    *ops = symtable_matchingOps(stab, n, 
						 operatorNode_signature(oldOp));
	    switch (set_size(operator, ops)) {
	    case 0:
		if (canBeSort) {
		    badName(n, "no matching sort or operator for formal");
		} else {
		    badName(n, "no matching operator for formal");
		}
		break;
	    case 1:
		parameterAttr_kind(formal) = operatorKIND;
		parameterAttr_op(formal) = set_choose(operator, ops);
		list_traverse(parameterNode, formals, formal1) {
		    if (formal == formal1) {
			/* formal1 has met itself.  Leave the loop because  */
			/* rest of list has not been set up (attr block not */
			/* allocated).					    */
			break;
		    } else if (parameterAttr_kind(formal1) == operatorKIND &&
			       parameterAttr_op(formal1) ==
			       parameterAttr_op(formal)) {
			/* Found duplicate operator in formal list.	    */
			/* Complain.					    */
			badName(n, "operator used twice as formal parameter");
			parameterAttr_kind(formal) = simpleIdKIND;
			break;
		    }
		} end_traverse;
		break;
	    default:
		badName(n, "ambiguous operator used as formal parameter");
		break;
	    }
	} else if (canBeSort) {
	    badNode(oldSort, "no matching sort or operator for formal");
	} /* else error already reported by checkMarkers */
    } end_traverse;
}


static void
checkGenPartBys (genPartByNodeList *gl, symtable *stab) {
    if (node_null(gl)) return;
    list_traverse(genPartByNode, gl, gp) {
	switch (genPartByNode_kind(gp)) {
	case genByKIND:
	    checkGeneratedBy(genPartByNode_genBy(gp), stab);
	    break;
	case partByKIND:
	    checkPartitionedBy(genPartByNode_partBy(gp), stab);
	    break;
	}
    } end_traverse;
}

static void
checkGeneratedBy (genByNode *g, symtable *stab) {
    Sort 	*genS 			= genByNode_sort(g);
    bool  	 basisOpExists		= FALSE;
    nameNode 	*disambiguatedByBasis 	= (nameNode *)0;
    allocAttr(genBy, g);
    genByAttr_ops(g) = list_new(operator);
    list_traverse(operatorNode, genByNode_ops(g), op) {
	OpSet 	   	*ops;
	operator 	*theOp;
	bool       	 theOpIsBasis;
	bool	  	 cleverDisambiguation;
	allocAttr(operator, op);
	if (!checkMarkers(op)) continue;
	ops = symtable_matchingOps(stab, operatorNode_name(op),
				   operatorNode_signature(op));
	theOp 			= (operator *)0;
	theOpIsBasis 		= FALSE;
	cleverDisambiguation 	= FALSE;
	set_traverse(operator, ops, dec) {
	    signatureNode 	*sig;
	    bool		 basisOp;
	    sig = operator_signature(dec);
	    if (!sameSort(genS, signatureNode_range(sig))) continue;
	    basisOp = TRUE;
	    list_traverse(Sort, signatureNode_domain(sig), s) {
		if (sameSort(genS, s)) {
		    basisOp = FALSE;
		    break;
		}
	    } end_traverse;
	    if (node_exists(theOp)) {
		if (basisOpExists || (basisOp && theOpIsBasis)) {
		    badName(operatorNode_name(op), "ambiguous operator");
		    break;
		}
		cleverDisambiguation = TRUE;
		if (basisOp) { 
		    theOp = dec;	       /* dec is basis, not theOp */
		    theOpIsBasis = TRUE;
		} else if (!theOpIsBasis) {
		    theOp = (operator *)0;   /* neither is basis        */ 
		} else {
		    			       /* theOp is basis, not dec */
		}
	    } else {
		theOp = dec;
		theOpIsBasis = basisOp;
	    }
	} end_traverse;
	if (node_exists(theOp)) {
	    if (cleverDisambiguation) {
		disambiguatedByBasis = operatorNode_name(op);
	    } else if (theOpIsBasis && node_exists(disambiguatedByBasis)) {
		badName(disambiguatedByBasis, "ambiguous operator");
	    }
	    operatorAttr_op(op) = theOp;
	    basisOpExists = basisOpExists || theOpIsBasis;
	} else {
	    basisOpExists = TRUE;	/* suppress basis op error message */
	    if (set_size(operator, ops) == 0) {
		badName(operatorNode_name(op), "no matching operator");
	    } else {
		badName(operatorNode_name(op), 
			"no matching operator with generated sort as range");
	    }
	}
	list_addh(operator, genByAttr_ops(g), theOp);
    } end_traverse;
    if (!basisOpExists) {
	badLoc(node_loc(g), 
	       "all generators have the generated sort in their domains");
    }
}


static void
checkPartitionedBy (partByNode *p, symtable *stab) {
    Sort *partS = partByNode_sort(p);
    allocAttr(partBy, p);
    partByAttr_ops(p) = list_new(operator);
    list_traverse(operatorNode, partByNode_ops(p), op) {
	OpSet 	 *ops;
	operator *theOp;
	allocAttr(operator, op);
	if (!checkMarkers(op)) continue;
	ops   = symtable_matchingOps(stab, operatorNode_name(op), 
				     operatorNode_signature(op));
	theOp = (operator *)0;
	set_traverse(operator, ops, dec) {
	    signatureNode 	*sig;
	    bool		 domainConstraint;
	    sig = operator_signature(dec);
	    domainConstraint = FALSE;
	    list_traverse(Sort, signatureNode_domain(sig), s) {
		if (sameSort(s, partS)) {
		    domainConstraint = TRUE;
		    break;
		}
	    } end_traverse;
	    if (!domainConstraint) continue;
	    if (node_exists(theOp)) {
		badName(operatorNode_name(op), "ambiguous operator");
		break;
	    } else {
		theOp = dec;
	    }
	} end_traverse;
	if (node_exists(theOp)) {
	    operatorAttr_op(op) = theOp;
	} else {
	    if (set_size(operator, ops) == 0) {
		badName(operatorNode_name(op), "no matching operator");
	    } else {
		badName(operatorNode_name(op), 
			"no matching operator with partitioned sort in domain");
	    }
	}
	list_addh(operator, partByAttr_ops(p), theOp);
    } end_traverse;
}


static void
checkOperator (operatorNode *op, symtable *stab) {
    OpSet *ops;
    allocAttr(operator, op);
    if (!checkMarkers(op)) return;
    ops = symtable_matchingOps(stab, operatorNode_name(op), 
			       operatorNode_signature(op));
    switch (set_size(operator, ops)) {
    case 0:
	badName(operatorNode_name(op), "no matching operator");
	break;
    case 1:
	operatorAttr_op(op) = set_choose(operator, ops);
	break;
    default:
	badName(operatorNode_name(op), "ambiguous operator");
    }
}


static void
checkEquations (equationNodeList *eqs, symtable *stab, vartable *vars) {
    if (node_null(eqs)) return;
    list_traverse(equationNode, eqs, en) {
	checkEquation(en, stab, vars);
    } end_traverse;
}


static void
checkQuantEqns (quantEqnsNodeList *qes, symtable *stab) {
    if (node_null(qes)) return;
    list_traverse(quantEqnsNode, qes, qe) {
	if (qe->attr == 0) {
	    allocAttr(quantEqns, qe);
	}
	quantEqnsAttr_vars(qe) = 
	    checkQuantifier(quantEqnsNode_quantifier(qe), stab);
	checkEquations(quantEqnsNode_equations(qe), stab,
		       quantEqnsAttr_vars(qe));
    } end_traverse;
}


static vartable
*checkQuantifier (varDclNodeList *decs, symtable *stab) {
    vartable *vars = vartable_new();
    list_traverse(varDclNode, decs, vd) {
	Sort *so;
	so = varDclNode_sort(vd);
	if (!symtable_sortExists(stab, so)) {
	    badNode(so, "sort not introduced");
	    symtable_declareSort(stab, so);
	}
	list_traverse(varIdLeaf, varDclNode_vars(vd), vl) {
	    if (symtable_legalVar(stab, vl, so)) {
		if (!vartable_declare(vars, vl, so)) {
		    badLeaf(*vl, "duplicate variable in declaration");
		}
	    } else {
		badLeaf(*vl, "variable duplicates constant of same sort");
	    }
	} end_traverse;
    } end_traverse;
    return vars;
}


static void
checkEquation (equationNode *e, symtable *stab, vartable *vars) {
    termNode 	*left 	= equationNode_left(e);
    termNode 	*right 	= equationNode_right(e);
    SortSet 	*leftSorts;
    SortSet 	*rightSorts;
    SortSet 	*commonSorts;
    Sort	*theSort;
    leftSorts = findPossibleSorts(left, stab, vars);
    if (node_null(right)) {
	rightSorts = set_new(Sort, sameSort);
	set_insert(Sort, rightSorts, makeBoolSort());
    } else {
	rightSorts = findPossibleSorts(right, stab, vars);
    }
    if ((termAttr_kind(left) == BADTERM) || 
	(node_exists(right) && termAttr_kind(right) == BADTERM)) {
	return;
    }
    commonSorts = set_intersect(Sort, leftSorts, rightSorts);
    switch (set_size(Sort, commonSorts)) {
    case 0:
	if (node_exists(right)) {
	    badNode(e, "sorts of terms in equation do not match");
	    printSorts("Possible sorts for left side: ", leftSorts);
	    printSorts("Possible sorts for right side: ", rightSorts);
	} else {
	    badNode(e, "sort of term as equation must be Bool");
	    printSorts("Possible sorts for term: ", leftSorts);
	}
	return;
    case 1:
	theSort = set_choose(Sort, commonSorts);
	break;
    default:
	badNode(e, "more than one possible sort for terms in equation");
	printSorts("Possible sorts: ", commonSorts);
	/* !!! ENHANCE: mark term, subterms as bad? */
	return;
    }
    assignSorts(left, theSort);
    if (node_exists(right)) assignSorts(right, theSort);
}


static void
checkTerm (termNode *t, symtable *stab, vartable *vars) {
    SortSet 	*possibleSorts  = findPossibleSorts(t, stab, vars);
    Sort	*theSort;
    switch (set_size(Sort, possibleSorts)) {
    case 0:
	return;
    case 1:
	theSort = set_choose(Sort, possibleSorts);
	break;
    default:
	badNode(t, "more than one possible sort for term");
	printSorts("Possible sorts: ", possibleSorts);
	return;
    }
    assignSorts(t, theSort);
}


static void
checkConversion (conversionNode *c, symtable *stab) {
    operatorNodeList	*ops = conversionNode_converts(c);
    exemptionNode	*e   = conversionNode_exempts(c);
    list_traverse(operatorNode, ops, op) {
	checkOperator(op, stab);
    } end_traverse;
    if (node_exists(e)) {
	varDclNodeList	*forall = exemptionNode_forall(e);
	termNodeList	*terms	= exemptionNode_terms(e);
	allocAttr(exemption, e);
	exemptionAttr_vars(e) = 
	    node_exists(forall)?checkQuantifier(forall, stab):vartable_new();
	list_traverse(termNode, terms, term) {
	    checkTerm(term, stab, exemptionAttr_vars(e));
	    if (termAttr_kind(term) != BADTERM) {
		/* Ensure term contains some converted operator */
		OpSet *tops = termOps(term);
		bool   ok   = FALSE;
		list_traverse(operatorNode, ops, op) {
		    if (!node_exists(operatorAttr_op(op)) ||
			set_is_element(operator, operatorAttr_op(op), tops)){
			ok = TRUE;
			break;
		    }
		} end_traverse;
		if (!ok) 
		    badNode(term, "exempted term lacks converted operator");
	    }
	} end_traverse;
    }
}


extern nameNode
*adjustMarkers (nameNode *newName, operator *oldOp) {
    char 	*msg    = (char *)0;
    int   	 arityS = operator_arity(oldOp);
    int   	 arityN = nameNodeArity(newName);
    switch (nameNode_kind(newName)) {
    case ppiOpKIND:
	if (arityN > arityS) {
	    msg = "too many __'s in new name for operator";
	} else if (arityS > 2) {
	    msg = "new name unsuitable for old operator with arity > 2";
	} else if (0 < arityN && arityN < arityS) {
	    msg = "too few __'s in new name for operator";
	} else if (arityN == 0 && arityS == 1) {
	    if (nameNode_kind(operator_name(oldOp)) == ppiOpKIND) {
		ppiOpNode *p = nameNode_ppiOp(operator_name(oldOp));
		anyOpNode *nameAnyOp = ppiOpNode_op(nameNode_ppiOp(newName));
		if (ppiOpNode_before(p)) {
		    newName = makePpiOpNameNode(TRUE, nameAnyOp, FALSE);
		} else if (ppiOpNode_after(p)) {
		    newName = makePpiOpNameNode(FALSE, nameAnyOp, TRUE);
		} else {
		    error_programFailure("adjustMarkers");
		}
	    } else {
		msg = "ambiguous where __ belongs in new name for operator";
	    }
	} else if (arityN == 0 && arityS == 2) {
	    ppiOpNode *p = nameNode_ppiOp(newName);
	    newName = makePpiOpNameNode(TRUE, ppiOpNode_op(p), TRUE);
	} else if (arityN != arityS) {
	    error_programFailure("adjustMarkers");
	}
	break;
    case selectOpKIND:
	if (arityS == 1) {
	    /* !!! IS THIS NECESSARY?      */
	    /* Ensure that marker is there */
	    selectOpNode *s = nameNode_selectOp(newName);
	    newName = makeSelectOpNameNode(selectOpNode_sel(s));
	} else {
	    msg = "old operator for new selectOp name does not have arity 1";
	}
	break;
    default:
	if (arityN >= 0 && arityN != arityS) {
	    msg = "wrong number of __'s in new name for operator";
	}
	break;
    }
    if (msg == (char *)0) {
	return newName;
    } else {
	badName(newName, msg);
	return (nameNode *)0;
    }
}



/*
** Subsidiary procedures, in alphabetical order.
*/

static void
assignSorts (termNode *t, Sort *s) {
    nameNode		*name	= termNode_varOrOp(t);
    termNodeList	*args 	= termNode_args(t);
    bool	 	 found 	= FALSE;
    OpSet		*pops	= termAttr_possibleOps(t);
    operator		*op;
    list(Sort)		*dom;
    if (s == 0) error_programFailure("assignSorts");
    if (termAttr_kind(t) == ZEROARYTERM) {
	VarSet *pvars = termAttr_possibleVars(t);
	set_traverse(variable, pvars, v) {
	    if (sameSort(s, variable_sort(v))) {
		termAttr_kind(t) = VARTERM;
		termAttr_sort(t) = s;
		termAttr_possibleVars(t) = set_new(variable, variable_equal);
		set_insert(variable, termAttr_possibleVars(t), v);
		return;
	    }
	} end_traverse;
    } else if (termAttr_kind(t) == QUANTIFIEDTERM) {
        termNode *arg1, *arg2;
	VarSet   *pvars;
	variable *var = (variable *)0;
        list_pointToFirst(termNode, args);
	arg1 = list_current(termNode, args);
	list_pointToNext(termNode, args);
	arg2 = list_current(termNode, args);
	assignSorts(arg2, makeBoolSort());
	termAttr_kind(arg1) = VARTERM;
	name = termNode_varOrOp(arg1);
	pvars = termAttr_possibleVars(arg1);
	switch (set_size(variable, pvars)) {
	case 0:
	  badName(name, "undeclared variable");
	  return;
	case 1:
	  var = set_choose(variable, pvars);
	  break;
	default:
	  /* ENHANCE: disambiguate by picking free variable in subterm */
	  set_traverse(variable, pvars, v) {
	    if (variableFreeIn(v, arg2)) {
	      if (var == (variable *)0) {
		var = v;
	      } else {
		badName(name, "ambiguous variable");
		return;
	      }
	    }
	  } end_traverse;
	  if (var == (variable *)0) {
	    badName(name, "ambiguous variable");
	    return;
	  }
	}
	termAttr_sort(arg1) = variable_sort(var);
	termAttr_possibleVars(arg1) = set_new(variable, variable_equal);
	set_insert(variable, termAttr_possibleVars(arg1), var);
	return;
      }
    set_traverse(operator, pops, op1) {
	if (sameSort(s, signatureNode_range(operator_signature(op1)))) {
	    if (found) {
		badName(name, "ambiguous operator");
		return;
	    }
	    found = TRUE;
	    op = op1;
	    termAttr_kind(t) = OPTERM;
	    termAttr_sort(t) = s;
	    termAttr_possibleOps(t) = OpSet_new();
	    set_insert(operator, termAttr_possibleOps(t), op);
	}
    } end_traverse;
    if (!found) {
        switch (termAttr_kind(t)) {
	case OPTERM:
	    badName(name, "not declared with matching signature");
	    print_setIndent(8);
	    print_str("Range should be ");
	    unparse_sort(s);
	    print_newline(1);
	    print_setIndent(0);
	    break;
        case ZEROARYTERM:
        case VARTERM:
	    badName(name, 
		    "not declared as a constant or variable with matching sort");
	    break;
        default:
	    /* Error has been reported already */
	    break;
	}
	return;
    }
    if (node_exists(args)) {
	/* NOTE: Copy is needed to avoid interference */
	dom = list_copy(Sort, signatureNode_domain(operator_signature(op)));
	list_pointToFirst(Sort, dom);
	list_traverse(termNode, args, arg) {
	    assignSorts(arg, list_current(Sort, dom));
	    list_pointToNext(Sort, dom);
	} end_traverse;
    }
}


static bool
variableFreeIn (variable *v, termNode *t) {
  termNodeList *args = termNode_args(t);
  termNode *arg;
  if (!node_exists(t->attr)) return FALSE;
  switch (termAttr_kind(t)) {
  case VARTERM:
    return variable_equal(v, set_choose(variable, termAttr_possibleVars(t)));
  case OPTERM:
    list_traverse(termNode, args, t1) {
      if (variableFreeIn(v, t1)) return TRUE;
    } end_traverse;
    return FALSE;
  case QUANTIFIEDTERM:
    list_pointToFirst(termNode, args);
    arg = list_current(termNode, args);
    if (variable_equal(v, set_choose(variable, termAttr_possibleVars(arg)))) {
      return FALSE;
    }
    list_pointToNext(termNode, args);
    return variableFreeIn(v, list_current(termNode, args));
  default:
    return FALSE;
  }
}


static bool
checkMarkers (operatorNode *op) {
    bool ok = TRUE;
    signatureNode *sig = operatorNode_signature(op);
    nameNode *name = operatorNode_name(op);
    if (node_exists(sig)) {
	bool ok;
	int arityS = signatureNodeArity(sig);
	int arityN = nameNodeArity(name);
	switch (nameNode_kind(name)) {
	case ppiOpKIND:
	    ok = (arityN <= arityS) && (arityS <= 2);
	    break;
	case selectOpKIND:
	    ok = (arityS == 1);
	    break;
	default:
	    ok = (arityN < 0) || (arityN == arityS);
	    break;
	}
    }
    if (!ok) 
	badName(name, "number of __'s in opForm does not match signature");
    return ok;
}


static SortSet
*findPossibleSorts (termNode *t, symtable *stab, vartable *vars) {
    OpSet		*pops;
    VarSet 		*pvars;
    SortSet 		*ps 		= set_new(Sort, sameSort);
    nameNode 		*name 		= termNode_varOrOp(t);
    termNodeList	*args 		= termNode_args(t);
    Sort		*qual		= termNode_qual(t);
    list(SortSet)	*argSorts	= list_new(SortSet);
    SortSet 		*ss;
    if (t->attr == 0) {
	allocAttr(term, t);
	termAttr_possibleOps(t) = OpSet_new();
    }
    if ((nameNode_kind(name) == simpleIdKIND)
	&& (node_null(args) || (list_size(termNode, args) == 0))) {
	termAttr_kind(t) = ZEROARYTERM;
	pvars = vartable_matchingVars(vars, nameNode_simpleId(name), qual);
	termAttr_possibleVars(t) = pvars;
	set_traverse(variable, pvars, pvar) {
	    set_insert(Sort, ps, variable_sort(pvar));
	} end_traverse;
    } else if (nameNode_kind(name) == quantifierKIND) {
        termNode  *arg;
	termAttr_kind(t) = QUANTIFIEDTERM;
	termAttr_sort(t) = makeBoolSort();
	set_insert(Sort, ps, termAttr_sort(t));
	/* Find possible sorts for quantified variable */
	list_pointToFirst(termNode, args);
	arg = list_current(termNode, args);
	if (arg->attr == 0) allocAttr(term, arg);
	termAttr_kind(arg) = VARTERM;
	name = termNode_varOrOp(arg);
	qual = termNode_qual(arg);
	pvars = vartable_matchingVars(vars, nameNode_simpleId(name), qual);
	termAttr_possibleVars(arg) = pvars;
	/* Find possible sorts for quantified subterm */
	list_pointToNext(termNode, args);
	arg = list_current(termNode, args);
	ss = findPossibleSorts(arg, stab, vars);
	if (set_size(Sort, ss) == 0) termAttr_kind(arg) = BADTERM;
	return ps;
    } else {
	termAttr_kind(t) = OPTERM;
    }
    if (node_exists(args)) {
	bool 	 fail = FALSE;
	SortSet *noSorts;
	list_traverse(termNode, args, arg) {
	    ss = findPossibleSorts(arg, stab, vars);
	    if (set_size(Sort, ss) == 0) {
		fail = TRUE;
		noSorts = ss;
	    } else {
		list_addh(SortSet, argSorts, ss);
	    }
	} end_traverse;
	if (fail) {
	    termAttr_kind(t) = BADTERM;
	    return noSorts;
	}
    }
    pops = symtable_opsWithLegalDomain(stab, name, argSorts, qual);
    termAttr_possibleOps(t) = pops;
    set_traverse(operator, pops, op) {
	signatureNode *sig;
	sig = operator_signature(op);
	set_insert(Sort, ps, signatureNode_range(sig));
    } end_traverse;
    if (set_size(Sort, ps) == 0) {
	if (termAttr_kind(t) == OPTERM) {
	    char buf[512];
	    int arity = list_size(termNode, args);
	    if (symtable_opExistsWithArity(stab, name, arity)) {
		int pn = 0;
		badName(name, "not declared with matching domain sorts");
		list_traverse(SortSet, argSorts, ss) {
		    sprintf(buf, "Possible sorts for arg %d: ", ++pn);
		    printSorts(buf, ss);
		} end_traverse;
	    } else {
		sprintf(buf, "undeclared %d-ary operator", arity);
		badName(name, buf);
	    }
	} else {
	    badName(name, "undeclared constant or variable");
	}
	termAttr_kind(t) = BADTERM;
    }
    return ps;
}

static LSLToken
*makeToken (LSLTokenCode cod, char *text, LSLToken loc) {
    LSLToken *t = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
    *t = LSLInsertToken(cod, LSLString2Symbol(text), 0, FALSE);
    token_setCol(*t, token_col(loc));
    token_setLine(*t, token_line(loc));
    token_setFileName(*t, token_fileName(loc));
    return t;
}


static void
printSorts (char *label, SortSet *ss) {
    bool printed = FALSE;
    print_setIndent(8);
    print_str(label);
    print_setIndent(12);
    set_traverse(Sort, ss, sort) {
	if (printed) print_str(", ");
	printed = TRUE;
	unparse_sort(sort);
    } end_traverse;
    print_newline(1);
    print_setIndent(0);
}

sortNode		
*tagSortNode(sortNode *n) {
    char *id = string_paste(token_text(*sortNode_id(n)), "_tag");
    return makeSortNode(makeToken(SIMPLEID, id, n->loc), sortNode_subsorts(n));
}


static OpSet
*termOps (termNode *t) {
    OpSet *ops;
    if (!node_exists(t->attr)) {
	error_programFailure("termOps: unattributed term"); 
    }
    switch (termAttr_kind(t)) {
    case OPTERM:
	ops = termAttr_possibleOps(t);
	if (termNode_args(t) != (termNodeList *)0) {
	    list_traverse(termNode, termNode_args(t), t1) {
		if (termAttr_kind(t1) == OPTERM) {
		    ops = set_union(operator, ops, termOps(t1));
		}
	    }
	} end_traverse;
	break;
    case VARTERM:
	ops = OpSet_new();
	break;
    default:
	error_programFailure("termOps: improperly checked term"); 
    }
    return ops;
}


static void
attributeOps (operatorNodeList *opn, OpList *ops) {
    list_pointToFirst(operator, ops);
    list_traverse(operatorNode, opn, op) {
	if (!node_exists(op->attr)) {
	    allocAttr(operator, op);
	    op->attr->op = list_current(operator, ops);
	}
	list_pointToNext(operator, ops);
    } end_traverse;
}
