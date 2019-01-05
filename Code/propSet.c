/*
**
**	Copyright (c) MIT, 1991, 1992, 1993, 1994
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   propSet.c
**
**	PURPOSE:    Manages sets of propositions
**
**	     A set of propositions is represented by a list and a hash table.
**	The list contains the elements of the set in the order of insertion.
**	It may also contain elements explicitly marked as duplicates of other
**	elements occurring earlier in the list.  The hash table contains 
**	pointers to all elements in the list that are not marked as duplicates;
**	it is used to determine whether an element being inserted in the set is
**	a new element or a duplicate.
**
**          The pedigrees of elements in the list change only when the propKind
**	of the element is TRAITPROP, i.e., all props between two of type 
**	TRAITPROP have the same pedigree as the first of type TRAITPROP.
**
**  MODIFICATION HISTORY:
**
**	{0} Garland  at MIT     -- 91.12.26 -- Original
**	{1} Garland  at MIT	-- 92.01.12 -- Add LP translation to print
**	{2} Garland  at MIT	-- 92.01.28 -- Treat AC ops, do asserts with ..
**	{3} Garland  at MIT	-- 92.02.12 -- Fix AC op treatment, add free
**	{4} Garland  at MIT	-- 93.09.23 -- Change for LP3.1
**	{5} Garland  at MIT	-- 93.12.06 -- Push settings for DecimalLiterals
**	{6} Garland  at MIT	-- 94.05.23 -- Print QED only after "prove"
**	{n} Who	     at Where   -- yy.mm.dd -- what
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include "general.h"
# include "osdfiles.h"
# include STDLIB
# include "string2sym.h"
# include "token.h"
# include "error.h"
# include "parse.h"
# include "list.h"
# include "set.h"
# include "abstract.h"
# include "operator.h"
# include "renaming.h"
# include "symtable.h"
# include "vartable.h"
# include "prop.h"
# include "propSet.h"
# include "checking.h"
# include "print.h"
# include "unparse.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define HTSIZE		(HASHMASK + 1)
# define PROPSET_ALLOC	100


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

extern propSet	*propSet_new();
extern void	 propSet_free();
extern bool	 propSet_insert();
extern void	 propSet_extend();
extern void	 propSet_extendCvt();
extern void	 propSet_print();

static void 	 printDots (bool *printed, bool *reset);
static void	 printOpTheory(char *name, propSet *ps, int i);
static void	 printQED (void);

/*
**
**  STATIC VARIABLES
**
*/


/*
**
** FUNCTION DEFINITIONS
**
*/

propSet
*propSet_new (void) {
    int i;
    propSet *ps = (propSet *)LSLMoreMem((void *)0, sizeof(propSet));
    ps->nontrivial = FALSE;
    ps->size = 0;
    ps->alloc = 0;
    ps->list = (propList *)0;
    ps->hashTable = 
	(propBucket *)LSLMoreMem((void *)0, HTSIZE*sizeof(propBucket));
    for (i = 0; i < HTSIZE; i++) {
	ps->hashTable[i].elem = (prop *)0;
    }
    return ps;
}


extern void
propSet_free (propSet *ps) {
    int i;
    if (ps == (propSet *)0) return;
    if (ps->list != (propList *)0) free(ps->list);
    for (i = 0; i < HTSIZE; i++) {
	propBucket *b, *next;
	for (b = ps->hashTable[i].next; b != (propBucket *)0; b = next) {
	    next = b->next;
	    free(b);
	}
    }
    free(ps->hashTable);
    free(ps);
}



/* Inserts "*pr" in "*ps", whether or not it is already there.  If "*pr" is */
/* a duplicate of an element already in "*ps", marks the newly inserted     */
/*  as a duplicate and returns TRUE.  Otherwise returns FALSE.              */

bool
propSet_insert (propSet *ps, prop *p) {
    bool addToHashTable = TRUE;
    bool duplicate      = FALSE;
    propBucket *b = &ps->hashTable[prop_key(p)];
    while (b->elem != (prop *)0) {
	if (prop_similar(b->elem, p)) {
	    /* Mark "p" as a duplicate, add to list, but not to hash table */
	    p->duplicate = b->elem->pedigree;
	    duplicate = TRUE;
	    addToHashTable = FALSE;
	    break;
	}
	if (b->next == (propBucket *)0) {
	    /* Create entry for "p" in bucket */
	    b->next = (propBucket *)LSLMoreMem((void *)0, sizeof(propBucket));
	    b = b->next;
	    break;
	} else {
	    /* Examine next entry in bucket */
	    b = b->next;
	}
    }
    if (addToHashTable) {
	b->elem = p;
	b->next = (propBucket *)0;
    }
    if (ps->size >= ps->alloc) {
	ps->alloc = ps->alloc + PROPSET_ALLOC;
	ps->list = 
	    (propList *)LSLMoreMem(ps->list, (ps->alloc)*sizeof(propList));
    }
    ps->list[ps->size++] = p;
    if (p->kind != TRAITPROP) ps->nontrivial = TRUE;
    return duplicate;
}


/* Appends to the props "ps1" from trait "*t" the props in "ps2" from its   */
/* offspring number "child" renamed by "r" and with their ancestries        */
/* extended by "t", "r", and "child", but with the following exceptions.    */
/* Marks any renamed prop from "ps2" as a duplicate if there is already a   */
/* similar prop in "ps1".  If a trait prop in "ps2" that is not already     */
/* marked as a duplicate is similar to a trait prop in "ps1", marks that    */
/* prop as a duplicate and appends it to "ps1", but does not append any of  */
/* the props from the duplicate trait.                                      */

void 
propSet_extend (propSet *ps1, traitNode *t, int child, 
		renaming *r, propSet *ps2) {
    ancestry *current = (ancestry *)0;	     /* current ancestry from "ps2" */
    ancestry *extended;			     /* extension of "current"      */
    bool      skip = FALSE;
    prop     *pr;
    int	      i;
    for (i = 0; i < ps2->size; i++) {
	pr = ps2->list[i];
	if (skip && (ancestry_length(pr->pedigree) > ancestry_length(current)
		     || pr->pedigree == current)) continue;
	if (pr->pedigree != current) {
	    current = pr->pedigree;
	    extended = ancestry_extend(current, t, r, child);
	    skip = FALSE;
	}
	if (propSet_insert(ps1, prop_rename(pr, extended))) {
	    /* prop was a duplicate */
	    if (pr->kind == TRAITPROP) {
		if (i == 0) return;	      /* all of "ps2" are redundant */
		skip = TRUE;
	    }
	}
    } 
}


/* Renames the props in "*ps2" from trait "*t" by "*r" and appends those    */
/* that change to "*ps1".  Marks any renamed prop from "ps2" as a duplicate */
/* if there is already a similar prop in "ps1".  If a trait prop in "ps2"   */
/* that is not already  marked as a duplicate is similar to a trait prop in */
/* "ps1", marks that prop as a duplicate and appends it to "ps1", but does  */
/* not append any of the props from the duplicate trait.                    */

void 
propSet_extendCvt (propSet *ps1, traitNode *t, renaming *r, propSet *ps2) {
    ancestry *current = (ancestry *)0;	     /* current ancestry from "ps2" */
    ancestry *extended;			     /* extension of "current"      */
    bool      skip = FALSE;
    prop     *pr, *pr1;
    int	      i;
    for (i = 0; i < ps2->size; i++) {
	pr = ps2->list[i];
	if (skip && pr->pedigree == current) continue;
	if (pr->pedigree != current) {
	    current = pr->pedigree;
	    extended = ancestry_extend(current, t, r, 0);
	    skip = FALSE;
	}
	pr1 = prop_rename(pr, extended);
	if (prop_unchanged(pr, pr1)) continue;
	if (propSet_insert(ps1, pr1)) {
	    /* prop was a duplicate */
	    if (pr->kind == TRAITPROP) {
		if (i == 1) return;	      /* all of "ps2" are redundant */
		skip = TRUE;
	    }
	}
    } 
}


void
propSet_print (propSet *ps, bool asserts, renaming *r) {
    ancestry	*aPrev        = (ancestry *)0;
    propKind	 lastKind     = TRAITPROP;
    bool	 nameNeeded   = TRUE;
    bool	 printed      = FALSE;
    bool	 decimalTrait = FALSE;
    bool	 opTheory     = FALSE;
    char	*name;
    int		 i;
    prop	*p;
    ancestry	*a;
    if (!ps->nontrivial) return;
    for (i = 0; i < ps->size; i++) {
	p = ps->list[i];
	a = prop_pedigree(p);
	if (!ancestry_equal(a, aPrev)) {
	    printDots(&printed, &decimalTrait);
	    print_newline(1);
	    print_setPrefix("% ");
	    ancestry_unparse(a, TRUE);
	    print_setPrefix("");
	    print_newline(1);
	    nameNeeded = TRUE;
	    aPrev = a;
	}
	if (prop_isDuplicate(p)) {
	    print_setPrefix("% ");
	    switch (prop_kind(p)) {
	    case TRAITPROP:
		print_str("duplicate of ");
		break;
	    case GENPROP:
		print_str("duplicate of generated-by in ");
		break;
	    case PARTPROP:
		print_str("duplicate of partitioned-by in ");
		break;
	    case EQPROP:
		print_str("duplicate of equation in ");
		break;
	    default:
		error_programFailure("propSet_print");
	    }
	    ancestry_unparse(prop_duplicate(p), FALSE);
	    print_setPrefix("");
	} else {
	    opTheory = FALSE;
	    if (nameNeeded) {
		name = token_text(*traitNode_name(ancestry_bottomTrait(a)));
		if (prop_kind(p) != TRAITPROP || strcmp(name, "AC") == 0 ||
		    strcmp(name, "Commutative") == 0) {
		    printDots(&printed, &decimalTrait);
		    print_str("set name ");
		    print_str(name);
		    if (asserts) {
		      decimalTrait = strcmp(name, "DecimalLiterals") == 0;
		      if (decimalTrait) {
			 print_newline(2);
			 print_str("push-settings");
			 print_newline(1);
			 print_str("set activity off");
			 print_newline(1);
			 print_str("set immunity on");
		       }
		    } else {
		      print_str("Theorem");
		    }
		    print_newline(2);
		    nameNeeded = FALSE;
		    opTheory = (asserts && prop_kind(p) == TRAITPROP);
		  }
	      }
	    if (prop_kind(p) != TRAITPROP || opTheory) {
	       if (printed) {
		  print_char(';');
		} else {
		  if (asserts)
		    print_str("assert");
		  else
		    print_str("prove");
		  print_newline(1);
		  print_setIndent(2);
		  printed = TRUE;
		}
	     }
	    switch (prop_kind(p)) {
	    case EQPROP:
		unparse_equation(renaming_mapEqn(r, prop_equation(p)));
		print_newline(1);
		break;
	    case GENPROP:
		unparse_genBy(renaming_mapGenBy(r, prop_genBy(p)));
		print_newline(1);
		break;
	    case PARTPROP:
		unparse_partBy(renaming_mapPartBy(r, prop_partBy(p)));
		print_newline(1);
		break;
	    case TRAITPROP:
		if (opTheory) {
		  printOpTheory(name, ps, i);
		  print_newline(1);
		}
		break;
	    }
	    if (prop_kind(p) != TRAITPROP || opTheory)
	      print_setIndent(2);
	    if (!asserts) {
	      printDots(&printed, &decimalTrait);
	      if (printed) printQED();
	    }
	    lastKind = prop_kind(p);
	}
    }
    printDots(&printed, &decimalTrait);
    print_newline(1);
}


static void
printDots (bool *printed, bool *reset) {
    if (*printed) {
	print_str("..");
	print_newline(1);
	print_setIndent(0);
	*printed = FALSE;
	if (*reset) {
	    print_newline(1);
	    print_str("pop-settings");
	    print_newline(1);
	    *reset = FALSE;
	}
    }
}


static void
printOpTheory(char *name, propSet *ps, int i) {
    ancestry     *a = prop_pedigree(ps->list[i]);
    propSet      *s = traitAttr_assertions(ancestry_bottomTrait(a));
    prop         *p = (prop *)0;
    operator     *op;
    nameNode     *n;
    termNode	 *t;
    for (i = 0; i < s->size; i++) {
	p = s->list[i];
	if (prop_kind(p) == EQPROP) break;
    }
    if (p == (prop *)0) return;
    t = equationNode_left(prop_equation(p));
    op = set_choose(operator, termAttr_possibleOps(t));
    n = operator_name(op);
    if (nameNode_kind(n) == ppiOpKIND
	&& anyOpNode_kind(ppiOpNode_op(nameNode_ppiOp(n))) == eqOpKIND) {
      list_pointToFirst(termNode, termNode_args(t));
      t = list_current(termNode, termNode_args(t));
      op = set_choose(operator, termAttr_possibleOps(t));
    }
    if (op == (operator *)0) return;
    if (strcmp(name, "AC") == 0) {
      print_str("ac ");
    } else {
      print_str("commutative ");
    }
    operator_unparse(renaming_mapOp(a->rename, op));
}


static void
printQED (void) {
    print_setIndent(2);
    print_str("qed");
    print_newline(2);
    print_setIndent(0);
}


