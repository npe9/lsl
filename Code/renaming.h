/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1995
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   renaming.h
**
**	PURPOSE:    Interface to renaming abstraction
**
**	USAGE:	# include "set.h"
**		# include "abstract.h"
**		# include "renaming.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland  at MIT     -- 90.10.30 -- Original
**    	{1} Garland  at MIT     -- 91.12.20 -- Add routines to rename props
**	{2} Garland  at MIT     -- 92.01.09 -- Enhanced renaming_compose
**	{3} Garland  at MIT     -- 92.02.12 -- Add renaming_free
**	{4} Garland  at MIT     -- 92.02.16 -- Add DecimalLiterals
**	{5} Garland  at MIT     -- 92.02.23 -- Add renaming_composeSorts
**      {6} Garland  at MIT     -- 95.01.23 -- Remove renaming_composeSorts
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS (none)
**
*/


/*
**
**  TYPEDEFS
**
*/

typedef struct {
    Sort	*from;
    Sort	*to;
} SortRename;

typedef struct {
    operator	*from;
    operator	*to;
    bool	 signatureRenamed;
} OpRename;

typedef set(SortRename) SortMap;

typedef set(OpRename) OpMap;

typedef struct _renaming {
    SortMap	*sorts;
    OpMap	*operators;
    bool	 okToAddSorts;
} renaming;

/* 
** The following are NOT representation invariants of a renaming:
**    No SortRename maps a sort to itself.
**    No OpRename maps an operator to one with the same name.
** Context-sensitive checking uses the existence of such maps to detect
** multiple renamings in constructs such as
**    includes T(f for f, g for f)
*/

/* 
** The following typedefs belong in symtable.h, but are needed for the function
** prototype for renaming_compose.
*/

typedef struct _opBucket {
    operator		*op;
    bool		 overloaded;
    bool		 overloadedDomain;
    struct _opBucket	*next;
} opBucket;

typedef struct _symtable {
    SortSet		*sorts;
    opBucket	 	*buckets;
    unsigned int	 nOps;
    OpSet		*plus1, *literals;
} symtable;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern renaming		*renaming_new(void);
extern void		 renaming_free(renaming *r);
extern bool	 	 renaming_addSort(renaming *r, Sort *from, Sort *to);
extern bool	 	 renaming_addOp(renaming *r, operator *from,
					nameNode *to);
extern renaming		*renaming_compose(renaming *r1, renaming *r2, 
					  symtable *s2);
extern void		 renaming_composeWork(renaming *r, renaming *r1, 
					      renaming *r2, operator *op);
extern bool		 renaming_equal(renaming *r1, renaming *r2);	
extern bool		 renaming_isIdentity(renaming *r);
extern equationNode	*renaming_mapEqn(renaming *r, equationNode *n);
extern genByNode	*renaming_mapGenBy(renaming *r, genByNode *n);
extern operator		*renaming_mapOp(renaming *r, operator *op);
extern partByNode	*renaming_mapPartBy(renaming *r, partByNode *n);
extern Sort		*renaming_mapSort(renaming *r, Sort *s);
extern termNode		*renaming_mapTerm(renaming *r, termNode *n);
extern int		 renaming_nSorts(renaming *r);
extern int		 renaming_nOps(renaming *r);

extern void		 renaming_unparse(renaming *r);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
