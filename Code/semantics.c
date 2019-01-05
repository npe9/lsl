/*
**
**	Copyright (c) MIT, 1991, 1992, 1993, 1994, 1998
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   semantics.c
**
**	PURPOSE:    Semantic attribution for traits
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 91.12.20 -- Original
**      {1} Garland at MIT   -- 92.01.29 -- Fix union semantics
**	{3} Garland at MIT   -- 92.02.19 -- Reorder #include "error.h"
**	{4} Garland at MIT   -- 93.01.07 -- Collect (some) lemmas
**	{5} Garland at MIT   -- 93.12.06 -- Remove unused declaration
**	{6} Garland at MIT   -- 94.05.24 -- Use "generated freely" for enums
**	{7} Garland at MIT   -- 98.02.24 -- Added new tuple equations
**	{8} Garland at MIT   -- 98.06.29 -- Use "generated freely" for unions
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
# include	"semantics.h"


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

extern void	  doSemantics (traitNodeList *allTraits);
static void	  traitSemantics (traitNode *t);
static void	  doTraitRefs (traitRefNodeList *refs, propSet *props, 
			       vartable *vars, ancestry *a, bool assertions,
			       bool assumptions, unsigned int *nRefs);
static void	  traitRefSemantics (traitRefNode *ref, propSet *props, 
				     vartable *vars, ancestry *a, 
				     bool assertions, bool assumptions, 
				     unsigned int *nRefs);
static void	  consequencesSemantics (traitNode *t, ancestry *a, 
					 unsigned int nRefs);
static void	  enumSemantics (propSet *props, ancestry *a, 
				 enumerationNode *e);
static void	  tupleSemantics (propSet *props, vartable *vars,
				  ancestry *a, tupleNode *tu);
static void	  unionSemantics (propSet *props, vartable *vars,
				  ancestry *a, unionNode *u);
static void	  propPartSemantics (propSet *props, vartable *vars,
				     ancestry *a, propPartNode *ppn);
static void	  genPartBysSemantics (propSet *props, ancestry *a, 
				       genPartByNodeList *gl);
static void	  equationsSemantics (propSet *props, ancestry *a, 
				      equationNodeList *eqs);
static void	  quantEqnsSemantics (propSet *props, ancestry *a, 
				      quantEqnsNodeList *qes, vartable *vars);
static void	  equationSemantics (propSet *props, ancestry *a, 
				     equationNode *e);
static termNode  *term_make0(operator *op);
static termNode  *term_make1(operator *op, termNode *t);
static termNode  *term_make2(operator *op, termNode *t1, termNode *t2);
static termNode	 *term_makeVar(varIdLeaf *v, Sort *s);


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


/*
** Computes the semantic attributes for the traits in "*allTraits".  Assumes
** that these traits have been checked successfully, i.e., that for any "*t" in
** "*allTraits", "traitAttr_axiomsCorrect(t) = CORRECT" and 
** "traitAttr_consequencesCorrect(t) = CORRECT".
*/

extern void
doSemantics (traitNodeList *allTraits) {
    list_traverse(traitNode, allTraits, t) {
	traitSemantics(t);
    } end_traverse;
}


/*
** Computes semantic attributes for trait "*t" and its subtraits.
*/

static void
traitSemantics (traitNode *t) {
    propSet 		*asserts = traitAttr_assertions(t);
    vartable		*assertVars;
    ancestry 		*a;
    unsigned int 	 nRefs = 0;
    unsigned int 	 nRefs1;
    unsigned int 	 nRefs2;
    if (node_exists(asserts)) return;	       /* semantics computed already */
    a = ancestry_create(t);
    traitAttr_assertions(t)   	= propSet_new();
    traitAttr_assumptions(t)  	= propSet_new();
    traitAttr_checkAssumes(t) 	= propSet_new();
    traitAttr_consequences(t) 	= propSet_new();
    traitAttr_lemmas(t) 	= propSet_new();
    traitAttr_assertVars(t)   	= vartable_new();
    traitAttr_assumeVars(t)	= vartable_new();
    traitAttr_checkVars(t)	= vartable_new();
    traitAttr_conseqVars(t)   	= vartable_new();
    traitAttr_lemmaVars(t)   	= vartable_new();
    /* Collect "traitAttr_assertions(t)" and "traitAttr_assumptions(t)" */
    asserts = traitAttr_assertions(t);
    assertVars = traitAttr_assertVars(t);
    propSet_insert(asserts, prop_makeTrait(a));     /* header prop for trait */
    if (node_exists(traitNode_exts(t))) {
	/* Collect assertions corresponding to shorthands in "*t" */
	list_traverse(extNode, traitNode_exts(t), en) {
	    switch (extNode_kind(en)) {
	    case LSLenumKIND:
		enumSemantics(asserts, a, extNode_LSLenum(en));
		break;
	    case LSLtupleKIND:
		tupleSemantics(asserts, assertVars, a, extNode_LSLtuple(en));
		break;
	    case LSLunionKIND:
		unionSemantics(asserts, assertVars, a, extNode_LSLunion(en));
		break;
	    case assumesKIND:
	    case includesKIND:
		break;
	    }
	} end_traverse;
    }
    if (node_exists(traitNode_asserts(t))) {
	/* Collect assertions asserted directly in "*t" */
	list_traverse(propPartNode, traitNode_asserts(t), ppn) {
	    propPartSemantics(asserts, assertVars, a, ppn);
	} end_traverse;
    }
    if (node_exists(traitNode_exts(t))) {
	/* Collect assertions inherited from traits included in "*t"       */
	/* Collect "traitAttr_assumptions(t)", "traitAttr_checkAssumes(t)" */
	/* Collect "traitAttr_lemmas(t)"				   */
	propSet  *assumes      = traitAttr_assumptions(t);
	vartable *assumesVars  = traitAttr_assumeVars(t);
	propSet  *checkAssumes = traitAttr_checkAssumes(t);
	vartable *checkVars    = traitAttr_checkVars(t);
	propSet  *lemmas       = traitAttr_lemmas(t);
	vartable *lemmaVars    = traitAttr_lemmaVars(t);
	list_traverse(extNode, traitNode_exts(t), en) {
	    switch (extNode_kind(en)) {
	    case LSLenumKIND:
	    case LSLtupleKIND:
	    case LSLunionKIND:
		break;
	    case assumesKIND:
		nRefs1 = nRefs;
		/* For "t1" assumed by "t", add "traitAttr_assertions(t1)",  */
		/* "traitAttr_assumptions(t1)" to "traitAttr_assumes(t)".    */
		doTraitRefs(assumesNode_traits(extNode_assumes(en)),
			    assumes, assumesVars, a, TRUE, TRUE, &nRefs);
		/* Add "traitAttr_consequences(t1) to "traitAttr_lemmas(t)"  */
		doTraitRefs(assumesNode_traits(extNode_assumes(en)),
			    lemmas, lemmaVars, a, FALSE, FALSE, &nRefs1);
		break;
	    case includesKIND:
		nRefs1 = nRefs;
		nRefs2 = nRefs;
		/* For "t1" included in "t", add "traitAttr_assertions(t1)"  */
		/* to "traitAttr_assertions(t)".			     */
		doTraitRefs(includesNode_traits(extNode_includes(en)),
			    asserts, assertVars, a, TRUE, FALSE, &nRefs);
		/* Add "traitAttr_assumptions(t1)" to 	   	             */
		/* "traitAttr_checkAssumes(t)" 				     */
		doTraitRefs(includesNode_traits(extNode_includes(en)),
			    checkAssumes, checkVars, a, FALSE, TRUE, &nRefs1);
		/* Add "traitAttr_consequences(t1) to "traitAttr_lemmas(t)"  */
		doTraitRefs(includesNode_traits(extNode_includes(en)),
			    lemmas, lemmaVars, a, FALSE, FALSE, &nRefs2);
		break;
	    }
	} end_traverse;
    }
    /* Collect consequences */
    consequencesSemantics(t, a, nRefs);
}


/*
** If "assertions" is TRUE, adds "traitAttr_assertions(t1)" for all "*t1" in
** "*refs" to "*props", merges "traitAttr_assertVars(t1)" into "*vars", and
** increments "*nRefs" by the number of subtraits in "*refs".
**
** If "assumptions" is TRUE, adds "traitAttr_assumptions(t1)" for all "*t1" in 
** "*refs" to "*props" and merges "traitAttr_assumeVars(t1)" into "*vars".
**
** Assumes: "ancestry_depth(a) = 0"
*/

static void 
doTraitRefs (traitRefNodeList *refs, propSet *props, vartable *vars,
	     ancestry *a, bool assertions, bool assumptions,
	     unsigned int *nRefs) {
    list_traverse(traitRefNode, refs, ref) {
	traitRefSemantics(ref, props, vars, a, assertions, assumptions, nRefs);
    } end_traverse;
}


/*
** If "assertions" is TRUE, adds "traitAttr_assertions(t1)" for all "*t1" in
** "*ref" to "*props", merges "traitAttr_assertVars(t1)" into "*vars", and
** increments "*nRefs" by the number of subtraits in "*ref".
**
** If "assumptions" is TRUE, adds "traitAttr_assumptions(t1)" for all "*t1" in 
** "*ref" to "*props" and merges "traitAttr_assumeVars(t1)" into "*vars".
**
** If "assertions" and "assumptions" are both FALSE, adds 
** "traitAttr_consequences(t1)" and "traitAttr_lemmas(t1)" for all "*t1" in 
** "*ref" to "*props" and merges "traitAttr_conseqVars(t1)" and
** "traitAttr_lemmaVars(t1)" into "*vars".
**
** Assumes: "ancestry_depth(a) = 0"
*/

static void
traitRefSemantics (traitRefNode *ref, propSet *props, vartable *vars,
		   ancestry *a, bool assertions, bool assumptions, 
		   unsigned int *nRefs) {
    traitNodeList 	*subtraits 	= traitRefAttr_traits(ref);
    renaming		*r		= traitRefAttr_rename(ref);
    int			 child		= nRefs[0];
    traitNode		*t0		= ancestry_bottomTrait(a);
    list_traverse(traitNode, subtraits, t) {
	traitSemantics(t);
	if (assertions) {
	    ++nRefs[0];
	    propSet_extend(props, t0, ++child, r, traitAttr_assertions(t));
	    vartable_merge(vars, traitAttr_assertVars(t), r);
	}
	if (assumptions) {
	    if (!assertions) ++child;
	    propSet_extend(props, t0, child, r, traitAttr_assumptions(t));
	    vartable_merge(vars, traitAttr_assumeVars(t), r);
	} else if (!assertions) {
	    propSet_extend(props, t0, ++child, r, traitAttr_consequences(t));
	    vartable_merge(vars, traitAttr_conseqVars(t), r);
	    propSet_extend(props, t0, ++child, r, traitAttr_lemmas(t));
	    vartable_merge(vars, traitAttr_lemmaVars(t), r);
	}
    } end_traverse;
}


/*
** Computes the consequences list of "*t".
*/

static void
consequencesSemantics (traitNode *t, ancestry *a, unsigned int nRefs) {
    consequencesNode 	*c     	= traitNode_implies(t);
    propSet		*props 	= traitAttr_consequences(t);
    vartable		*vars	= traitAttr_conseqVars(t);
    if (node_exists(c)) {
	traitRefNodeList *refs = consequencesNode_traits(c);
	genPartBysSemantics(props, a, consequencesNode_genPartBys(c));
	equationsSemantics(props, a, consequencesNode_eqns(c));
	quantEqnsSemantics(props, a, consequencesNode_quantEqns(c), vars);
	if (node_exists(refs)) {
	    doTraitRefs(refs, props, vars, a, TRUE, FALSE, &nRefs);
	}
    }
}


/* Adds the following assertions for "S enumeration of c_1, ..., c_n" to    */
/* "*props" with ancestry "*a".						    */
/*      assert S generated freely by c_1, ..., c_n 	    		    */
/*      assert succ(c_i) = c_(i+1) 	  	[1 <= i <= n]	    	    */

static void
enumSemantics (propSet *props, ancestry *a, enumerationNode *e) {
    Sort		*S 	= enumerationNode_sort(e);
    OpList		*elems	= enumerationAttr_elems(e);
    operator		*succ	= enumerationAttr_succ(e);
    int			 n	= list_size(operatorNode, elems);
    termNode	       **c;
    equationNode	*eqn;
    int			 i;
    propSet_insert(props, 
		   prop_makeGenBy(buildGenByNode(S, TRUE, elems, node_loc(e)), a));
    c = (termNode **)LSLMoreMem((void *)0, n*sizeof(termNode *));
    i = 0;
    list_traverse(operator, elems, op) {
	c[i++] = term_make0(op);
    } end_traverse;
    for (i = 0; i < n-1; i++) {
	eqn = makeEquationNode(term_make1(succ, c[i]), c[i+1]);
	propSet_insert(props, prop_makeEqn(eqn, a));
    }
}


/* Adds the following assertions for "S tuple of f1:S1, ..., fn:Sn" to	    */
/* "*props" with ancestry "*a".						    */
/*      assert S generated freely by [__, ..., __] 	    		    */
/*      assert with x1, y1: S1, ..., xn, yn: Sn, s: S 			    */
/*	       [x1, ..., xi, ..., xn].fi = xi;				    */
/*	       set_fi([x1, ..., xi, ..., xn], yi) = [x1, ..., yi, ..., xn]; */
/*             set_fi(s, xi).fi = xi;               			    */
/*             set_fi(s, xi).fj = s.fj;     % for j ~= i 	            */
/*             [s.f1, ..., s.fn] = s                                        */
/* Also adds the quantified variables used in these assertions to "*vars".  */
/* No longer adds the following assertion:				    */
/*      assert S partitioned by .f1, ..., .fn 			    	    */
/* To do: consider adding						    */
/*      assert with x1: S1, ..., xn: Sn, s1, s2: S 			    */
/*             s1 = s2 <=> s1.f1 = s2.f1 /\ ... /\ s1.fn = s2.fn            */
/* and removing (or making immune)					    */
/*      assert with x1, y1: S1, ..., xn, yn: Sn, s: S 			    */
/*             set_fi([x1, ..., xi, ..., xn], yi) = [x1, ..., yi, ..., xn]; */
/*             [s.f1, ..., s.fn] = s                                        */

static void
tupleSemantics (propSet *props, vartable *vars, ancestry *a, tupleNode *tu) {
    Sort		*S 	= tupleNode_sort(tu);
    OpList		*gets	= tupleAttr_gets(tu);
    OpList		*sets	= tupleAttr_sets(tu);
    int 		 n	= list_size(operatorNode, gets);
    operator		*make	= tupleAttr_make(tu);
    OpList		*makeL;
    termNodeList	*x, *xtL, *comps;
    termNode		*xt, *st;
    termNode	       **ax, **ay;
    operator	       **agets, **asets;
    int			 i, j;

    /* Create term "st" to represent s */
    st = term_makeVar(vartable_invent(vars, "s", 0, S, node_loc(tu)), S);

    /* Create arrays of get operators, set operators, fresh variables	    */
    /* Also create lists of variables, components of st			    */
    agets = (operator **)LSLMoreMem((void *)0, n*sizeof(operator *));
    asets = (operator **)LSLMoreMem((void *)0, n*sizeof(operator *));
    ax    = (termNode **)LSLMoreMem((void *)0, n*sizeof(termNode *));
    ay    = (termNode **)LSLMoreMem((void *)0, n*sizeof(termNode *));
    x     = list_new(termNode);
    comps = list_new(termNode);
    i     = 0;
    list_pointToFirst(operator, sets);
    list_traverse(operator, gets, geti) {
	varIdLeaf *v;
	Sort	  *s;
        agets[i] = geti;
	asets[i] = list_current(operator, sets);
	s = signatureNode_range(operator_signature(geti));
        v = vartable_invent(vars, "x", i+1, s, node_loc(tu));
	ax[i] = term_makeVar(v, s);
        v = vartable_invent(vars, "y", i+1, s, node_loc(tu));
	ay[i] = term_makeVar(v, s);
	list_addh(termNode, x, ax[i]);
	list_addh(termNode, comps, term_make1(agets[i], st));
	list_pointToNext(operator, sets);
	i++;
    } end_traverse;

    /* Formulate generated-by and partitioned-by */
    makeL = list_new(operator);
    list_addh(operator, makeL, make);
    propSet_insert
      (props, prop_makeGenBy(buildGenByNode(S, TRUE, makeL, node_loc(tu)), a));
    /*
    propSet_insert
      (props, prop_makePartBy(buildPartByNode(S, gets, node_loc(tu)), a));
      */

    /* Formulate  [s.f1, ..., s.fn] = s  */
    xt = makeTermNode(operator_name(make), comps);
    attributeOpTerm(xt, make);
    propSet_insert(props, prop_makeEqn(makeEquationNode(xt, st), a));    

    /* Create term "xt" to represent [x1, ..., xn]         */
    /* Also a list "xtL" containing "xt" (an optimization) */
    xt = makeTermNode(operator_name(make), x);
    attributeOpTerm(xt, make);
    xtL = list_new(termNode);
    list_addh(termNode, xtL, xt);

    /* Formulate equations */
    for (i = 0; i < n; i++) {
      termNode     *left, *right;
      termNodeList *xy;
      Sort	   *s;
      s = signatureNode_range(operator_signature(agets[i]));
      /* Formulate [x1, ..., xi, ..., xn].fi = xi		            */
      left = makeTermNode(operator_name(agets[i]), xtL);
      attributeOpTerm(left, agets[i]);
      propSet_insert(props, prop_makeEqn(makeEquationNode(left, ax[i]), a));
      /* Formulate 							    */
      /* set_fi([x1, ..., xi, ..., xn], yi) = [x1, ..., yi, ..., xn]	    */
      /* set_fi(s, xi).fj = s.fj;    	   for j ~= i  	                    */
      xy = list_new(termNode);
      for (j = 0; j < n; j++) {
	if (i == j) {
	  list_addh(termNode, xy, ay[i]);
	} else {
	  list_addh(termNode, xy, ax[j]);
	  left = term_make1(agets[j], term_make2(asets[i], st, ax[i]));
	  right = term_make1(agets[j], st);
	  propSet_insert(props, 
			 prop_makeEqn(makeEquationNode(left, right), a));
	}
      }
      left = term_make2(asets[i], xt, ay[i]);
      right = makeTermNode(operator_name(make), xy);
      attributeOpTerm(right, make);
      propSet_insert(props, prop_makeEqn(makeEquationNode(left, right), a));
      /* Formulate set_fi(s, xi).fi = xi               		    */
      left = term_make1(agets[i], term_make2(asets[i], st, ax[i]));
      propSet_insert(props, prop_makeEqn(makeEquationNode(left, ax[i]), a));
    }
    /* ENHANCE: generate prop of type TUPLE to optimize duplicate detection */
}


/* Adds the following assertions for "S union of f1:S1, ..., fn:Sn" to      */
/* "*props" with ancestry "*a".						    */
/*      S_tag enumeration of f1, ..., fn		    		    */
/*      assert S generated freely by f1, ..., fn			    */
/*      assert S partitioned by .f1, ..., .fn, tag 			    */
/*      assert \forall x1: S1, ..., xn: Sn 				    */
/*		fi(xi).fi = xi						    */
/*		tag(fi(xi)) = fi					    */
/* Also adds the quantified variables used in these assertions to "*vars".  */

static void
unionSemantics (propSet *props, vartable *vars, ancestry *a, unionNode *u) {
    Sort		*S 	= unionNode_sort(u);
    OpList		*toS	= unionAttr_toS(u);
    OpList		*fromS	= unionAttr_fromS(u);
    OpList		*parts	= list_copy(operator, fromS);
    operator		*tag	= unionAttr_tag(u);
    enumerationNode	*labels	= unionAttr_labels(u);
    OpList		*f	= enumerationAttr_elems(labels);
    int			 i;
    /* Formulate enumeration-of, generated-by, and partitioned-by */
    enumSemantics(props, a, labels);
    propSet_insert(props, 
		   prop_makeGenBy(buildGenByNode(S, TRUE, toS, node_loc(u)), a));
    list_addh(operator, parts, tag);
    propSet_insert(props, 
		   prop_makePartBy(buildPartByNode(S, parts, node_loc(u)), a));
    /* Formulate equations */
    i = 0;
    list_pointToFirst(operator, toS);
    list_pointToFirst(opertor, f);
    list_traverse(operator, fromS, fromSi) {
	Sort	     *Si;
	operator     *toSi, *fi;
	varIdLeaf    *v;
	termNode     *xi, *toSixi, *left, *right;
	toSi = list_current(operator, toS);
	Si = signatureNode_range(operator_signature(fromSi));
	fi = list_current(operator, f);
        v = vartable_invent(vars, "x", ++i, Si, node_loc(u));
	xi = term_makeVar(v, Si);
	toSixi = term_make1(toSi, xi);
	left = term_make1(fromSi, toSixi);
	propSet_insert(props, prop_makeEqn(makeEquationNode(left, xi), a));
	left = term_make1(tag, toSixi);
	right = term_make0(fi);
	propSet_insert(props, prop_makeEqn(makeEquationNode(left, right), a));
	list_pointToNext(operator, toS);
	list_pointToNext(opertor, f);
    } end_traverse;
    /* ENHANCE: generate prop of type UNION to optimize duplicate detection */
}


/* Adds the assertions in "*ppn" to "*props".  Also adds any quantified	    */
/* variables in "*ppn" to "*vars".					    */

static void
propPartSemantics (propSet *props, vartable *vars, ancestry *a, 
		   propPartNode *ppn) {
    genPartBysSemantics(props, a, propPartNode_genPartBys(ppn));
    equationsSemantics(props, a, propPartNode_equations(ppn));
    quantEqnsSemantics(props, a, propPartNode_quantEqns(ppn), vars);
}


/* Adds the assertions in "*gl" to "*props". */

static void
genPartBysSemantics (propSet *props, ancestry *a, genPartByNodeList *gl) {
    if (node_null(gl)) return;
    list_traverse(genPartByNode, gl, gp) {
	switch (genPartByNode_kind(gp)) {
	case genByKIND:
	    propSet_insert(props, prop_makeGenBy(genPartByNode_genBy(gp), a));
	    break;
	case partByKIND:
	    propSet_insert(props, prop_makePartBy(genPartByNode_partBy(gp), a));
	    break;
	}
    } end_traverse;
}


/* Adds the equations in "*eqs" to "*props". 		*/

static void
equationsSemantics (propSet *props, ancestry *a, equationNodeList *eqs) {
    if (node_null(eqs)) return;
    list_traverse(equationNode, eqs, en) {
	equationSemantics(props, a, en);
    } end_traverse;
}


/* Adds the equations in "*qes" to "*props".  Adds the quantified variables */
/* in "*qes" to "*vars".						    */

static void
quantEqnsSemantics (propSet *props, ancestry *a, quantEqnsNodeList *qes,
		    vartable *vars) {
    if (node_null(qes)) return;
    list_traverse(quantEqnsNode, qes, qe) {
	vartable_merge(vars, quantEqnsAttr_vars(qe), (renaming *)0);
	equationsSemantics(props, a, quantEqnsNode_equations(qe));
    } end_traverse;
}


/* Adds the equation "*e" to "*props". */

static void
equationSemantics (propSet *props, ancestry *a, equationNode *e) {
    propSet_insert(props, prop_makeEqn(e, a));
}


static termNode 
*term_make0 (operator *op) {
    termNode *n = makeTermNode(operator_name(op), (termNodeList *)0);
    attributeOpTerm(n, op);
    return n;
}


static termNode
*term_make1 (operator *op, termNode *t) {
    termNode *n;
    termNodeList *tl = list_new(termNode);
    list_addh(termNode, tl, t);
    n = makeTermNode(operator_name(op), tl);
    attributeOpTerm(n, op);
    return n;
}


static termNode
*term_make2 (operator *op, termNode *t1, termNode *t2) {
    termNode *n;
    termNodeList *tl = list_new(termNode);
    list_addh(termNode, tl, t1);
    list_addh(termNode, tl, t2);
    n = makeTermNode(operator_name(op), tl);
    attributeOpTerm(n, op);
    return n;
}


static termNode
*term_makeVar (varIdLeaf *v, Sort *s) {
    termNode *n = makeTermNode(makeSimpleIdNameNode(v), (termNodeList *)0);
    attributeVarTerm(n, s);
    return n;
}
