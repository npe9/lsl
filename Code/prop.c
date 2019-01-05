/*
**
**	Copyright (c) MIT, 1991, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   prop.c
**
**	PURPOSE:    Implementation of proposition abstraction
**	
**  MODIFICATION HISTORY:
**
**	{0} Garland  at MIT     -- 91.12.20 -- Original
**	{1} Garland  at MIT     -- 93.11.10 -- Add quantified terms
**	{2} Garland  at MIT     -- 93.12.06 -- Add #includes for clean compile
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/



/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include "general.h"
# include "osdfiles.h"
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
# include "lsl.h"
# include "unparse.h"

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

extern ancestry		*ancestry_create (traitNode *t);
extern bool		 ancestry_equal (ancestry *a1, ancestry *a2);
extern ancestry		*ancestry_extend (ancestry *a, traitNode *t, 
					  renaming *r, unsigned int child);
extern traitNode	*ancestry_bottomTrait (ancestry *a);
extern traitNode	*ancestry_topTrait (ancestry *a);
extern void		 ancestry_unparse (ancestry *a, bool renamings);

extern prop		*prop_makeTrait (ancestry *a);
extern prop		*prop_makeEqn (equationNode *e, ancestry *a);
extern prop		*prop_makeGenBy (genByNode *g, ancestry *a);
extern prop		*prop_makePartBy (partByNode *p, ancestry *a);
extern prop		*prop_rename (prop *p, ancestry *a);
extern bool		 prop_similar (prop *p1, prop *p2);
extern bool		 prop_unchanged (prop *p1, prop *p2);

static void		 ancestry_unparse_work (ancestry *a, int n, char *s,
						bool renamings);
static bool		 equation_similar (equationNode *n1, equationNode *n2);
static bool		 genBy_similar (genByNode *n1, genByNode *n2);
static bool		 partBy_similar (partByNode *n1, partByNode *n2);
static bool		 term_similar (termNode *n1, termNode *n2);
static unsigned int	 trait_key (ancestry *a);


/*
**
**  STATIC VARIABLES (none)
**
*/


/*
**
** FUNCTIONS
**
**
*/


/* Returns an ancestry for "*t" as a top-level trait. */

extern ancestry
*ancestry_create (traitNode *t) {
    ancestry *a = (ancestry *)LSLMoreMem((void *)0, sizeof(ancestry));
    a->depth = 0;
    a->lineage = (traitPtr *)LSLMoreMem((void *)0, sizeof(traitPtr));
    a->lineage[0] = t;
    a->renamings = (renamingPtr *)LSLMoreMem((void *)0, sizeof(renamingPtr));
    a->renamings[0] = (renamingPtr)0;
    a->path = (unsigned int *)LSLMoreMem((void *)0, sizeof(unsigned int));
    a->path[0] = 0;
    a->rename = (renaming *)0;
    return a;
}


/* Returns TRUE if "*a1" and "*a2" are the same. */

extern bool
ancestry_equal (ancestry *a1, ancestry *a2) {
    int i;
    if (a1 == a2) return TRUE;
    if (a1 == (ancestry *)0 || a2 == (ancestry *)0) return FALSE;
    if (a1->depth != a2->depth) return FALSE;
    for (i = 0; i <= a1->depth; i++) {
	if (a1->lineage[i] != a2->lineage[i]) return FALSE;
	if (a1->path[i] != a2->path[i]) return FALSE;
    }
    for (i = 0; i <= a1->depth; i++) {
	if (!renaming_equal(a1->renamings[i], a2->renamings[i])) return FALSE;
    }
    return TRUE;
}



/* Returns an extension of the ancestry "*a" by trait "*t" renamed by "*r", */
/* where "*t" is the "child"th subtrait of its parent.		 	    */

extern ancestry
*ancestry_extend (ancestry *a, traitNode *t, renaming *r, unsigned int child) {
    ancestry *b = (ancestry *)LSLMoreMem((void *)0, sizeof(ancestry));
    int n = a->depth + 1;
    int i;
    b->depth = n;
    b->lineage = 
	(traitPtr *)LSLMoreMem((void *)0, (n+1)*sizeof(traitPtr));
    b->renamings = 
	(renamingPtr *)LSLMoreMem((void *)0, (n+1)*sizeof(renamingPtr));
    b->path = 
	(unsigned int *)LSLMoreMem((void *)0, (n+1)*sizeof(unsigned int));
    for (i = 0; i < n; i++) {
	b->lineage[i]   = a->lineage[i];
	b->renamings[i] = a->renamings[i];
	b->path[i]      = a->path[i];
    }
    b->lineage[n]   = t;
    b->renamings[n] = r;
    b->path[n]      = child;
    b->rename 	    = renaming_compose(r, a->rename, 
				       traitAttr_symtab(b->lineage[0]));
    return b;
}


/* Returns the bottom-level trait in "*a". */

extern traitNode
*ancestry_bottomTrait (ancestry *a) {
    return a->lineage[0];
}



/* Returns the top-level trait in "*a". */

extern traitNode
*ancestry_topTrait (ancestry *a) {
    return a->lineage[a->depth];
}


/* Prints "*a". */

extern void
ancestry_unparse (ancestry *a, bool renamings) {
    int	d = a->depth;
    int	i;
    ancestry_unparse_work(a, 0, "", renamings);
    if (GetDebugFlag()) {
	print_adjustIndent(4);
	for (i = 1; i <= d; i++) ancestry_unparse_work(a, i, "in ", renamings);
	print_adjustIndent(-4);
    }
    if (renamings && d > 0 && !renaming_equal(a->rename, a->renamings[1])) {
	print_adjustIndent(2);
	print_str("with renaming ");
	renaming_unparse(a->rename);
	print_newline(1);
	print_adjustIndent(-2);
    }
}


static void
ancestry_unparse_work (ancestry *a, int d, char *label, bool renamings) {
    LSLToken   *name	= traitNode_name(a->lineage[d]);
    bool 	printed = FALSE;
    char 	buf[10];
    int 	i;
    print_str(label);
    if (d == a->depth) {
	print_str("main trait: ");
	unparse_token(name);
    } else {
	print_str("subtrait ");
	for (i = a->depth; i > d; i--) {
	    if (printed) print_char('.');
	    sprintf(buf, "%d", a->path[i]);
	    print_str(buf);
	    printed = TRUE;
	}
	print_str(": ");
	unparse_token(name);
	if (renamings) {
	    print_str(" ");
	    renaming_unparse(a->renamings[d+1]);
	}
    }
    print_newline(1);
}


/* Returns a proposition corresponding to the bottom-level trait in "*a".   */

extern prop
*prop_makeTrait (ancestry *a) {
    prop *p 		= (prop *)LSLMoreMem((void *)0, sizeof(prop));
    p->kind 		= TRAITPROP;
    p->pedigree 	= a;
    p->duplicate 	= (ancestry *)0;
    p->key		= trait_key(a);
    return p;
}


/* Returns a proposition containing equation "*e" with ancestry "*a". 	    */

extern prop
*prop_makeEqn (equationNode *e, ancestry *a) {
    prop *p      	= (prop *)LSLMoreMem((void *)0, sizeof(prop));
    p->kind      	= EQPROP;
    p->choice.eqn	= e;
    p->pedigree  	= a;
    p->duplicate 	= (ancestry *)0;
    p->key		= node_key(e);
    return p;
}


/* Returns a proposition containing the generated-by "*g" with ancestry	    */
/* "*a". 								    */

extern prop
*prop_makeGenBy (genByNode *g, ancestry *a) {
    prop *p      	= (prop *)LSLMoreMem((void *)0, sizeof(prop));
    p->kind      	= GENPROP;
    p->choice.genBy 	= g;
    p->pedigree  	= a;
    p->duplicate 	= (ancestry *)0;
    p->key		= node_key(g);
    return p;
}


/* Returns a proposition containing the partitioned-by "*pb" with ancestry  */
/* "*a". 								    */

extern prop
*prop_makePartBy (partByNode *pb, ancestry *a) {
    prop *p      	= (prop *)LSLMoreMem((void *)0, sizeof(prop));
    p->kind      	= PARTPROP;
    p->choice.partBy	= pb;
    p->pedigree  	= a;
    p->duplicate 	= (ancestry *)0;
    p->key		= node_key(pb);
    return p;
}


/* Returns the proposition with ancestry "*a" obtained by applying 	    */
/* "a->renamings[a->depth]" to "*p".					    */

extern prop
*prop_rename (prop *p, ancestry *a) {
    renaming *r = a->renamings[a->depth];
    prop *p1 = (prop *)LSLMoreMem((void *)0, sizeof(prop));
    p1->kind = p->kind;
    switch (p->kind) {
    case EQPROP:
	p1->choice.eqn = renaming_mapEqn(r, p->choice.eqn);
	p1->key = node_key(p1->choice.eqn);
	break;
    case GENPROP:
	p1->choice.genBy = renaming_mapGenBy(r, p->choice.genBy);
	p1->key = node_key(p1->choice.genBy);
	break;
    case PARTPROP:
	p1->choice.partBy = renaming_mapPartBy(r, p->choice.partBy);
	p1->key = node_key(p1->choice.partBy);
	break;
    case TRAITPROP:
	p1->key = trait_key(a);
	break;
    }
    p1->pedigree = a;
    p1->duplicate = p->duplicate;
    return p1;
}


/* Returns TRUE if "*p1" and "*p2" represent the same assertion or set of    */
/* assertions.  When "*p1" and "*p2" are propositions for traits, the sets   */
/* of assertions they represent are the same if they are the same renaming   */
/* of the same bottom-level trait.			  		     */

/* ENHANCE: check that renaming is the same on the operators in trait; it    */
/* does not matter what the renaming does to other operators                 */

extern bool
prop_similar (prop *p1, prop *p2) {
    traitNode *t1, *t2;
    if (p1->kind != p2->kind) return FALSE;
    if (prop_unchanged(p1, p2)) return TRUE;
    switch (p1->kind) {
    case EQPROP:	
	return equation_similar(p1->choice.eqn, p2->choice.eqn);
    case GENPROP:	
	return genBy_similar(p1->choice.genBy, p2->choice.genBy);
    case PARTPROP:	
	return partBy_similar(p1->choice.partBy, p2->choice.partBy);
    case TRAITPROP:	
	if (p2->duplicate != (ancestry *)0) return FALSE;
	t1 = ancestry_bottomTrait(p1->pedigree);
	t2 = ancestry_bottomTrait(p2->pedigree);
	return t1 == t2 && 
	    renaming_equal(p1->pedigree->rename, p2->pedigree->rename);
    }
}


/* Returns TRUE if "*p1" and "*p2" represent assertions that are the same */
/* objects.                                                               */

extern bool
prop_unchanged (prop *p1, prop *p2) {
    if (p1->kind != p2->kind) return FALSE;
    switch (p1->kind) {
    case EQPROP:	
	return p1->choice.eqn == p2->choice.eqn;
    case GENPROP:	
	return p1->choice.genBy == p2->choice.genBy;
    case PARTPROP:	
	return p1->choice.partBy == p2->choice.partBy;
    case TRAITPROP:	
	return p1 == p2;
    }
}


static bool
equation_similar (equationNode *n1, equationNode *n2) {
    if (node_key(n1) != node_key(n2)) return FALSE;
    return (term_similar(equationNode_left(n1), equationNode_left(n2)) &&
	    term_similar(equationNode_right(n1), equationNode_right(n2)));
}


static bool
genBy_similar (genByNode *n1, genByNode *n2) {
    if (node_key(n1) != node_key(n2)) return FALSE;
    /* IMPLEMENT: better check for same elements in any order in list */
    return list_compare(operator, genByAttr_ops(n1), genByAttr_ops(n2),
			operator_equal);
}


static bool
partBy_similar (partByNode *n1, partByNode *n2) {
    if (node_key(n1) != node_key(n2)) return FALSE;
    /* IMPLEMENT: better check for same elements in any order in list */
    return list_compare(operator, partByAttr_ops(n1), partByAttr_ops(n2),
			operator_equal);
}


static bool
term_similar (termNode *n1, termNode *n2) {
    if (n1 == n2) return TRUE;
    if (n1 == (termNode *)0 || n2 == (termNode *)0) return FALSE;
    if (node_key(n1) != node_key(n2)) return FALSE;
    if (termAttr_kind(n1) != termAttr_kind(n2)) return FALSE;
    if (!sameSort(termAttr_sort(n1), termAttr_sort(n2))) return FALSE;
    switch (termAttr_kind(n1)) {
    case OPTERM:
	if (!operator_equal(set_choose(operator, termAttr_possibleOps(n1)),
			    set_choose(operator, termAttr_possibleOps(n2))))
	    return FALSE;
	return list_compare(termNode, termNode_args(n1), termNode_args(n2),
			    term_similar); 
    case VARTERM:
	return variable_equal(set_choose(variable, termAttr_possibleVars(n1)),
			      set_choose(variable, termAttr_possibleVars(n2)));
    case QUANTIFIEDTERM:
	return 
	  (nameNodeMatches(termNode_varOrOp(n1), termNode_varOrOp(n2), FALSE)
	   && list_compare(termNode, termNode_args(n1), termNode_args(n2),
			   term_similar)); 
    default:
	error_programFailure("term_similar");
    }
    return TRUE;
}


static unsigned int
trait_key (ancestry *a) {
    /* make this better by taking a->rename into account */
    return node_key(ancestry_bottomTrait(a));
}





