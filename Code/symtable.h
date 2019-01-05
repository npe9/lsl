/*
**
**	Copyright (c) MIT, 1990, 1991, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   symtable.h
**
**	PURPOSE:    Interface to symbol table abstraction
**
**	USAGE:	# include "set.h"
**		# include "abstract.h"
**		# include "operator.h"
**		# include "renaming.h"
**		# include "symtable.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.10.02 -- Original
**      {1} Garland at MIT   -- 91.01.29 -- Add symtable_printSorts
**	{2} Garland at MIT   -- 91.12.20 -- Change symtable_unparse to
**					    symtable_print2LP, 
**					    add symtable_opExists
**	{3} Garland at MIT   -- 92.01.09 -- Recode as hash table, detect 
**					    overloaded ids, non-LP ids
**	{4} Garland at MIT   -- 92.01.12 -- Recode LP mixfix translation,
**					    add symtable_contains
**      {5} Garland at MIT   -- 92.01.12 -- Restore symtable_unparse
**      {6} Garland at MIT   -- 92.02.12 -- Add composeOpRenamings, free
**      {7} Garland at MIT   -- 92.02.16 -- Add DecimalLiterals
**      {8} Garland at MIT   -- 92.02.16 -- Add symtable_opExistsWithArity
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS (none)
**
*/


/*
**
**  TYPEDEFS (see renaming.h)
**
*/


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern symtable 	*symtable_blank(void);
extern symtable		*symtable_new(void);
extern void		 symtable_free(symtable *t);
extern void	 	 symtable_declareOp(symtable *t, nameNode *n, 
					    signatureNode *sig);
extern void	 	 symtable_declareOpAndSorts(symtable *t, nameNode *n, 
						    signatureNode *sig);
extern void	 	 symtable_declareSort(symtable *t, Sort *s);
extern void		 symtable_renamingWork(renaming *r, renaming *r1,
					       renaming *r2, symtable *t2);
extern bool	 	 symtable_contains(symtable *t1, symtable *t2, 
					   renaming *r, LSLToken *loc);
extern void		 symtable_enableLiterals(symtable *t, operator *plus1);
extern void	 	 symtable_extend(symtable *t1, symtable *t2, 
					 renaming *r);
extern bool		 symtable_isLiteral(symtable *t, operator *op);
extern bool	 	 symtable_legalVar(symtable *t, varIdLeaf *v, Sort *s);
extern bool		 symtable_markOverloadedConstants(symtable *t,
							  varIdLeaf *v);
extern OpSet		*symtable_matchingOps(symtable *t, nameNode *n,
					      signatureNode *sig);
extern bool	 	 symtable_opExists(symtable *t, nameNode *n, 
					   signatureNode *sig);
extern bool		 symtable_opExistsWithArity(symtable *t, nameNode *n, 
						    int arity);
extern OpSet		*symtable_opsWithLegalDomain(symtable *t, nameNode *n, 
						     list(SortSet) *argSorts,
						     Sort *s);
extern bool		 symtable_overloaded(symtable *t, operator *op);
extern bool		 symtable_overloadedDomain(symtable *t, operator *op);
extern bool	 	 symtable_sortExists(symtable *t, Sort *s);
extern void		 symtable_printSorts(symtable *t);
extern renaming	 	*symtable_print2LP(symtable *t, renaming *r);
extern void              symtable_unparse(symtable *t);
extern bool		 isLiteralToken (LSLToken *t);
extern bool		 isLiteralName (nameNode *n);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
