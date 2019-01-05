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
**      FILENAME:   symtable.c
**
**	PURPOSE:    Symbol table abstraction
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT      -- 90.10.02 -- Original
**	{1} Wild    at Digital  -- 90.11.09 -- Include osdfiles.h
**	{2} Wild    at Digital  -- 90.12.10 -- Unparse sorts with symbol table.
**      {3} Garland at MIT      -- 91.01.29 -- Add symtable_printSorts
**	{4} Wild    at Digital  -- 91.02.20 -- Merge in MIT changes.
**	{5} Wild    at Digital  -- 91.02.22 -- Change stdio to stdio.h. gcc
**					       complained.
**	{6} Garland at MIT      -- 91.12.20 -- Change symtable_unparse to
**					       symtable_print2LP, 
**					       add symtable_opExists
**	{7} Garland at MIT      -- 92.01.09 -- Recode as hash table, detect 
**					       overloaded ids, add rename4LP
**	{8} Garland at MIT	-- 92.01.12 -- Combine rename4LP, print2LP;
**					       add symtable_contains
**      {9} Garland at MIT      -- 92.01.12 -- Restore symtable_unparse
**     {10} Garland at MIT	-- 92.02.12 -- Add symtable_renamingWork, free
**     {11} Garland at MIT	-- 92.02.16 -- Add DecimalLiterals
**     {12} Garland at MIT      -- 92.02.19 -- Use error_reportBadToken
**     {13} Garland at MIT      -- 92.02.23 -- Fix uses of nameNodeMatches
**     {14} Garland at MIT      -- 92.06.14 -- Composite sort notation A[B]
**     {15} Garland at MIT      -- 92.09.30 -- Add symtable_opExistsWithArity
**     {16} Garland at MIT      -- 93.04.09 -- Print __'s in "% Renaming for"
**     {17} Garland at MIT      -- 93.09.23 -- Change for LP3.1
**     {18} Garland at MIT      -- 93.10.27 -- Have sortExists find S in S$T
**     {19} Garland at MIT      -- 93.11.02 -- Fixed DecimalLiteral for LP3.1
**     {20} Garland at MIT      -- 93.11.09 -- Added quantifiers
**     {21} Garland at MIT      -- 93.11.11 -- Accumulate symbols in print2LP
**     {22} Garland at MIT      -- 93.11.26 -- Add iff
**     {23} Garland at MIT      -- 93.12.06 -- Add #include for clean compile
**     {24} Garland at MIT      -- 94.04.14 -- Print commas in LP op decs
**     {25} Garland at MIT      -- 94.05.23 -- Composite sort notation A[B, C]
**      {n} Who     at Where    -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include <string.h>
# include	"general.h"
# include	"osdfiles.h"
# include	STDLIB
# include	"string2sym.h"
# include	"token.h"
# include	"error.h"
# include	"list.h"
# include	"set.h"
# include	"tokentable.h"
# include	"parse.h"
# include	"abstract.h"
# include	"operator.h"
# include	"renaming.h"
# include	"symtable.h"
# include	"vartable.h"
# include	"print.h"
# include	"unparse.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define HTSIZE	 (HASHMASK + 1)
# define badTok  error_reportBadToken


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

extern void		 symtable_init(void);
extern symtable		*symtable_blank();
extern symtable		*symtable_new();
extern void		 symtable_free();
extern void		 symtable_renamingWork();
extern bool		 symtable_contains();
extern void	 	 symtable_declareOp();
extern void	 	 symtable_declareOpAndSorts();
extern void	 	 symtable_declareSort();
extern void		 symtable_enableLiterals();
extern void		 symtable_extend();
extern bool		 symtable_isLiteral();
extern bool		 symtable_legalVar();
extern bool		 symtable_markOverloadedConstants();
extern OpSet		*symtable_matchingOps();
extern bool	 	 symtable_opExists();
extern bool		 symtable_opExistsWithArity();
extern OpSet		*symtable_opsWithLegalDomain();
extern bool		 symtable_overloaded();
extern bool		 symtable_overloadedDomain();
extern bool		 symtable_sortExists();
extern void		 symtable_printSorts();
extern renaming		*symtable_print2LP();
extern void		 symtable_unparse();

static bool		 rename4LP (operator *op, renaming *r);
static signatureNode	*makeBoolSig(int n);
static bool		 sameDomain (signatureNode *sig1, signatureNode *s2);
static opBucket         *findBucket (symtable *t, nameNode *n, 
				     signatureNode *sig);
extern bool		 isLiteralToken (LSLToken *t);
extern bool		 isLiteralName (nameNode *n);
static void		 symtable_declareLiteral (symtable *t, nameNode *n, 
						  signatureNode *sig,
						  OpSet *ops);


/*
**
**  STATIC VARIABLES
**
*/

static signatureNode	*boolSig0 = (signatureNode *)0;
static signatureNode	*boolSig1;
static signatureNode	*boolSig2;

static unsigned int 	 prefixCount = 0;
static unsigned int 	 postfixCount = 0;
static unsigned int 	 mixfixCount = 0;
static unsigned int	 constantCount = 0;
static unsigned int	 selectCount = 0;


/*
**
**  FUNCTION DEFINITIONS
**
*/

extern symtable
*symtable_blank (void) {
    int       i;
    symtable *t = (symtable *)LSLMoreMem((void *)0, sizeof(symtable));
    t->sorts = SortSet_new();
    t->plus1 = OpSet_new();
    t->literals = OpSet_new();
    t->buckets = (opBucket *)LSLMoreMem((void *)0, HTSIZE*sizeof(opBucket));
    t->nOps = 0;
    for (i = 0; i < HTSIZE; i++) {
	opBucket *b;
	b = &t->buckets[i];
        b->op = (operator *)0;
    }
    return t;
}


extern symtable
*symtable_new (void) {
    symtable *t = symtable_blank();
    symtable_declareSort(t, makeBoolSort());
    symtable_declareOp(t, makeFalseOpNode(), makeBoolSig(0));

    symtable_declareOp(t, makeTrueOpNode(), makeBoolSig(0));
    symtable_declareOp(t, makeNotOpNode(), makeBoolSig(1));
    symtable_declareOp(t, makeAndOpNode(), makeBoolSig(2));
    symtable_declareOp(t, makeOrOpNode(), makeBoolSig(2));
    symtable_declareOp(t, makeImpliesOpNode(), makeBoolSig(2));
    symtable_declareOp(t, makeIffOpNode(), makeBoolSig(2));
    return t;
}


extern void
symtable_free (symtable *t) {
    int i;
    if (t == (symtable *)0) return;
    set_free(Sort, t->sorts);
    set_free(operator, t->plus1);
    for (i = 0; i < HTSIZE; i++) {
	opBucket *b, *next;
	for (b = t->buckets[i].next; b != (opBucket *)0; b = next) {
	    next = b->next;
	    free(b);
	}
    }
    free(t->buckets);
    free(t);
}


extern void
symtable_declareOp (symtable *t, nameNode *n, signatureNode *sig) {
    Sort       *range 		 = signatureNode_range(sig);
    opBucket   *b     		 = &t->buckets[node_key(n)];
    bool        overloaded       = FALSE;
    bool        overloadedDomain = FALSE;
    if (b->op != (operator *)0) {
	for (;; b = b->next) {
	    if (nameNodeMatches(n, operator_name(b->op), FALSE)) {
		signatureNode *sig1 = operator_signature(b->op);
		if (sameDomain(sig, sig1)) {
		    if (sameSort(range, signatureNode_range(sig1))) return;
		    overloadedDomain = TRUE;
		    b->overloadedDomain = TRUE;
		}
		overloaded = TRUE;
		b->overloaded = TRUE;
	    }
	    if (b->next == (opBucket *)0) break;
	}
	b->next = (opBucket *)LSLMoreMem((void *)0, sizeof(opBucket));
	b = b->next;
    }
    b->op = operator_new(n, sig);
    b->overloaded = overloaded;
    b->overloadedDomain = overloadedDomain;
    t->nOps++;
}


extern void
symtable_declareOpAndSorts (symtable *t, nameNode *n, signatureNode *sig) {
    list(Sort) *dom   		 = signatureNode_domain(sig);
    if (dom != 0) {
	list_traverse(Sort, dom, s) {
	    symtable_declareSort(t, s);
	} end_traverse;
    }
    symtable_declareSort(t, signatureNode_range(sig));
    symtable_declareOp(t, n, sig);
}


extern void
symtable_declareSort (symtable *t, Sort *s) {
    if (set_is_element(Sort, s, t->sorts)) return;
    set_insert(Sort, t->sorts, s);
    symtable_declareOp(t, makeEqOpNode(), 
		       makeSignature2(s, s, makeBoolSort()));
    symtable_declareOp(t, makeNeqOpNode(), 
		       makeSignature2(s, s, makeBoolSort()));
    symtable_declareOp(t, makeAllOpNode(),
		       makeSignature2(s, makeBoolSort(), makeBoolSort()));
    symtable_declareOp(t, makeExistsOpNode(),
		       makeSignature2(s, makeBoolSort(), makeBoolSort()));
    symtable_declareOp(t, makeIfOpNode(),
		       makeSignature3(makeBoolSort(), s, s, s));
}



/*
** Invokes "renaming_composeWork(r, r1, r2, op)" for every operator "*op" in
** "*t2".
*/

extern void
symtable_renamingWork (renaming *r, renaming *r1, renaming *r2, symtable *t2) {
    int 	i;
    opBucket   *b;
    if (t2->nOps > 0) {
	for (i = 0; i < HTSIZE; i++) {
	    b = &t2->buckets[i];
	    if (b->op == (operator *)0) continue;
	    for (; b != (opBucket *)0; b = b->next)
		renaming_composeWork(r, r1, r2, b->op);
	}
    }
}


/* 
** Checks that "*t1" contains the operators in "*t2" renamed by "*r".  Reports
** any missing operators as being undefined in the trait containing the
** implication at location "*loc".
*/

extern bool
symtable_contains (symtable *t1, symtable *t2, renaming *r, LSLToken *loc) {
    opBucket *b;
    operator *op;
    int	      i;
    bool      missing = FALSE;
    for (i = 0; i < HTSIZE; i++) {
	b = &t2->buckets[i];
	if (b->op == (operator *)0) continue;
	for (; b != (opBucket *)0; b = b->next) {
	    op = renaming_mapOp(r, b->op);
	    if (!symtable_opExists(t1, operator_name(op), 
				   operator_signature(op))) {
		LSLToken tok = node_loc(operator_name(b->op));
		badTok(loc, "implication of trait containing new operator");
		error_reportLocation(token_fileName(tok), token_line(tok),
				     token_col(tok));
		print_char('`');
		unparse_name(operator_name(b->op));
		print_str("'");
		print_newline(0);
		error_report(" is the new operator");
		/* ENHANCE: show likely candidates, e.g., postfix - */
		/* rather than prefix -                             */
		missing = TRUE;
	    }
	}
    }
    set_traverse(operator, t2->plus1, plus1) {
	if (!set_is_element(operator, renaming_mapOp(r, plus1), t1->plus1)) {
	    badTok(loc, "implied trait contains new DecimalLiterals");
	    missing = TRUE;
	}
    } end_traverse;
    return missing;
}


extern void
symtable_enableLiterals (symtable *t, operator *plus1) {
    set_insert(operator, t->plus1, plus1);
}


/* 
** Modifies "*t1" by adding the symbols in "*t2" renamed by "*r".
*/

extern void
symtable_extend (symtable *t1, symtable *t2, renaming *r) {
    opBucket *b;
    operator *op;
    int	      i;
    for (i = 0; i < HTSIZE; i++) {
	b = &t2->buckets[i];
	if (b->op == (operator *)0) continue;
	for (; b != (opBucket *)0; b = b->next) {
	    op = renaming_mapOp(r, b->op);
	    symtable_declareOp(t1, operator_name(op), operator_signature(op));
	}
    }
    set_traverse(operator, t2->plus1, plus1) {
	set_insert(operator, t1->plus1, renaming_mapOp(r, plus1));
    } end_traverse;
    if (renaming_nSorts(r) == 0) {
	t1->sorts = set_union(Sort, t1->sorts, t2->sorts);
    } else {
	set_traverse(Sort, t2->sorts, s) {
	    set_insert(Sort, t1->sorts, renaming_mapSort(r, s));
	} end_traverse;
    }
}


extern bool
symtable_isLiteral (symtable *t, operator *op) {
    Sort *range;
    if (   operator_arity(op) > 0 
	|| !isLiteralName(operator_name(op))
	|| set_size(operator, t->plus1) == 0)
	return FALSE;
    range = signatureNode_range(operator_signature(op));
    set_traverse(operator, t->plus1, plus1) {
 	if (sameSort(range, signatureNode_range(operator_signature(plus1))))
	    return TRUE;
   } end_traverse;
    return FALSE;
}


extern bool
symtable_legalVar (symtable *t, varIdLeaf *v, Sort *s) {
    nameNode *n;
    opBucket *b;
    if (isLiteralToken(v) && !set_size(operator, t->plus1) == 0) return FALSE;
    n = makeSimpleIdNameNode(v);
    b = &t->buckets[node_key(n)];
    if (b->op == (operator *)0) return TRUE;
    for (; b != (opBucket *)0; b = b->next) {
	if (nameNodeMatches(n, operator_name(b->op), FALSE) 
	    && operator_arity(b->op) == 0
	    && sameSort(signatureNode_range(operator_signature(b->op)), s))
	    return FALSE;
    }
    return TRUE;
}

extern bool
symtable_opExists (symtable *t, nameNode *n, signatureNode *sig) {
    if (isLiteralName(n) && list_size(Sort, signatureNode_domain(sig)) == 0) {
	Sort *range = signatureNode_range(sig);
	set_traverse(operator, t->plus1, plus1) {
	    if (sameSort(range, 
			 signatureNode_range(operator_signature(plus1)))) 
		return TRUE;
	} end_traverse;
    }
    return findBucket(t, n, sig) != (opBucket *)0;
}

extern bool
symtable_opExistsWithArity (symtable *t, nameNode *n, int arity) {
    opBucket   *b     = &t->buckets[node_key(n)];
    if (b->op == (operator *)0) return FALSE;
    for (; b != (opBucket *)0; b = b->next) {
	/* Perform the quick checks first */
	if (arity == operator_arity(b->op) &&
	    nameNodeMatches(n, operator_name(b->op), FALSE)) { 
	    return TRUE;
	}
    }
    return FALSE;
}

extern OpSet
*symtable_opsWithLegalDomain (symtable *t, nameNode *n, 
			      list(SortSet) *argSorts, Sort *qual) {
    int 	 arity 	= list_size(Sort, argSorts);
    OpSet	*ops 	= OpSet_new();
    opBucket    *b      = &t->buckets[node_key(n)];
    if (b->op != (operator *)0) {
	for (; b != (opBucket *)0; b = b->next) {
	    operator *op;
	    op = b->op;
	    if (arity == operator_arity(op) &&
		nameNodeMatches(n, op->name, FALSE) &&
		(qual == (Sort *)0 || 
		 sameSort(qual, signatureNode_range(op->signature)))) {
		list(Sort) *dom  = signatureNode_domain(op->signature);
		bool	    ok   = TRUE;
		list_pointToFirst(SortSet, argSorts);
		list_traverse(Sort, dom, s) {
		    if (set_is_element(Sort, s, list_current(SortSet, argSorts))) {
			list_pointToNext(SortSet, argSorts);
		    } else {
			ok = FALSE;
			break;
		    } 
		} end_traverse;
		if (ok) {	/* ENHANCE: set_insertUnique */
		    set_insert(operator, ops, op);
		}
	    }
	}
    }
    if (isLiteralName(n) && arity == 0 && set_size(operator, t->plus1) > 0) {
	set_traverse(operator, t->plus1, plus1) {
	    Sort *s1;
	    s1 = signatureNode_range(operator_signature(plus1));
	    symtable_declareLiteral(t, n, makeSignature0(s1), ops);
	} end_traverse;
    }
    return ops;
}


extern bool 
symtable_markOverloadedConstants (symtable *t, varIdLeaf *v) {
    nameNode *n1     = makeSimpleIdNameNode(v);
    opBucket *b      = &t->buckets[node_key(n1)];
    bool      marked = FALSE;
    if (b->op == (operator *)0) return FALSE;
    for (; b != (opBucket *)0; b = b->next) {
	operator *op;
	nameNode *n2;
	op = b->op;
	n2 = operator_name(op);
	if (nameNode_kind(n2) == simpleIdKIND && operator_arity(op) == 0 &&
	    LSLSameLeaf(nameNode_simpleId(n2), v)) {
	    b->overloadedDomain = TRUE;
	    marked = TRUE;
	}
    }
    return marked;
}


extern OpSet
*symtable_matchingOps (symtable *t, nameNode *n, signatureNode *sig) {
    OpSet 	  *ops 		= OpSet_new();
    opBucket      *b            = &t->buckets[node_key(n)];
    if (b->op != (operator *)0) {
	for (; b != (opBucket *)0; b = b->next) {
	    operator *op;
	    op = b->op;
	    if ((!node_exists(sig) || LSLSameSignature(sig, op->signature)) &&
		nameNodeMatches(n, op->name, TRUE)) {
		/* ENHANCE: set_insertUnique */
		set_insert(operator, ops, op);
	    }
	}
    }
    if (isLiteralName(n) && set_size(operator, t->plus1) > 0) {
	Sort *s     = (Sort *)0;
	bool  check = TRUE;
	if (node_exists(sig)) {
	    if (list_size(Sort, signatureNode_domain(sig)) == 0) {
		s = signatureNode_range(sig);
	    } else {
		check = FALSE;
	    }
	}
	if (check) {
	    set_traverse(operator, t->plus1, plus1) {
		Sort *s1;
		s1 = signatureNode_range(operator_signature(plus1));
		if (s == (Sort *)0 || sameSort(s, s1)) {
		    if (!node_exists(sig)) sig = makeSignature0(s1);
		    symtable_declareLiteral(t, n, sig, ops);
		}
	    } end_traverse;
	}
    }
    return ops;
}


extern bool
symtable_overloaded (symtable *t, operator *op) {
    opBucket *b = findBucket(t, operator_name(op), operator_signature(op));
    if (isLiteralName(operator_name(op)) && set_size(operator, t->plus1) > 1)
	return TRUE;
    if (b == (opBucket *)0) return FALSE;
    return b->overloaded;
}

extern bool
symtable_overloadedDomain (symtable *t, operator *op) {
    opBucket *b = findBucket(t, operator_name(op), operator_signature(op));
    if (isLiteralName(operator_name(op)) && set_size(operator, t->plus1) > 1)
	return TRUE;
    if (b == (opBucket *)0) return FALSE;
    return b->overloadedDomain;
}


extern bool
symtable_sortExists (symtable *t, Sort *s) {
  set_traverse(Sort, t->sorts, s1) {
    if (LSLSubsort(s, s1)) return TRUE;
  } end_traverse;
  return FALSE;
}


extern void
symtable_printSorts (symtable *t) {
    print_closeFile();	/* ensure that output goes to stdout */
    set_traverse(Sort, t->sorts, s) {
	unparse_sort(s);
	print_newline(1);
   } end_traverse;
}


extern renaming
*symtable_print2LP (symtable *t, renaming *r) {
    int 	i;
    opBucket   *b;
    bool 	printed;
    prefixCount = 0;
    postfixCount = 0;
    mixfixCount = 0;
    constantCount = 0;
    selectCount = 0;
    unparse_addSymbols(t);
    if (set_size(Sort, t->sorts) > 1) {   /* ignore "Bool" */
	print_str("declare sorts");
	print_newline(1);
	print_setIndent(2);
	printed = FALSE;
	set_traverse(Sort, t->sorts, e) {
	    if (sameSort(e, makeBoolSort())) continue;
	    if (printed) print_str(", ");
	    unparse_sort(e);
	    printed = TRUE;
	} end_traverse;
	print_newline(1);
	print_str("..");
	print_newline(2);
	print_setIndent(0);
    }
    if (t->nOps > 0) {
	print_str("declare operators");
	print_newline(1);
	print_setIndent(2);
	printed = FALSE;
	for (i = 0; i < HTSIZE; i++) {
	    operator *op;
	    b = &t->buckets[i];
	    if (b->op == (operator *)0) continue;
	    for (; b != (opBucket *)0; b = b->next) {
		if (operator_kind(b->op) != userOP) continue;
		op = renaming_mapOp(r, b->op);
		if (op == b->op && rename4LP(op, r)) {
		    print_setPrefix("% Renaming for ");
		    unparse_setForLP(FALSE);
		    operator_unparse(op);
		    unparse_setForLP(TRUE);
		    print_newline(1);
		    print_setPrefix("");
		    op = renaming_mapOp(r, op);
		}
		if (printed) print_str(", ");
		printed = TRUE;
		operator_unparse(op);
		print_newline(1);
	    }
	}
	print_str("..");
	print_newline(2);
	print_setIndent(0);
    }
    printed = FALSE;
    set_traverse(operator, t->literals, op) {
	char *digits, *buf;
	digits = token_text(*nameNode_simpleId(operator_name(op)));
	if (strlen(digits) < 2) continue;
	buf = (char *)LSLMoreMem((char *)0, (sizeof(digits)+1)*sizeof(char));
	strcpy(buf, digits);
	buf[strlen(digits)-1] = '\0';
	if (!printed) {
	    print_str("% Numerals");
	    print_newline(2);
	    print_str("set name DecimalLiterals");
	    print_newline(1);
	    print_str("set activity off");
	    print_newline(1);
	    print_str("set immunity on");
	    print_newline(1);
	    print_str("assert");
	    print_newline(1);
	    print_setIndent(2);
	    printed = TRUE;
	}
	print_str(digits);
	if (symtable_overloaded(t, op)) {
	    print_str(": ");
	    unparse_sort(signatureNode_range(operator_signature(op)));
	}
	print_str(" = times10plus(");
	print_str(buf);
	print_str(", ");
	print_str(digits + strlen(digits) - 1);
	print_str(");");
	print_newline(1);
	free(buf);
    } end_traverse;
    if (printed) {
	print_str("..");
	print_newline(1);
	print_setIndent(0);
	print_str("set activity on");
	print_newline(1);
	print_str("set immunity off");
	print_newline(2);
    }
    return r;
}


extern void
symtable_unparse (symtable *t) {
    int 	i;
    opBucket   *b;
    bool 	printed;
    if (t->nOps > 0) {
	printed = FALSE;
	for (i = 0; i < HTSIZE; i++) {
	    b = &t->buckets[i];
	    if (b->op == (operator *)0) continue;
	    for (; b != (opBucket *)0; b = b->next) {
/* 		if (operator_kind(b->op) != userOP) continue;  */
	      operator_unparse(b->op);
	      print_newline(1);
	    }
	  }
    }
}


/*
** Adds a renaming for "*op" to "*r" if the name of "*op" cannot be parsed
** by LP.  Ensures that renamings are not overloaded, so that qualifications
** will not be needed when producing output containing renamed operators.
** (If overloadings are retained, the renamed operators must be added to the
** symbol table.)
*/

static bool
rename4LP (operator *op, renaming *r) {
    nameNode   		*n;
    char       		 buf[10];
    char		*id;
    LSLToken		*tok;
    n = operator_name(op);
    switch (nameNode_kind(n)) {
    case simpleIdKIND:
    case ifOpKIND:
    case ppiOpKIND:
    case quantifierKIND:
	return FALSE;
    case mixfixOpKIND:
	/* FIX: Check that openSym and closeSym are parsable by LP. */
	return FALSE;
	if (operator_arity(op) == 0) {
	    sprintf(buf, "%d", ++constantCount);
	    id = string_paste("__constant", buf);
	} else {
	    sprintf(buf, "%d", ++mixfixCount);
	    id = string_paste("__mixfix", buf);
	}
	break;
    case selectOpKIND:
	return FALSE;
	sprintf(buf, "%d", ++selectCount);
	id = string_paste(string_paste("__sel", buf), "_");
	id = string_paste(id, 
			  token_text(*selectOpNode_sel(nameNode_selectOp(n))));
	break;
    }
    tok = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
    *tok = LSLInsertToken(SIMPLEID, LSLString2Symbol(id), 0, FALSE);
    renaming_addOp(r, op, makeSimpleIdNameNode(tok));
    return TRUE;
}


static signatureNode
*makeBoolSig (int n) {
    if (boolSig0 == (signatureNode *)0) {
	Sort *b = makeBoolSort();
	boolSig0 = makeSignature0(b);
	boolSig1 = makeSignature1(b, b);
	boolSig2 = makeSignature2(b, b, b);
    }
    switch (n) {
    case 0: return boolSig0;
    case 1: return boolSig1;
    case 2: return boolSig2;
    default: error_programFailure("makeBoolSig assumption failed");
    }
}


static bool
sameDomain (signatureNode *sig1, signatureNode *sig2) {
    list(Sort)	*dom1 = signatureNode_domain(sig1);
    list(Sort)	*dom2 = signatureNode_domain(sig2);
    if (list_size(Sort, dom1) != list_size(Sort, dom2)) return FALSE;
    list_pointToFirst(Sort, dom1);
    list_traverse(Sort, dom2, s) {
	if (!LSLSameSort(list_current(Sort, dom1), s)) return FALSE;
	list_pointToNext(Sort, dom1); 
    } end_traverse;
    return TRUE;
}


static opBucket
*findBucket (symtable *t, nameNode *n, signatureNode *sig) {
    list(Sort) *dom   = signatureNode_domain(sig);
    Sort       *range = signatureNode_range(sig);
    int   	arity = list_size(Sort, dom);
    opBucket   *b     = &t->buckets[node_key(n)];
    if (b->op == (operator *)0) return (opBucket *)0;
    for (; b != (opBucket *)0; b = b->next) {
	/* Perform the quick checks first */
	if (arity == operator_arity(b->op) &&
	    nameNodeMatches(n, operator_name(b->op), FALSE) &&
	    sameSort(range, signatureNode_range(operator_signature(b->op))) &&
	    sameDomain(sig, operator_signature(b->op))) {
	    return b;
	}
    }
    return (opBucket *)0;
}


static void
dump (symtable *t) {
    int i;
    for (i = 0; i < HTSIZE; i++) {
	opBucket *b;
	b = &t->buckets[i];
        if (b->op == (operator *)0) continue;
	printf("Bucket %d\n", i);
	for (; b != (opBucket *)0; b = b->next) {
	    operator_unparse(b->op);
	    print_newline(1);
	    if (b->overloaded) puts("     Overloaded");
	    if (b->overloadedDomain) puts("     Overloaded domain");
	}
    }
}


extern bool
isLiteralToken (LSLToken *t) {
    char *s = token_text(*t);
    return strspn(s, "0123456789") == strlen(s);
}


extern bool
isLiteralName (nameNode *n) {
    return (nameNode_kind(n) == simpleIdKIND && 
	    isLiteralToken(nameNode_simpleId(n)));
}


static void
symtable_declareLiteral (symtable *t, nameNode *n, signatureNode *sig,
			 OpSet *ops) {
    char *digits = token_text(*nameNode_simpleId(n));
    char *buf;
    operator *op;
    symtable_declareOp(t, n, sig);
    op = operator_new(n, sig);
    set_insert(operator, ops, op);
    set_insert(operator, t->literals, op);
    if (strlen(digits) <= 1) return;
    buf = (char *)LSLMoreMem((char *)0, (strlen(digits)+1)*sizeof(char));
    strcpy(buf, digits);
    for (buf[strlen(buf)-1] = '\0'; *buf != '\0'; buf[strlen(buf)-1] = '\0') {
	LSLToken *id;
	id = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
	*id = LSLInsertToken(SIMPLEID, LSLString2Symbol(buf), 0, FALSE);
	token_setFileName(*id, token_fileName(*nameNode_simpleId(n)));
	token_setLine(*id, token_line(*nameNode_simpleId(n)));
	token_setCol(*id, token_col(*nameNode_simpleId(n)));
	n = makeSimpleIdNameNode(id);
	op = operator_new(n, sig);
	symtable_declareOp(t, n, sig);
	set_insert(operator, t->literals, op);
    }
    free(buf);
}
