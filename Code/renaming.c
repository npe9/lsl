/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993, 1994, 1995
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   renaming.c
**
**	PURPOSE:    Renaming abstraction for sorts and operators
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.10.30 -- Original
**	{1} Wild    at Digital -- 90.11.09 -- Include osdfiles.h
**	{2} Wild    at Digital -- 90.12.11 -- Add renaming_unparse.
**	{3} Garland at MIT     -- 91.12.20 -- Add lsl2lp support for composing
**                                            applying renamings, change 
**					      renaming_unparse to use print.c,
**					      return same object if not renamed
**	{4} Garland at MIT     -- 92.02.12 -- Fixed renaming_compose
**      {5} Garland at MIT     -- 92.02.19 -- Reorder #include "error.h"
**      {6} Garland at MIT     -- 92.02.23 -- Fix uses of nameNodeMatches,
**					      add renaming_composeSorts
**      {7} Garland at MIT     -- 92.06.14 -- Composite sort notation A$B
**      {8} Garland at MIT     -- 93.11.11 -- Add more guards for (renaming *)0
**      {9} Garland at MIT     -- 93.12.06 -- Add #includes for clean compile
**     {10} Garland at MIT     -- 94.04.20 -- Add "generated freely"
**     {11} Garland at MIT     -- 94.05.23 -- Composite sort notation A[B,C]
**     {12} Garland at MIT     -- 95.01.23 -- Remove renaming_composeSorts
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include	"general.h"
# include	"osdfiles.h"
# include	STDLIB
# include	"string2sym.h"
# include	"token.h"
# include	"error.h"
# include	"list.h"
# include	"set.h"
# include	"parse.h"
# include	"abstract.h"
# include	"operator.h"
# include	"renaming.h"
# include	"symtable.h"
# include	"vartable.h"
# include	"prop.h"
# include	"propSet.h"
# include	"checking.h"
# include	"print.h"
# include	"unparse.h"


/*
**
**  MACRO DEFINITIONS (none)
**
*/


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

extern renaming		*renaming_new();
extern void		 renaming_free();
extern bool	 	 renaming_addSort();
extern bool	 	 renaming_addOp();
extern renaming		*renaming_compose();
extern renaming		*renaming_composeSorts();
extern bool		 renaming_equal();
extern bool		 renaming_isIdentity();
extern equationNode	*renaming_mapEqn();
extern genByNode	*renaming_mapGenBy();
extern operator		*renaming_mapOp();
extern partByNode	*renaming_mapPartBy();
extern Sort		*renaming_mapSort();
extern termNode		*renaming_mapTerm();
extern int		 renaming_nSorts();
extern int		 renaming_nOps();
extern void		 renaming_unparse();

static bool		 sameFromOp(OpRename *or1, OpRename *or2);
static bool		 sameFromSort(SortRename *sr1, SortRename *sr2);
static signatureNode	*mapSig(renaming *r, signatureNode *sig);
static void		 mapTo(renaming *r, OpRename *or);


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


extern renaming
*renaming_new (void) {
    renaming *r = (renaming *)LSLMoreMem((void *)0, sizeof(renaming));
    r->sorts = set_new(SortRename, sameFromSort);
    r->operators = set_new(OpRename, sameFromOp);
    r->okToAddSorts = TRUE;
    return r;
}


extern void
renaming_free (renaming *r) {
    if (r == (renaming *)0) return;
    set_free(SortRename, r->sorts);
    set_free(OpRename, r->operators);
    free(r);
}


/*
** Modifies: r->sorts
** Ensures:
**    (result  = ~(*from \in domain(r->sorts)) & ~(*from = *to)) &&
**    (result  => (r->sorts)' = (r->sorts) \union <*from, *to>)  &&
**    (~result => unchanged(r->sorts))
*/

extern bool
renaming_addSort (renaming *r, Sort *from, Sort *to) {
    SortRename *sr;
    if (!r->okToAddSorts) error_programFailure("renaming_addSort");
    sr = (SortRename *)LSLMoreMem((void *)0, sizeof(SortRename));
    sr->from = from;
    sr->to = to;
    return set_insert(SortRename, r->sorts, sr);
}


extern bool
renaming_addOp (renaming *r, operator *from, nameNode *to) {
    OpRename *or;
    or = (OpRename *)LSLMoreMem((void *)0, sizeof(OpRename));
    or->from = from;
    or->to = operator_new(to, operator_signature(from));
    or->signatureRenamed = FALSE;
    return set_insert(OpRename, r->operators, or);
}


extern renaming
*renaming_compose (renaming *r1, renaming *r2, symtable *s2) {
    renaming *r;
    if (r1 == (renaming *)0) return r2;
    r = renaming_new();
    if (renaming_nSorts(r2) != 0) {
	set_traverse(SortRename, r2->sorts, sr) {
	    renaming_addSort(r, sr->from, renaming_mapSort(r1, sr->to));
	} end_traverse;
    }
    set_traverse(SortRename, r1->sorts, sr) {
	if (symtable_sortExists(s2, sr->from)) {
	    /* The definition of element equality for "set(SortRename)" */
	    /* makes the following a no-op if "sr->from" is already in  */
	    /* the domain of "r".				        */
	    renaming_addSort(r, sr->from, sr->to);
	}
    } end_traverse;
    r->okToAddSorts = FALSE;
    symtable_renamingWork(r, r1, r2, s2);
    if (renaming_nOps(r2) != 0) {
	set_traverse(OpRename, r2->operators, or) {
	    if (symtable_isLiteral(s2, or->from)) {
		renaming_addOp(r, or->from, 
			       operator_name(renaming_mapOp(r1, or->to)));
	    }
	} end_traverse;
    }
    set_traverse(OpRename, r1->operators, or) {
	if (symtable_isLiteral(s2, or->from)) {
	    renaming_addOp(r, or->from, operator_name(or->to));
	}
    } end_traverse;
    return r;
}


extern void
renaming_composeWork (renaming *r, renaming *r1, renaming *r2, operator *op) {
    operator *op1 = renaming_mapOp(r1, renaming_mapOp(r2, op));
    if (op1 != op &&
	!nameNodeMatches(operator_name(op), operator_name(op1), FALSE))
	renaming_addOp(r, op, operator_name(op1));
}


extern bool
renaming_equal (renaming *r1, renaming *r2) {
    if (r1 == (renaming *)0) return renaming_isIdentity(r2);
    if (r2 == (renaming *)0) return renaming_isIdentity(r1);
    set_traverse(SortRename, r1->sorts, sr) {
	if (!sameSort(sr->to, renaming_mapSort(r2, sr->from))) return FALSE;
    } end_traverse;
    set_traverse(SortRename, r2->sorts, sr) {
	if (!sameSort(sr->to, renaming_mapSort(r1, sr->from))) return FALSE;
    } end_traverse;
    set_traverse(OpRename, r1->operators, or) {
	if (!operator_equal(or->to, renaming_mapOp(r2, or->from))) 
	    return FALSE; 
    } end_traverse;
    set_traverse(OpRename, r2->operators, or) {
	if (!operator_equal(or->to, renaming_mapOp(r1, or->from))) 
	    return FALSE; 
    } end_traverse;
    return TRUE;
}

					  
extern bool
renaming_isIdentity (renaming *r) {
    if (r == (renaming *)0) return TRUE;
    if (renaming_nSorts(r) == 0 && renaming_nOps(r) == 0) return TRUE;
    set_traverse(SortRename, r->sorts, sr) {
	if (!sameSort(sr->from, sr->to)) return FALSE;
    } end_traverse;
    set_traverse(OpRename, r->operators, or) {
	if (!nameNodeMatches(operator_name(or->from), operator_name(or->to), 
			     FALSE))
	    return FALSE; 
    } end_traverse;
    return TRUE;
}


extern equationNode
*renaming_mapEqn (renaming *r, equationNode *n) {
    termNode *left, *right;
    if (r == (renaming *)0) return n;
    left = renaming_mapTerm(r, equationNode_left(n));
    right = renaming_mapTerm(r, equationNode_right(n));
    if (left == equationNode_left(n) && right == equationNode_right(n))
	return n;
    return makeEquationNode(left, right);
}



extern genByNode 
*renaming_mapGenBy (renaming *r, genByNode *n) {
    OpList *ops;
    Sort   *s;
    bool    same;
    if (r == (renaming *)0) return n;
    s = renaming_mapSort(r, genByNode_sort(n));
    same = sameSort(s, genByNode_sort(n));
    ops = list_new(operator);
    list_traverse(operator, genByAttr_ops(n), op) {
	operator *op1;
        op1 = renaming_mapOp(r, op);
	same = same && (op == op1);
	list_addh(operator, ops, op1);
    } end_traverse;
    if (same) return n;
    return buildGenByNode(s, genByNode_freely(n), ops, node_loc(n));
}



/*
** Ensures:
**	(op \in domain(r->operators) & result = image(r->operators, op)) |
**	(~(op \in domain(r->operators)) & result = op)
** Returns "*op" itself, and not a copy, if it is not renamed by "*r".
*/

extern operator
*renaming_mapOp (renaming *r, operator *op) {
    if (r == (renaming *)0) return op;
    set_traverse(OpRename, r->operators, or) {
	if (operator_equal(op, or->from)) {
	    if (!or->signatureRenamed) mapTo(r, or);
	    if (operator_equal(op, or->to)) return op;
	    return or->to;
	}
    } end_traverse;
    if (renaming_nSorts(r) == 0) {
	return op;
    } else {
	signatureNode *sig = mapSig(r, operator_signature(op));
	if (sig == operator_signature(op)) return op;
	return operator_new(operator_name(op), sig);
    }
}


extern partByNode
*renaming_mapPartBy (renaming *r, partByNode *n) {
    OpList *ops;
    Sort   *s;
    bool    same;
    if (r == (renaming *)0) return n;
    s = renaming_mapSort(r, partByNode_sort(n));
    same = sameSort(s, partByNode_sort(n));
    ops = list_new(operator);
    list_traverse(operator, partByAttr_ops(n), op) {
	operator *op1;
        op1 = renaming_mapOp(r, op);
	same = same && (op == op1);
	list_addh(operator, ops, op1);
    } end_traverse;
    if (same) return n;
    return buildPartByNode(s, ops, node_loc(n));
}


extern Sort
*renaming_mapSort (renaming *r, Sort *s) {
    Sort *new;
    sortIdLeaf *id = sortNode_id(s);
    SortList *subsorts;
    bool matchedId = FALSE;
    bool matchedSubsort = FALSE;
    if (renaming_nSorts(r) == 0) return s;
    /* Handle renamings of entire sort or of sort identifier first */
    set_traverse(SortRename, r->sorts, sr) {
	if (sameSort(s, sr->from)) return sr->to;
	if (sortNode_subsorts(sr->from) == (SortList *)0
	    && sortNode_subsorts(sr->to) == (SortList *)0
	    && LSLSameLeaf(sortNode_id(s), sortNode_id(sr->from))) {
	  	matchedId = TRUE;
		id = sortNode_id(sr->to);
	      }
    } end_traverse;
    /* Now look for renamings of subsorts */
    if (sortNode_subsorts(s) != (sortNodeList *)0) {
      list_traverse(sortNode, sortNode_subsorts(s), s1) {
	new = renaming_mapSort(r, s1);
	if (s1 != new) {
	  matchedSubsort = TRUE;
	  break;
	}
      } end_traverse;
    }
    if (!matchedId && !matchedSubsort) return s;
    if (matchedSubsort) {
      subsorts = list_new(sortNode);
      list_traverse(sortNode, sortNode_subsorts(s), s1) {
	list_addh(sortNode, subsorts, renaming_mapSort(r, s1));
      } end_traverse;
    } else {
      subsorts = sortNode_subsorts(s);
    }
    new = makeSortNode(id, subsorts);
    renaming_addSort(r, s, new);
    return new;
}


extern termNode
*renaming_mapTerm (renaming *r, termNode *n){
  nameNode 	*root;
  termNodeList 	*args, *args1;
  operator	*op, *op1;
  bool		 same;
  if (n == (termNode *)0 || r == (renaming *)0) return n;
  switch (termAttr_kind(n)) {
  case VARTERM:
    {variable *v = set_choose(variable, termAttr_possibleVars(n));
     Sort *s = renaming_mapSort(r, variable_sort(v));
     if (s == variable_sort(v)) return n;
     n = makeTermNode(makeSimpleIdNameNode(variable_id(v)), 
		      (termNodeList *)0);
     attributeVarTerm(n, s);
     return n;
   }
  case OPTERM:
    op = set_choose(operator, termAttr_possibleOps(n));
    op1 = renaming_mapOp(r, op);
    same = op == op1;
    break;
  case QUANTIFIEDTERM:
    root = termNode_varOrOp(n);
    same = TRUE;
    break;
  default:
    error_programFailure("renaming_mapTerm");
  }
  args = termNode_args(n);
  if (args == (termNodeList *)0) {
    args1 = args;
  } else {
    args1 = list_new(termNode);
    list_traverse(termNode, args, a) {
      termNode *t;
      t = renaming_mapTerm(r, a);
      list_addh(termNode, args1, t);
      same = same && (t == a);
    } end_traverse;
  }
  if (!same) {
    switch (termAttr_kind(n)) {
    case OPTERM:
      n = makeTermNode(operator_name(op1), args1);
      attributeOpTerm(n, op1);
      break;
    case QUANTIFIEDTERM:
      n = makeTermNode(root, args1);
      attributeQuantifiedTerm(n);
      break;
    default:
      error_programFailure("renaming_mapTerm");
    }
  }
  return n;
}

extern int
renaming_nSorts (renaming *r) {
  if (r == (renaming *)0) return 0;
  return set_size(SortRename, r->sorts);
}


extern int
renaming_nOps (renaming *r) {
  if (r == (renaming *)0) return 0;
    return set_size(OpRename, r->operators);
}


extern void
renaming_unparse (renaming *r) {
    bool printed = FALSE;
    if (r == (renaming *)0) return;
    if (set_size(SortRename, r->sorts) == 0 && 
	set_size(OpRename, r->operators) == 0) return;
    print_char('(');
    set_traverse(SortRename, r->sorts, sr) {
	if (printed) print_str(", ");
	unparse_sort(sr->to);
	print_str(" for ");
	unparse_sort(sr->from);
	printed = TRUE;
    } end_traverse;
    set_traverse(OpRename, r->operators, or) {
	if (printed) print_str(", ");
	if (!or->signatureRenamed) mapTo(r, or);
	operator_unparse(or->to);
	print_str(" for ");
	operator_unparse(or->from);
	printed = TRUE;
    } end_traverse;
    print_char(')');
}


static bool
sameFromOp (OpRename *or1, OpRename *or2) {
    return operator_equal(or1->from, or2->from);
}


static bool
sameFromSort (SortRename *sr1, SortRename *sr2) {
    return LSLSameSort(sr1->from, sr2->from);
}


static signatureNode
*mapSig (renaming *r, signatureNode *sig) {
    Sort 	   *s1;
    bool  	    new = FALSE;
    list(sortNode) *dom = list_new(sortNode);
    list_traverse(sortNode, signatureNode_domain(sig), s) {
	s1 = renaming_mapSort(r, s);
	list_addh(sortNode, dom, s1);
	new = new || (s != s1);
    } end_traverse;
    s1 = renaming_mapSort(r, signatureNode_range(sig));
    if (new || s1 != signatureNode_range(sig)) {
	return makeSignature(dom, s1);
    } else {
	return sig;
    }
}


static void
mapTo (renaming *r, OpRename *or) {
    signatureNode *sig = mapSig(r, operator_signature(or->to));
    if (sig != operator_signature(or->to)) 
	or->to = operator_new(operator_name(or->to), sig);
    or->signatureRenamed = TRUE;
    r->okToAddSorts = FALSE;
}
