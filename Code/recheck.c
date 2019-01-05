/*
**
**	Copyright (c) MIT, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   rechecking.c
**
**	PURPOSE:    Context-sensitive checking for referenced traits
**
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 92.02.23 -- Original
**      {1} Garland at MIT   -- 92.06.14 -- Composite sorts (minor changes)
**	{2} Garland at MIT   -- 95.01.24 -- Fix "stackSize + 1 >= stackAlloc"
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include	"general.h"
# include 	"osdfiles.h"
# include 	"string2sym.h"
# include	"token.h"
# include 	"error.h"
# include	"source.h"
# include	"lsl.h"
# include	"list.h"
# include	"set.h"
# include	"tokentable.h"
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


/*
**
**  MACRO DEFINITIONS
**
*/

# define DELTA  10


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

extern void	recheckTraitRefs ();
extern void	recheck_error ();
extern void	recheck_sortError ();

static void	recheckTraitRef (symtable *st, traitRefNode *ref);
static void	recheckTuple (tupleNode *tu);
static void	recheckUnion (unionNode *u);
static void	recheckPropPart (symtable *st, propPartNode *ppn);
static void	recheckGenPartBys (symtable *st, genPartByNodeList *gl);
static void	recheckGeneratedBy (symtable *st, genByNode *g);
static void	recheckQuantEqns (symtable *st, quantEqnsNodeList *qes);






/*
**
**  STATIC VARIABLES
**
*/

static traitRefNode    **refs		= (traitRefNode **)0;
static renaming        **renamings	= (renaming **)0;
static unsigned int 	 stackAlloc 	= 0;
static unsigned int 	 stackSize 	= 0;


/*
**
**  FUNCTION DEFINITIONS
**
*/


extern void 
recheckTraitRefs (symtable *st, bool reset, traitRefNodeList *refs) {
    if (reset) stackSize = 0;
    list_traverse(traitRefNode, refs, ref) {
	recheckTraitRef(st, ref);
    } end_traverse;
}


extern void
recheck_error (void) {
    int i;
    for (i = 1; i <= stackSize; i++) {
	badNode(refs[i], "error in traitRef");
    }
}


extern void
recheck_sortError (void) {
    int i;
    for (i = 1; i <= stackSize; i++) {
	badNode(refs[i], "error in traitRef caused by renamed sorts");
    }
}


static void
recheckTraitRef (symtable *st, traitRefNode *ref) {
    if (stackSize + 1 >= stackAlloc) {
	stackAlloc += DELTA;
	refs = 
	    (traitRefNode **)LSLMoreMem(refs, stackAlloc*sizeof(traitRefNode *));
	renamings = 
	    (renaming **)LSLMoreMem(renamings, stackAlloc*sizeof(renaming *));
    }
    refs[++stackSize] = ref;
    if (stackSize == 1) renamings[stackSize] = traitRefAttr_rename(ref);
    list_traverse(traitNode, traitRefAttr_traits(ref), t) {
	if (traitAttr_axiomsCorrect(t) != CORRECT) continue;
	if (stackSize > 1 &&(node_exists(traitNode_exts(t))
			     || node_exists(traitNode_asserts(t)))) {
	    renamings[stackSize] = renaming_compose(renamings[stackSize-1],
						    traitRefAttr_rename(ref),
						    traitAttr_symtab(t));
	}
	if (node_exists(traitNode_exts(t))) {
	    list_traverse(extNode, traitNode_exts(t), en) {
		switch (extNode_kind(en)) {
		case LSLenumKIND:
		    break;
		case LSLtupleKIND:
		    recheckTuple(extNode_LSLtuple(en));
		    break;
		case LSLunionKIND:
		    recheckUnion(extNode_LSLunion(en));
		    break;
		case assumesKIND:
		    recheckTraitRefs(st, FALSE,
				     assumesNode_traits(extNode_includes(en)));
		    break;
		case includesKIND:
		    recheckTraitRefs(st, FALSE,
				     includesNode_traits(extNode_includes(en)));
		    break;
		}
	    } end_traverse;
	}
	if (node_exists(traitNode_asserts(t))) {
	    list_traverse(propPartNode, traitNode_asserts(t), ppn) {
		recheckPropPart(st, ppn);
	    } end_traverse;
	}
    } end_traverse;
    stackSize--;
}


static void
recheckTuple (tupleNode *tu) {
    Sort *S1, *S2, *S3;
    renaming *r = renamings[stackSize];
    if (renaming_nSorts(r) == 0) return;
    S1 = renaming_mapSort(r, tupleNode_sort(tu));
    S3 = (Sort *)0;
    list_traverse(operator, tupleAttr_gets(tu), op) {
	S2 = signatureNode_range(operator_signature(op));
	if (S3 != (Sort *)0 && sameSort(S2, S3)) continue;
	if (sameSort(S1, renaming_mapSort(r, S2))) {
	    recheck_sortError();
	    badNode(S2, "field sort must differ from tuple sort");
	}
	S3 = S2;
    } end_traverse;
}


static void
recheckUnion (unionNode *u) {
    Sort *S1, *S2, *S3;
    renaming *r = renamings[stackSize];
    if (renaming_nSorts(r) == 0) return;
    S1 = renaming_mapSort(r, tupleNode_sort(u));
    S3 = (Sort *)0;
    list_traverse(operator, unionAttr_fromS(u), op) {
	S2 = signatureNode_range(operator_signature(op));
	if (S3 != (Sort *)0 && sameSort(S2, S3)) continue;
	if (sameSort(S1, renaming_mapSort(r, S2))) {
	    recheck_sortError();
	    badNode(S2, "field sort must differ from union sort");
	}
	S3 = S2;
    } end_traverse;
}


static void
recheckPropPart (symtable *st, propPartNode *ppn) {
    recheckGenPartBys(st, propPartNode_genPartBys(ppn));
    recheckQuantEqns(st, propPartNode_quantEqns(ppn));
}


static void
recheckGenPartBys (symtable *st, genPartByNodeList *gl) {
    renaming *r = renamings[stackSize];
    if (node_null(gl) || renaming_nSorts(r) == 0) return;
    list_traverse(genPartByNode, gl, gp) {
	switch (genPartByNode_kind(gp)) {
	case genByKIND:
	    recheckGeneratedBy(st, genPartByNode_genBy(gp));
	    break;
	case partByKIND:
	    break;
	}
    } end_traverse;
}


static void
recheckGeneratedBy (symtable *st, genByNode *g) {
    renaming *r = renamings[stackSize];
    Sort *S1 = renaming_mapSort(r, genByNode_sort(g));
    list_traverse(operator, genByAttr_ops(g), op) {
	bool basis;
	basis = TRUE;
	list_traverse(Sort, signatureNode_domain(operator_signature(op)), S2) {
	    if (sameSort(S1, renaming_mapSort(r, S2))) {
		basis = FALSE;
		break;
	    }
	} end_traverse;
	if (basis) return;
    } end_traverse;
    recheck_sortError();
    badLoc(node_loc(g), 
	   "all generators have the generated sort in their domains");
}


static void
recheckQuantEqns (symtable *st, quantEqnsNodeList *qes) {
    if (node_null(qes)) return;
    list_traverse(quantEqnsNode, qes, qe) {
	vartable_recheck(quantEqnsAttr_vars(qe), st, renamings[stackSize]);
    } end_traverse;
}



/* !!! IMPLEMENT: recheckConsequences */
