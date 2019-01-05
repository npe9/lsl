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
**      FILENAME:   operator.h
**
**	PURPOSE:    Interface to operator abstraction
**
**	USAGE:	# include "abstract.h"
**		# include "operator.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.10.31 -- Extracted from symtable.h
**	{1} Garland at MIT   -- 91.12.20 -- Add OpList, operator_kind,
**					    operator_arity
**	{2} Garland at MIT   -- 92.06.14 -- Composite sorts
**	{3} Garland at MIT   -- 93.11.09 -- Add quantifiers
**	{4} Garland at MIT   -- 94.05.23 -- Define SortList
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define Sort		sortNode
# define SortList	sortNodeList
# define sameSort	LSLSameSort


/*
**
**  TYPEDEFS
**
*/

/* typedefs of operator and OpList are in abstract.h */

typedef enum {userOP, falseOP, trueOP, notOP, andOP, orOP, impliesOP, iffOP,
	      equalOP, ifOP, quantifierOP} operatorKind;

typedef set(Sort) 	SortSet;
typedef set(operator)	OpSet;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern operatorKind	 operator_kind(operator *op);
extern operator		*operator_new(nameNode *name, signatureNode *sig);

extern int		 operator_arity(operator *op);
# define		 operator_arity(pOp)			\
    				signatureNodeArity(pOp->signature)

extern bool	 	 operator_equal(operator *op1, operator *op2);

extern nameNode		*operator_name(operator *op);
# define		 operator_name(pOp)			\
    				((pOp)->name)

extern signatureNode	*operator_signature(operator *op);
# define		 operator_signature(pOp)		\
    				((pOp)->signature)

extern bool		 operator_unparse(operator *op);

extern OpSet		*OpSet_new(void);
extern SortSet		*SortSet_new(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
