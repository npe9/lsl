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
**      FILENAME:   unparse.c
**
**	PURPOSE:    Displays representation of abstract syntax
**
**	     The current unparsing routines insert spaces around all symbols
**	other than parentheses and periods in a term to ensure that tokens
**	(e.g., as defined in an init file) are properly separated.  The
**	routines could be smarter, e.g., by testing whether the token just
**	printed ended with an alphanumeric.
**	
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.09.14 -- Original
**	{1} Wild at Digital  -- 90.11.09 -- Change ## to PASTE().
**	{2} Wild at Digital  -- 90.12.10 -- Add unparse_indent
**      {3} Garland at MIT   -- 91.01.29 -- Add unparse_sort (removed in {6})
**	{4} Wild at Digital  -- 91.02.20 -- Merge in MIT changes.
**	{5} Garland at MIT   -- 91.12.20 -- Change to use print.c, expose more
**	{6} Garland at MIT   -- 91.12.26 -- Add parentheses for right 
**					    associated ops, move some indents
**					    out of the way of lsl2lp
**      {7} Garland at MIT   -- 92.06.14 -- Add unparse_sort (again)
**	{8} Garland at MIT   -- 92.09.30 -- Add "node_exists(n->attr)" to
**					    unparse_parameter
**      {9} Garland at MIT   -- 93.03.08 -- Translate \neq to not(=) for LP
**					    Parenthesize (p /\ q) = r
**     {10} Garland at MIT   -- 93.09.23 -- Change for LP3.1
**     {11} Garland at MIT   -- 93.11.09 -- Add quantified formulas
**     {12} Garland at MIT   -- 93.11.11 -- Make symbols accumulate
**     {13} Garland at MIT   -- 93.11.26 -- Add iff
**     {14} Garland at MIT   -- 94.03.10 -- Add "sort" to LP genPartBy
**     {15} Garland at MIT   -- 94.04.14 -- Change unparse_sort to use S[A,B]
**					    Change a:S[1] to (a:S)[1]
**     {16} Garland at MIT   -- 94.04.20 -- Add "generated freely"
**     {17} Garland at MIT   -- 94.05.23 -- Composite sort notation S[A, B]
**      {n} Who  at Where    -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include <string.h>
# include		"general.h"
# include		"osdfiles.h"
# include		"string2sym.h"
# include		"token.h"
# include		"error.h"
# include		"list.h"
# include		"set.h"
# include		"parse.h"
# include		"abstract.h"
# include		"operator.h"
# include		"renaming.h"
# include		"symtable.h"
# include		"vartable.h"
# include		"prop.h"
# include		"propSet.h"
# include		"checking.h"
# include		"print.h"
# include		"unparse.h"
# include		"lsl.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define TAB 	"   "
# define LPAREN	'('
# define RPAREN ')'
# define SPACE	' '


# define print(node, field)					\
     printLeaf((node)->body->field)

# define printNodeList(type, L, before, sep, after)		\
    if (L != 0) {						\
	bool printed = FALSE;					\
	print_str(before);					\
	list_traverse(PASTE(type, Node), L, theNode) {		\
	    if (printed) print_str(sep);			\
	    printed = TRUE;					\
	    PASTE(unparse_, type) (theNode);			\
	} end_traverse;						\
	print_str(after);					\
    }

# define printLeaf(node)					\
     print_str(LSLSymbol2String((node)->text))

# define printLeafList(type, L, before, sep, after)		\
    if (L != 0) {						\
	bool printed = FALSE;					\
	print_str(before);					\
	list_traverse(PASTE(type, Leaf), L, theLeaf) {		\
	    if (printed) print_str(sep);			\
	    printed = TRUE;					\
	    printLeaf(theLeaf);					\
	} end_traverse;						\
	print_str(after);					\
    }


/*
**
**  TYPEDEFS
**
*/


/*
**
**  FORWARD FUNCTIONS
**
*/

extern void unparse_setForLP(bool lp);
extern void unparse_resetSymbols(void);
extern void unparse_resetVariables(void);
extern void unparse_addSymbols(symtable *st);
extern void unparse_addVariables(vartable *vt);

extern void unparse_trait();
extern void unparse_name();
extern void unparse_signature();
extern void unparse_sort();
extern void unparse_genBy();
extern void unparse_partBy();
extern void unparse_equation();
extern void unparse_term();
extern void unparse_conversion();
extern void unparse_token();

static void unparse_parameter(parameterNode *n);
static void unparse_ppiOp(ppiOpNode *n);
static void unparse_mixfixOp(mixfixOpNode *n);
static void unparse_selectOp(selectOpNode *n);
static void unparse_anyOp(anyOpNode *n);
static void unparse_separator(separatorNode *n);
static void unparse_ext(extNode *n);
static void unparse_enumeration(enumerationNode *n);
static void unparse_tuple(tupleNode *n);
static void unparse_union(unionNode *n);
static void unparse_field(fieldNode *n);
static void unparse_includes(includesNode *n);
static void unparse_assumes(assumesNode *n);
static void unparse_traitRef(traitRefNode *n);
static void unparse_replace(replaceNode *n);
static void unparse_opPart(opPartNode *n);
static void unparse_opDcl(opDclNode *n);
static void unparse_propPart(propPartNode *n);
static void unparse_genPartBy(genPartByNode *n);
static void unparse_operator(operatorNode *n);
static void unparse_quantEqns(quantEqnsNode *n);
static void unparse_varDcl(varDclNode *n);
static void unparse_qual(termNode *n);
static void unparse_consequences(consequencesNode *n);
static void unparse_exemption(exemptionNode *n);

static bool ppiParens(nameNode *n, termNode *subterm, bool right);
static bool nonPpiParens(termNode *subterm);
static void unparse_indent(int n);


/*
**
**  STATIC VARIABLES
**
*/

static symtable *symbols;
static vartable *variables;
static bool 	 forLP 		= FALSE;


/*
**
**  FUNCTION DEFINITIONS
**
*/


extern void unparse_setForLP (bool lp) {
    forLP = lp;
}


extern void unparse_resetSymbols (void) {
    symbols = symtable_new();
}


extern void unparse_resetVariables (void) {
    variables = vartable_new();
}


extern void unparse_addSymbols (symtable *st) {
  symtable_extend(symbols, st, (renaming *)0);
}


extern void unparse_addVariables (vartable *vt) {
  vartable_merge(variables, vt, (renaming *)0);
}


extern void
unparse_trait(traitNode *n) {
    print_setIndent(0);
    print(n, name);
    printNodeList(parameter, n->body->formals, " (", ", ", ")");
    print_str(": trait");
    print_newline(1);
    unparse_indent(1);
    printNodeList(ext, n->body->exts, "", "", "");
    printNodeList(opPart, n->body->introduces, "", "", "");
    printNodeList(propPart, n->body->asserts, "", "", "");
    unparse_consequences(n->body->implies);
}


extern void
unparse_name(nameNode *n) {
    switch(n->body->kind) {
    case simpleIdKIND:
	print(n, choice.simpleId);
	break;
    case ifOpKIND:
	print_str("if __ then __ else __");
	break;
    case ppiOpKIND:
	unparse_ppiOp(n->body->choice.ppiOp);
	break;
    case mixfixOpKIND:
	unparse_mixfixOp(n->body->choice.mixfixOp);
	break;
    case selectOpKIND:
	unparse_selectOp(n->body->choice.selectOp);
	break;
    case quantifierKIND:
	print(n, choice.quantifier);
	break;
    }
}


extern void
unparse_signature(signatureNode *n) {
    printNodeList(sort, n->body->domain, ": ", ", ", "");
    if (list_size(sortNode, n->body->domain) > 0) print_char(' ');
    print_str("-> ");
    unparse_sort(signatureNode_range(n));
}


extern void
unparse_sort(sortNode *n) {
    print(n, id);
    printNodeList(sort, n->body->subsorts, "[", ", ", "]");
}


extern void
unparse_genBy(genByNode *n) {
    if (forLP) print_str("sort ");
    unparse_sort(genByNode_sort(n));
    if genByNode_freely(n) {
      printNodeList(operator, n->body->ops, " generated freely by ", ", ", "");
    } else {
      printNodeList(operator, n->body->ops, " generated by ", ", ", "");
    }
}


extern void
unparse_partBy(partByNode *n) {
    if (forLP) print_str("sort ");
    unparse_sort(partByNode_sort(n));
    printNodeList(operator, n->body->ops, " partitioned by ", ", ", "");
}


extern void
unparse_equation(equationNode *n) {
    if (forLP) {print_str("(");}
    unparse_term(n->body->left);
    if (forLP) {print_str(")");}
    if (n->body->right != 0) {
	if (forLP) {
	    print_str(" = ");
	} else {
	    print_str(" == ");
	}
	if (forLP) {print_str("(");}
	unparse_term(n->body->right);
	if (forLP) {print_str(")");}
    }
}


static bool
qual_term(termNode *n) {
    bool      qual 	= n->body->qual != 0 || GetPrintQualFlag();
    if (forLP && !qual) {
	operator *op;
	variable *v;
	switch (termAttr_kind(n)) {
	case OPTERM:
	    op = set_choose(operator, termAttr_possibleOps(n));
	    qual = symtable_overloadedDomain(symbols, op);
	    break;
	case VARTERM:
	    v = set_choose(variable, termAttr_possibleVars(n));
	    qual = vartable_overloaded(variables, v->id);
	    break;
        case QUANTIFIEDTERM:
	    qual = FALSE;
	    break;
	default:
	    error_programFailure("qual_term");
	}
    }
    return qual;
  }


extern void
unparse_term(termNode *n) {
    nameNode *nm 	= n->body->varOrOp;
    termNode *subterm;
    bool      qual 	= qual_term(n);
    bool      parens;
    switch(nm->body->kind) {
    case simpleIdKIND:
	print(nm, choice.simpleId);
	if (n->body->args != 0 && list_size(termNode, n->body->args) > 0)
	    printNodeList(term, n->body->args, "(", ", ", ")");
	if (qual) unparse_qual(n);
	break;
    case ifOpKIND:
	if (qual) print_char(LPAREN);
	print_str("if ");
	list_pointToFirst(termNode, n->body->args);
	unparse_term(list_current(termNode, n->body->args));
	print_str(" then ");
	list_pointToNext(termNode, n->body->args);
	unparse_term(list_current(termNode, n->body->args));
	print_str(" else ");
	list_pointToNext(termNode, n->body->args);
	unparse_term(list_current(termNode, n->body->args));
	if (qual) {
	  print_char(RPAREN);
	  unparse_qual(n);
	}
	break;
    case ppiOpKIND:
	if (qual) print_char(LPAREN);
	list_pointToFirst(termNode, n->body->args);
	if (nm->body->choice.ppiOp->body->before) {
	    subterm = list_current(termNode, n->body->args);
	    parens = ppiParens(nm, subterm, FALSE);
	    if (parens) print_char(LPAREN);
	    unparse_term(subterm);
	    if (parens) print_char(RPAREN);
	    list_pointToNext(termNode, n->body->args);
	    print_char(' ');
	}
	unparse_anyOp(nm->body->choice.ppiOp->body->op);
	if (nm->body->choice.ppiOp->body->after) {
	    print_char(' ');
	    subterm = list_current(termNode, n->body->args);
	    parens = ppiParens(nm, subterm, TRUE);
	    if (parens) print_char(LPAREN);
	    unparse_term(subterm);
	    if (parens) print_char(RPAREN);
	}
	if (qual) {
	    print_char(RPAREN);
	    if (qual) unparse_qual(n);
	}
	break;
    case mixfixOpKIND:
	{ mixfixOpNode *m;
	  m = nm->body->choice.mixfixOp;
	  if (n->body->args != 0) {
	      list_pointToFirst(termNode, n->body->args);
	  }
	  if (m->body->before) {
	      subterm = list_current(termNode, n->body->args);
	      parens = nonPpiParens(subterm) || qual_term(subterm);
	      if (parens) print_char(LPAREN);
	      unparse_term(subterm);
	      if (parens) print_char(RPAREN);
	      list_pointToNext(termNode, n->body->args);
	      print_char(' ');
	  }
	  print(m, open);
	  print_char(' ');
	  if (m->body->seps != 0) {
	      unparse_term(list_current(termNode, n->body->args));
	      list_pointToNext(termNode, n->body->args);
	      list_traverse(separatorNode, m->body->seps, sep) {
		  unparse_separator(sep);
		  print_char(' ');
		  unparse_term(list_current(termNode, n->body->args));
		  list_pointToNext(termNode, n->body->args);
	      } end_traverse;
	      print_char(' ');
	  }
	  print(m, close);
	  if (qual) unparse_qual(n);
	  if (m->body->after) {
	      print_char(' ');
	      subterm = list_current(termNode, n->body->args);
	      parens = nonPpiParens(subterm);
	      if (parens) print_char(LPAREN);
	      unparse_term(list_current(termNode, n->body->args));
	      if (parens) print_char(RPAREN);
	  }
      }
	break;
    case selectOpKIND:
	list_pointToFirst(termNode, n->body->args);
	subterm = list_current(termNode, n->body->args);
	parens = nonPpiParens(subterm);
	if (parens) print_char(LPAREN);
	unparse_term(subterm);
	if (parens) print_char(RPAREN);
	print_char('.');
	print(nm->body->choice.selectOp, sel);
	if (qual) unparse_qual(n);
	break;
    case quantifierKIND:
	print(nm, choice.quantifier);
	print_char(' ');
	list_pointToFirst(termNode, n->body->args);
	unparse_term(list_current(termNode, n->body->args));
	list_pointToNext(termNode, n->body->args);
	subterm = list_current(termNode, n->body->args);
	switch (subterm->body->varOrOp->body->kind) {
	case quantifierKIND:
	case simpleIdKIND:
	  parens = FALSE;
	  break;
	case ifOpKIND:
	case mixfixOpKIND:
	case selectOpKIND:
	  parens = TRUE;
	  break;
	case ppiOpKIND:
	  parens = subterm->body->varOrOp->body->choice.ppiOp->body->after;
	  break;
	}
	print_char(' ');
	if (parens) print_char(LPAREN);
	unparse_term(subterm);
	if (parens) print_char(RPAREN);
    }
}


extern void
unparse_conversion(conversionNode *n) {
    printNodeList(operator, n->body->converts, "converts ", ", ", "");
    if (n->body->exempts != 0) {
	unparse_exemption(n->body->exempts);
    }
    print_newline(1);
}


extern void 
unparse_token(LSLToken *t) {
    printLeaf(t);
}


static void
unparse_parameter(parameterNode *n) {
    operatorNode *opn;
    operator     *op;
    switch (parameterNode_kind(n)) {
    case operatorKIND:
	opn = parameterNode_operator(n);
	if (node_exists(n->attr)) op = parameterAttr_op(n);
	unparse_name(operatorNode_name(opn));
	if ((forLP && symtable_overloaded(symbols, op)) ||
	    (GetPrintQualFlag() && node_exists(n->attr) && node_exists(op))) {
	    unparse_signature(operator_signature(op));
	} else {
	    signatureNode *s = operatorNode_signature(opn);
	    if (node_exists(s)) unparse_signature(s);
	}
	break;
    case sortKIND:
	unparse_sort(parameterNode_sort(n));
	break;
    case simpleIdKIND:
	printLeaf(parameterNode_simpleId(n));
    }
}


static void
unparse_ppiOp(ppiOpNode *n) {
    if (n->body->before) print_str("__ ");
    unparse_anyOp(n->body->op);
    if (n->body->after) print_str(" __");
}


static void
unparse_mixfixOp(mixfixOpNode *n) {
    if (n->body->before) print_str("__ ");
    print(n, open);
    if (n->body->seps != 0) {
	print_str(" __ ");
	list_traverse(separatorNode, n->body->seps, sep) {
	    unparse_separator(sep);
	    print_str(" __ ");
	} end_traverse;
    }
    print(n, close);
    if (n->body->after) print_str(" __");
}


static void
unparse_selectOp(selectOpNode *n) {
    if (n->body->before) print_str("__");
    print_str(".");
    print(n, sel);
}


static void
unparse_anyOp(anyOpNode *n) {
  switch(n->body->kind) {
  case simpleOpKIND:
    if (forLP && (token_text(*n->body->choice.simpleOp) == 
		   token_text(NotToken))) {
      print_char('~');
    } else {
      print(n, choice.simpleOp);
    }
    break;
  case logicalOpKIND:
    if (!forLP) {
      print(n, choice.logicalOp);
    } else if (token_text(*n->body->choice.logicalOp) == 
               token_text(AndToken)) {
      print_str("/\\");
    } else if (token_text(*n->body->choice.logicalOp) == 
	       token_text(OrToken)) {
      print_str("\\/");
    } else if (token_text(*n->body->choice.logicalOp) == 
	       token_text(ImpliesToken)) {
      print_str("=>");
    } else {
      print_str("<=>");
    }
    break;
  case eqOpKIND:
    if (!forLP) {
      print(n, choice.eqOp);
    } else if (token_text(*n->body->choice.eqOp) == token_text(EqToken)) {
      print_char('=');
    } else if (token_text(*n->body->choice.eqOp) == token_text(NeqToken)) {
      print_str("~=");
    } else {
      print(n, choice.eqOp);
    }
    break;
  }
}


static void
unparse_separator(separatorNode *n) {
    if (n->body->separator == 0) {
	print_str(",");
    } else {
	print(n, separator);
    }
}


static void
unparse_ext(extNode *n) {
  switch(n->body->kind) {
  case LSLenumKIND:
    unparse_enumeration(n->body->choice.LSLenum);
    break;
  case LSLtupleKIND:
    unparse_tuple(n->body->choice.LSLtuple);
    break;
  case LSLunionKIND:
    unparse_union(n->body->choice.LSLunion);
    break;
  case includesKIND:
    unparse_includes(n->body->choice.includes);
    break;
  case assumesKIND:
    unparse_assumes(n->body->choice.assumes);
    break;
  }
}


static void
unparse_enumeration(enumerationNode *n) {
    unparse_sort(enumerationNode_sort(n));
    printLeafList(elementId, n->body->elems, " enumeration of ", ", ", "");
    print_newline(1);
}


static void
unparse_tuple(tupleNode *n) {
    unparse_sort(tupleNode_sort(n));
    printNodeList(field, n->body->fields, " tuple of ", ", ", "");
    print_newline(1);
}


static void
unparse_union(unionNode *n) {
    unparse_sort(unionNode_sort(n));
    printNodeList(field, n->body->fields, " union of ", ", ", "");
    print_newline(1);
}


static void
unparse_field(fieldNode *n) {
    printLeafList(fieldId, n->body->fields, "", ", ", ": ");
    unparse_sort(fieldNode_sort(n));
}


static void
unparse_includes(includesNode *n) {
    printNodeList(traitRef, n->body->traits, "includes ", ", ", "");
    print_newline(1);
}


static void
unparse_assumes(assumesNode *n) {
    printNodeList(traitRef, n->body->traits, "assumes ", ", ", "");
    print_newline(1);
}


static void
unparse_traitRef(traitRefNode *n) {
    bool oneTrait;
    bool actuals;
    bool renamings;
    oneTrait = n->body->traits->first->next == 0;
    actuals = n->body->actuals != 0;
    renamings = n->body->renamings != 0;
    if (oneTrait) {
	printLeafList(traitId, n->body->traits, "", "", "");
    } else {
	printLeafList(traitId, n->body->traits, "(", ", ", ")");
    }
    if (actuals || renamings) print_str("(");
    if (actuals) {
	printNodeList(name, n->body->actuals, "", ", ", "");
	if (renamings) print_str(", ");
    }
    if (renamings) printNodeList(replace, n->body->renamings, "", ", ", "");
    if (actuals || renamings) print_str(")");

}


static void
unparse_replace(replaceNode *n) {
    unparse_parameter(n->body->actual);
    print_str(" for ");
    unparse_parameter(n->body->formal);
}


static void
unparse_opPart(opPartNode *n) {
    unparse_indent(1);
    print_str("introduces");
    print_newline(1);
    unparse_indent(2);
    list_traverse(opDclNode, n->body->introduces, opd) {
	unparse_opDcl(opd);
	print_newline(1);
    } end_traverse;
    unparse_indent(1);
}


static void
unparse_opDcl(opDclNode *n) {
    printNodeList(name, n->body->ops, "", ", ", "");
    unparse_signature(n->body->signature);
}


static void
unparse_propPart(propPartNode *n) {
    unparse_indent(1);
    print_str("asserts");
    print_newline(1);
    printNodeList(genPartBy, n->body->genPartBys, "", "", "");
    if (n->body->equations != 0) {
	unparse_indent(2);
	print_str("equations");
	print_newline(1);
	list_traverse(equationNode, n->body->equations, eq) {
	    unparse_indent(3);
	    unparse_equation(eq);
	    print_newline(1);
	} end_traverse;
    }
    printNodeList(quantEqns, n->body->quantEqns, "", "", "");
    unparse_indent(1);
}


static void
unparse_genPartBy(genPartByNode *n) {
    unparse_indent(2);
    switch(n->body->kind) {
    case genByKIND:
	unparse_genBy(n->body->choice.genBy);
	break;
    case partByKIND:
	unparse_partBy(n->body->choice.partBy);
	break;
    }
    print_newline(1);
}


static void
unparse_operator(operatorNode *n) {
    operator *op = operator_declaration(n);
    if ((forLP && symtable_overloaded(symbols, op)) ||
	(GetPrintQualFlag() && node_exists(op))) {
	unparse_name(operator_name(op));
	unparse_signature(operator_signature(op));
    } else {
	signatureNode *s = operatorNode_signature(n);
	unparse_name(operatorNode_name(n));
	if (node_exists(s)) unparse_signature(s);
    }
}


static void
unparse_quantEqns(quantEqnsNode *n) {
    unparse_indent(2);
    printNodeList(varDcl, n->body->quantifier, "\\forall ", ", ", "");
    print_newline(1);
    list_traverse(equationNode, n->body->equations, eq) {
	unparse_indent(3);
	unparse_equation(eq);
	print_newline(1);
    } end_traverse;
}


static void
unparse_varDcl(varDclNode *n) {
    printLeafList(varId, n->body->vars, "", ", ", ": ");
    unparse_sort(varDclNode_sort(n));
}


static void
unparse_qual(termNode *n) {
    Sort *s = n->body->qual;
    if (s == 0) {
	s = term_sort(n);
	if (s == 0) return;
    }
    print_char(':');
    unparse_sort(s);
}


static void
unparse_consequences(consequencesNode *n) {
    if (n == 0) return;
    unparse_indent(1);
    print_str("implies");
    print_newline(1);
    if (n->body->traits != 0) {
	unparse_indent(2);
	printNodeList(traitRef, n->body->traits, "", ", ", "");
	print_newline(1);
    }
    if (n->body->genPartBys != 0) {
    }
    printNodeList(genPartBy, n->body->genPartBys, "", "", "");
    if (n->body->eqns != 0) {
	unparse_indent(2);
	print_str("equations");
	print_newline(1);
	list_traverse(equationNode, n->body->eqns, eq) {
	    unparse_indent(3);
	    unparse_equation(eq);
	    print_newline(1);
	} end_traverse;
    }
    printNodeList(quantEqns, n->body->quantEqns, "", "", "");
    unparse_indent(2);
    printNodeList(conversion, n->body->converts, "", "", "");
}


static void
unparse_exemption(exemptionNode *n) {
    print_str(" exempting ");
    printNodeList(varDcl, n->body->forall, "\\forall ", ", ", "");
    printNodeList(term, n->body->terms, " ", ", ", "");
}


static bool
ppiParens(nameNode *n, termNode *subterm, bool right) {
    anyOpNode *mainAnyOp;
    anyOpNode *subAnyOp;
    switch(subterm->body->varOrOp->body->kind) {
    case mixfixOpKIND:
    case selectOpKIND:
    case simpleIdKIND:
    case quantifierKIND:
	return FALSE;
    case ifOpKIND:
	return TRUE;
    case ppiOpKIND:
	break;
    }
    mainAnyOp = n->body->choice.ppiOp->body->op;
    subAnyOp = subterm->body->varOrOp->body->choice.ppiOp->body->op;
    switch(mainAnyOp->body->kind) {
    case simpleOpKIND:
	return right || (subAnyOp->body->kind != simpleOpKIND) ||
	    (subAnyOp->body->choice.simpleOp->text !=
	     mainAnyOp->body->choice.simpleOp->text);
    case logicalOpKIND:
	return (subAnyOp->body->kind == logicalOpKIND) &&
	    (right || (subAnyOp->body->choice.logicalOp->text !=
		       mainAnyOp->body->choice.logicalOp->text)); 
    case eqOpKIND:
	return right || subAnyOp->body->kind != simpleOpKIND;
    }
    return FALSE;
}


static bool
nonPpiParens(termNode *subterm) {
    switch(subterm->body->varOrOp->body->kind) {
    case selectOpKIND:
    case simpleIdKIND:
	return FALSE;
    default:
	return TRUE;
    }
}


static void
unparse_indent(int n) {
    print_setIndent(n*3);
}
