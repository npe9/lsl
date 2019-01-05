/*
**
**	Copyright (c) MIT, 1991, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   lsl2lp.c
**
**	PURPOSE:    LP output routines
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 91.12.20 -- Original
**	{1} Garland at MIT   -- 92.01.12 -- Recode LP translation for mixfix
**      {2} Garland at MIT   -- 92.01.23 -- Add support for -tr
**      {3} Garland at MIT   -- 92.01.28 -- Add "set auto-ord on/off"
**					    Add "set script"
**      {4} Garland at MIT   -- 92.02.19 -- Use error_reportBadToken
**	{5} Garland at MIT   -- 92.08.04 -- Fix "set_log" typo
**	{6} Garland at MIT   -- 92.10.05 -- Change id[1] to id[0] in convert_op
**	{7} Garland at MIT   -- 93.01.07 -- Add lemmas to file of axioms
**					    Output for command line files only
**	{8} Garland at MIT   -- 93.09.23 -- Change for LP3.1
**	{9} Garland at MIT   -- 93.11.09 -- Add quantified formulas
**     {10} Garland at MIT   -- 93.11.11 -- Accumulate declarations
**     {11} Garland at MIT   -- 01.03.19 -- Fix loop in outputConverts
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include	"general.h"
# include	"osdfiles.h"
# include	"string2sym.h"
# include	"token.h"
# include 	"error.h"
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
# include	"unparse.h"
# include       "source.h"
# include	"scan.h"
# include	"print.h"
# include	"lsl2lp.h"
# include       "lsl.h"

/*
**
**  MACRO DEFINITIONS (none)
**
*/

# define SIMPLEIDSUFFIX	"'"
# define SIMPLEOPSUFFIX "*"


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

extern void	 outputLP (traitNodeList *allTraits,			   
			   char **trFiles, int nTrFiles);
static void	 outputTrait (traitNode *t, renaming *r);
static void      parse_renaming (source *s, renaming *r);
static void      outputAxioms (traitNode *t, char *name, renaming *r);
static void	 outputTheorems (traitNode *t, char *name, renaming *r);
static void	 outputChecks (traitNode *t, char *name, renaming *r);
static void	 outputConverts (traitNode *t, char *name, conversionNode *cvt,
				 renaming *r);
static void	 openFile (char *name, char *ext);
static void	 deleteFile (char *name, char *ext);
static operator *convert_op (operator *from, symtable *tsyms, symtable *csyms);


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
** Generates LP files for the traits in "*allTraits" that were listed on the
** command line.  Assumes that semantic attributes have been computed.  Applies
** operator renamings found in "trFiles".
*/

extern void
outputLP (traitNodeList *allTraits, char **trFiles, int nTrFiles) {
    renaming *r = renaming_new();
    int i;
    for (i = 0; i < nTrFiles; i++) {
      source *s;
      s = source_create(trFiles[i], "", FALSE, FALSE);
      if (!source_getPath(SPECPATH, s, token_null())) {
	puts(string_paste("Warning: can't find -tr file `",
			  string_paste(source_fileName(s), "'")));
	continue;
      }
      if (!source_open(s)) {
	puts(string_paste("Warning: can't open file `",
			  string_paste(source_fileName(s), "'")));
	continue;
      }
      parse_renaming(s, r);
      source_close(s);
    }
    unparse_setForLP(TRUE);
    list_traverse(traitNode, allTraits, t) {
	if (traitNode_commandLineArg(t)) outputTrait(t, r);
    } end_traverse;
}

/*
** Adds the operator renamings in "*s" to "*r".
*/

static void
parse_renaming (source *s, renaming *r) {
  replaceNode   *replace;
  parameterNode	*actual;
  parameterNode	*formal;
  operator      *op;
  operatorNode	*n;
  nameNode	*newName;
  LSLToken 	*tok;
  int rc = error_setQuit();
  if (rc == 0) {
    LSLScanReset(s);
    LSLParseReset();
    for (;;) {
      LSLAbstractReset();
      if (!LSLParseReplace()) break;
      replace = LSLAbstractParsedReplace();
      actual  = replaceNode_actual(replace);
      formal  = replaceNode_formal(replace);
      if (parameterNode_kind(formal) != operatorKIND) {
	tok = &node_loc(formal);
	error_reportBadToken(tok, "missing signature in -tr renaming");
	continue;
      }
      n = parameterNode_operator(formal);
      op = operator_new(operatorNode_name(n), operatorNode_signature(n));
      switch (parameterNode_kind(actual)) {
      case operatorKIND:
	  newName = operatorNode_name(parameterNode_operator(actual));
	  break;
      case sortKIND:
	  tok = &node_loc(actual);
	  error_reportBadToken(tok, "bad name for operator in -tr renaming");
	  break;
      case simpleIdKIND:
	  newName = makeSimpleIdNameNode(parameterNode_simpleId(actual));
	  break;
      }
      newName = adjustMarkers(newName, op);
      if (!renaming_addOp(r, op, newName)) {
	tok = &node_loc(formal);
	error_reportBadToken(tok, "old operator already used in -tr renaming");
      }
      /* FIX: Check that new operator is not used */
    }
  }
}


/*
** Generates LP files for trait "*t".
*/

static void
outputTrait (traitNode *t, renaming *r) {
    char *name = LSLSymbol2String(traitNode_name(t)->text);
    if (strcmp(name, "DecimalLiterals") == 0) return;
    unparse_resetSymbols();
    unparse_resetVariables();
    outputAxioms(t, name, r);
    outputTheorems(t, name, r);
    outputChecks(t, name, r);
}

static void
outputAxioms (traitNode *t, char *name, renaming *r) {
    symtable *st = traitAttr_symtab(t);
    bool asserts = propSet_nontrivial(traitAttr_assertions(t));
    bool assumes = propSet_nontrivial(traitAttr_assumptions(t));
    bool lemmas  = propSet_nontrivial(traitAttr_lemmas(t));
    openFile(name, "_Axioms.lp");
    print_str("%%% Axioms for trait ");
    print_str(name);
    print_newline(2);
    print_str("%% Operator declarations");
    print_newline(2);
    symtable_print2LP(st, r);
    if (asserts || assumes) {
	print_str("set automatic-ordering off");
	print_newline(2);
    }
    if (asserts) {
	print_str("%% Assertions");
	print_newline(2);
	vartable_print2LP(traitAttr_assertVars(t), st);
	propSet_print(traitAttr_assertions(t), TRUE, r);
    }
    if (assumes) {
	print_str("%% Assumptions");
	print_newline(2);
	vartable_print2LP(traitAttr_assumeVars(t), st);
	propSet_print(traitAttr_assumptions(t), TRUE, r);
    }
    if (lemmas) {
	print_str("%% Consequences of subsidiary traits");
	print_newline(2);
	vartable_print2LP(traitAttr_lemmaVars(t), st);
	propSet_print(traitAttr_lemmas(t), TRUE, r);
    }
    if (asserts || assumes || lemmas) {
	print_str("set automatic-ordering on");
	print_newline(2);
    }
    print_closeFile();
}


static void
outputTheorems (traitNode *t, char *name, renaming *r) {
    if (!propSet_nontrivial(traitAttr_consequences(t))) {
	deleteFile(name, "_Theorems.lp");
	return;
    }
    openFile(name, "_Theorems.lp");
    print_str("%%% Theorems from trait ");
    print_str(name);
    print_newline(2);
    print_str("execute ");
    print_str(name);
    print_str("_Axioms");
    print_newline(2);
    vartable_print2LP(traitAttr_conseqVars(t), traitAttr_symtab(t));
    print_str("%% Theorems");
    print_newline(2);
    propSet_print(traitAttr_consequences(t), TRUE, r);
    /* ENHANCE: follow implication chains */
    print_closeFile();
}

static void
outputChecks (traitNode *t, char *name, renaming *r) {
    symtable	     *st       = traitAttr_symtab(t);
    consequencesNode *c        = traitNode_implies(t);
    bool 	      converts = (node_exists(c) && 
				  node_exists(consequencesNode_converts(c)));
    if (!propSet_nontrivial(traitAttr_consequences(t)) &&
	!propSet_nontrivial(traitAttr_checkAssumes(t)) &&
	!converts) {
	deleteFile(name, "_Checks.lp");
	return;
    }
    openFile(name, "_Checks.lp");
    print_str("set script ");
    print_str(name);
    print_newline(1);
    print_str("set log ");
    print_str(name);
    print_newline(2);
    print_str("%%% Proof Obligations for trait ");
    print_str(name);
    print_newline(2);
    print_str("execute ");
    print_str(name);
    print_str("_Axioms");
    print_newline(2);
    if (propSet_nontrivial(traitAttr_checkAssumes(t))) {
	print_str("%% Inherited assumptions");
	print_newline(2);
	vartable_print2LP(traitAttr_checkVars(t), st);
	propSet_print(traitAttr_checkAssumes(t), FALSE, r);
    }
    if (propSet_nontrivial(traitAttr_consequences(t))) {
	print_str("%% Implications");
	print_newline(2);
	vartable_print2LP(traitAttr_conseqVars(t), st);
	propSet_print(traitAttr_consequences(t), FALSE, r);
    }
    if (converts) {
	print_str("%% Conversions");
	print_newline(2);
	print_str("freeze ");
	print_str(name);
	print_newline(2);
	list_traverse(conversionNode, consequencesNode_converts(c), cvt) {
	    outputConverts(t, name, cvt, r);
	} end_traverse;
    }
    print_closeFile();
}


static void
outputConverts (traitNode *t, char *name, conversionNode *cvt, renaming *r0) {
    operatorNodeList 	*ops 	= conversionNode_converts(cvt);
    exemptionNode	*exempt = conversionNode_exempts(cvt);
    symtable		*tsyms 	= traitAttr_symtab(t);
    symtable		*csyms	= symtable_blank();
    vartable		*vars	= vartable_new();
    renaming 		*rc 	= renaming_new();
    propSet		*axioms	= propSet_new();
    termNodeList	*noArgs = (termNodeList *)0;
    equationNodeList	*checks = list_new(equationNode);
    /* Build renaming for converted operators, formulate proof obligations */
    list_traverse(operatorNode, ops, op) {
	operator	*original;
	operator	*duplicate;
	list(Sort)	*domain;
	termNodeList	*args;
	equationNode	*e;
	int		 n;
	original = operatorAttr_op(op);
	duplicate = convert_op(renaming_mapOp(r0, original), tsyms, csyms);
	renaming_addOp(rc, original, operator_name(duplicate));
	domain = signatureNode_domain(operator_signature(original));
	args = list_new(termNode);
	n = 0;
	list_traverse(Sort, domain, s) {
	    varIdLeaf *v;
	    termNode  *t;
	    v = vartable_invent(vars, "x", ++n, s, node_loc(cvt));
	    t = makeTermNode(makeSimpleIdNameNode(v), noArgs);
	    attributeVarTerm(t, s);
	    list_addh(termNode, args, t);
	} end_traverse;
	e = makeEquationNode(makeTermNode(operator_name(original), args),
			     makeTermNode(operator_name(duplicate), args));
	attributeOpTerm(equationNode_left(e), original);
	attributeOpTerm(equationNode_right(e), duplicate);
	list_addh(equationNode, checks, e);
    } end_traverse;
    /* Rename axioms */
    propSet_extendCvt(axioms, t, rc, traitAttr_assertions(t));
    propSet_extendCvt(axioms, t, rc, traitAttr_assumptions(t));
    if (node_exists(exempt)) {
	vartable_merge(vars, exemptionAttr_vars(exempt), (renaming *)0);
    }
    /* Print comment, thaw command for axioms, declare command for duplicate */
    /* operators, assert commands for renamed axioms, declare command for    */
    /* new variables, assert commands for exemptions, and prove commands for */
    /* proof obligations.						     */
    print_setPrefix("%% ");
    unparse_conversion(cvt);
    print_setPrefix("");
    print_newline(1);
    print_str("thaw ");
    print_str(name);
    print_newline(2);
    symtable_print2LP(csyms, r0);
    propSet_print(axioms, TRUE, r0);
    vartable_print2LP(vars, traitAttr_symtab(t));
    if (node_exists(exempt)) {
	termNodeList *terms = exemptionNode_terms(exempt);
        bool printed = FALSE;
	print_str("set name exemptions");
	print_newline(2);
	print_str("assert ");
	print_newline(1);
	print_setIndent(2);
	list_traverse(termNode, terms, t) {
	    if (printed) print_str(";");
            printed = TRUE;
	    unparse_term(renaming_mapTerm(r0, t));
	    print_str(" = ");
	    unparse_term(renaming_mapTerm(r0, renaming_mapTerm(rc, t)));
	    print_newline(1);
	} end_traverse;
	print_str("..");
	print_newline(2);
	print_setIndent(0);
    }
    print_str("set name conversionChecks");
    print_newline(2);
    list_traverse(equationNode, checks, e) {
	print_str("prove ");
	unparse_equation(renaming_mapEqn(r0, e));
	print_newline(1);
	print_setIndent(2);
	print_str("qed");
	print_setIndent(0);
	print_newline(1);
    } end_traverse;
    print_newline(1);
    propSet_free(axioms);
    renaming_free(rc);
/*    vartable_free(vars);  */          /* FIX: too much freed? */
/*    symtable_free(csyms); */
    list_free(termNode, noArgs);	/* ENHANCE: free termNodes too     */
    list_free(equationNode, checks);	/* ENHANCE: free equationNodes too */
}


static void
openFile (char *name, char *ext) {
    char *fullName = string_paste(name, ext);
    if (!print_openFile(fullName, "")) {
	error_reportAndFail(string_paste("Can't open ", fullName));
    }
}


static void
deleteFile (char *name, char *ext) {
    unlink(string_paste(name, ext));
}


/*
** Translates the operator "*op" (by appending SIMPLEIDSUFFIX or SIMPLEOPSUFFIX
** to the first token in its name) into one that has a similar name, but does
** not occur in "*avoidOps".  Adds the new operator to "*newOps".
*/

static operator
*convert_op (operator *op, symtable *avoidOps, symtable *newOps) {
    nameNode	  *n = operator_name(op);
    signatureNode *sig = operator_signature(op);
    ppiOpNode	  *p;
    mixfixOpNode  *m;
    anyOpNode	  *a;
    LSLToken	  *t = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
    /* Make "*n" into a copy of itself, with "*t" being a token in the copy */
    /* that can be translated to change "*n" into a new name.               */
    switch (nameNode_kind(n)) {
    case simpleIdKIND:
	*t = *nameNode_simpleId(n);
	n = makeSimpleIdNameNode(t);
	break;
    case ifOpKIND:
	*t = IfToken;
	n = makeSimpleIdNameNode(t);
	break;
    case ppiOpKIND:
	p = nameNode_ppiOp(n);
	a = ppiOpNode_op(p);
	switch (anyOpNode_kind(a)) {
	case simpleOpKIND:
	    *t = *anyOpNode_simpleOp(a);
	    break;
	case logicalOpKIND:
	    *t = *anyOpNode_logicalOp(a);
	    break;
	case eqOpKIND:
	    *t = *anyOpNode_eqOp(a);
	    break;
	}
	a = makeSimpleAnyOpNode(t);
	n = makePpiOpNameNode(ppiOpNode_before(p), a, ppiOpNode_after(p));
	break;
    case mixfixOpKIND:
	m = nameNode_mixfixOp(n);
	*t = *mixfixOpNode_open(m);
	n = makeMixfixOpNameNode(mixfixOpNode_before(m), t, 
				 mixfixOpNode_seps(m), mixfixOpNode_close(m), 
				 mixfixOpNode_after(m));
	break;
    case selectOpKIND:
	*t = *selectOpNode_sel(nameNode_selectOp(n));
	n = makeSelectOpNameNode(t);
	break;
    case quantifierKIND:
	error_programFailure("convert_op");
    }
    for (;;) {
	char 		*id, *suffix;
	LSLTokenCode 	 code;
	id = token_text(*t);
	code = token_code(*t);
	suffix = SIMPLEIDSUFFIX;
	switch (code) {
	case LOGICALOP:
	case EQOP:
	    code = SIMPLEOP;
	    break;
	case OPENSYM:
	    break;
	case SIMPLEID:
	case ifTOKEN:
	    code = SIMPLEID;
	    break;
	case SIMPLEOP:
	    if (id[0] != '\\') suffix = SIMPLEOPSUFFIX;
	    break;
	default: 
	    error_programFailure("convert_op");
	}
	id = string_paste(id, suffix);
	*t = LSLInsertToken(code, LSLString2Symbol(id), 0, FALSE);
	if (!symtable_opExists(avoidOps, n, sig)) break;
    }
    /* ENHANCE: use routine that doesn't declare sorts in signatures */
    symtable_declareOp(newOps, n, sig);
    return operator_new(n, sig);
}
