/*
**
**	Copyright (c) MIT, 1991, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   semantics.h
**
**	PURPOSE:    Interface to semantic attribution for traits
**
**	     The process of semantic attribution collects the following
**	information for each trait T:
**
**	traitAttr_assertions(T) [propSet]
**	     header prop for T
**	     props generated from T's shorthands
**	     props asserted directly in T
**	     renamed props from traitAttr_assertions(T1), for T1 included in T
**	        Note: by recursion, for any trait T2 below T1 in the inclusion
**              hierarchy, (an appropriately renamed) traitAttr_assetions(T2) 
**		is a subset of traitAttr_assertions(T)
**	traitAttr_assumptions(T) [propSet]
**	     renamed props from traitAttr_assertions(T1) and
**			        traitAttr_assumptions(T1), for T1 assumed in T
**		Note: In the LSL report, this set of props is called the set of
**		local assumptions
**	traitAttr_checkAssumes(T) [propSet]
**	     renamed props from traitAttr_assumptions(T1), for T1 included in T
**		Note: In the LSL report, this set of props is called the set of
**		inherited assumptions.
**	traitAttr_consequences(T) [propSet]
**	     props directly implied by T
**	     renamed props from traitAttr_assertions(T1), for T1 implied by T
**		Note: The LSL report does not seem to put
**		traitAttr_assumptions(T1) into traitAttr_consequences(T).
**	traitAttr_assertVars(T) [vartable]
**	     the variables in traitAttr_assertions(T)
**	traitAttr_assumeVars(T) [vartable]
**	     the variables in traitAttr_assumptions(T)
**	traitAttr_checkAssumesVars(T)
**	     the variables in traitAttr_checkAssumes(T)
**	traitAttr_conseqVars(T) [vartable]
**	     the variables in traitAttr_consequences(T)
**
**	     NOTE: The LSL report defines the theory of T to be the union of
** 	traitAttr_assertions(T) and traitAttr_assumptions(T).  Checking T 
**	involves (in part) showing that traitAttr_checkAssumes(T) and 
**	traitAttr_consequences(T) follow from the theory of T (it also
**	involves checking consistency and conversions).
**
**	     ENHANCE: collect (or compute) the following information when
**	generating output for LP:
**
**	traitAttr_theorems(T)
**	     props in traitAttr_consequences(T)
**	     props in traitAttr_consequences(T1), for T1 implied* by T
**		Here implied* is the transitive closure of the relation
**		    {<T1, T2>: T1 implies T2}
**		Note: Once all proof obligations have been discharged for all
**		traits, traitAttr_theorems(T) are known to follow from T and
**		can be used to check T's converts clauses.
**	traitAttr_implies_lemmas(T)
**	     props in traitAttr_consequences(T2), for T2 T-implied* by some T1
**	               less than T in the trait inclusion/assumption hierarchy
**		Here T-implied* is the transitive closure of the relation
**		    {<T1, T2>: T1 implies T2, T1 < T, and T2 < T}
**              Note: The use of T-implied* rather than implied* guarantees 
**		soundness  by imposing a bottom-up checking order on the trait 
**		hierarchy.  Less rigid checking orders are possible, but 
**		difficult to describe statically.
**	traitAttr_assumes_lemmas(T)
**	     props in traitAttr_consequences(T2), for T2 T-implied* by some T1
**		       less than T in the trait inclusion hierarchy
**		Note: We cannot use the consequences of traits assumed by T
**		when checking T's inherited assumptions (e.g., because an
**		assumed trait might imply itself).
**		
**		
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 91.12.20 -- Original
**      {1} Garland at MIT   -- 92.08.21 -- Updated comments
**      {2} Garland at MIT   -- 94.05.18 -- Updated comments
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

extern void	doSemantics (traitNodeList *allTraits);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
