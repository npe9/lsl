/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   operator.c
**
**	PURPOSE:    Operator abstraction
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT      -- 90.10.31 -- Extracted from symtable.c
**	{1} Wild    at Digital  -- 90.11.09 -- Include osdfiles.h
**	{2} Wild    at Digital  -- 90.12.10 -- Fix operator_unparse.
**	{3} Garland at MIT      -- 91.12.20 -- Use print.c for output, 
**					       add operator_kind
**      {4} Garland at MIT      -- 92.02.19 -- Reorder #include "error.h"
**      {5} Garland at MIT      -- 92.02.23 -- Fix use of nameNodeMatches
**	{6} Garland at MIT      -- 93.03.08 -- Treat \neq as equalOP (so it 
**					       won't appear in LP declarations)
**	{6} Garland at MIT      -- 93.11.09 -- Add quantifiers
**      {n} Who     at Where    -- yy.mm.dd -- What
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
# include	"error.h"
# include	"list.h"
# include	"set.h"
# include	"parse.h"
# include	"abstract.h"
# include	"operator.h"
# include	"print.h"
# include	"renaming.h"
# include	"symtable.h"
# include	"vartable.h"
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

extern operatorKind	 operator_kind();
extern operator		*operator_new();
extern bool		 operator_equal();
extern bool		 operator_unparse();


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


extern operatorKind
operator_kind(operator *op) {
    LSLToken	*tok;
    anyOpNode	*anyOp;
    switch (nameNode_kind(op->name)) {
    case simpleIdKIND:
	tok = nameNode_simpleId(op->name);
	if (tok == &FalseToken) return falseOP;
	if (tok == &TrueToken) return trueOP;
	return userOP;
    case ifOpKIND:
	return ifOP;
    case ppiOpKIND:
	anyOp = ppiOpNode_op(nameNode_ppiOp(op->name));
	switch (anyOpNode_kind(anyOp)) {
	case simpleOpKIND:
	    if (anyOpNode_simpleOp(anyOp) == &NotToken) return notOP;
	    return userOP;
	case logicalOpKIND:
	    tok = anyOpNode_logicalOp(anyOp);
	    if (tok == &AndToken) return andOP;
	    if (tok == &OrToken) return orOP;
	    if (tok == &ImpliesToken) return impliesOP;
	    if (tok == &IffToken) return iffOP;
	    return userOP;
	case eqOpKIND:
	    return equalOP;
	}
    case quantifierKIND:
	return quantifierOP;
    default:
	return userOP;
    }
}


extern operator
*operator_new (nameNode *name, signatureNode *sig) {
    operator *op = (operator *)LSLMoreMem((void *)0, sizeof(operator));
    op->name = name;
    op->signature = sig;
    return op;
}


extern bool
operator_equal (operator *op1, operator *op2) {
    return (nameNodeMatches(op1->name, op2->name, FALSE) 
	    && LSLSameSignature(op1->signature, op2->signature));
}


extern bool
operator_unparse (operator *op) {
    unparse_name(op->name);
    unparse_signature(op->signature);
    return TRUE;
}


extern OpSet
*OpSet_new (void) {
    return set_new(operator, operator_equal);
}


extern SortSet
*SortSet_new (void) {
    return set_new(Sort, sameSort);
}
