/*
**
**	Copyright (c) MIT, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   recheck.h
**
**	PURPOSE:    Context-sensitive checking for referenced traits
**		
**	     The definition of LSL requires that the normalization of a trait
**	containing traitRef's be subjected to the context-sensitive checks 
**	defined for LSL without traitRef's.  Assuming that the referenced trait
**	has already been checked, the following checks are the only ones that
**	must be applied (and then only when the renaming associated with the
**	traitRef contains a sortReplace):
**
**   	Section 2.1 (The Semantic Core Language).  Each generators in the 
**	traitRef must contain at least one operator such that the sort of the
**	generators does not occur in the domain of the operator.
**
**	Section 2.2 (Simple Traits).  No simpleId may occur more than once with
**	the same sort in a quantifier.
**
**	Section 2.12 (Shorthands).  No sort in a field may be the same as the 
**	sort of the enclosing tuple or union.  (This check catches problems
**	with the generators in the normalization of the shorthand.)
**
**		
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 92.02.23 -- Original
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
**  TYPEDEFS (none)
**
*/


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void	recheckTraitRefs (symtable *st, bool reset, 
				  traitRefNodeList *refs);
extern void	recheck_error (void);
extern void	recheck_sortError (void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
