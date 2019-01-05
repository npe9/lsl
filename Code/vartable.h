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
**      FILENAME:   vartable.h
**
**	PURPOSE:    Interface to variable table abstraction
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.10.05 -- Original
**	{1} Garland at MIT   -- 91.12.20 -- Add inventVar, merge, print2LP
**	{2} Garland at MIT   -- 92.01.08 -- Recode as hash table, implement
**					    overloaded id detection
**	{3} Garland at MIT   -- 92.01.12 -- Add vartable_free
**	{4} Garland at MIT   -- 92.02.23 -- Add vartable_recheck
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
**  TYPEDEFS
**
*/

typedef struct {
    varIdLeaf	*id;
    Sort	*sort;
} variable;

typedef struct _varBucket {
    variable		*var;
    bool		 overloaded;
    struct _varBucket	*next;
} varBucket;

typedef struct {
    varBucket 	       *buckets;
    unsigned int	size;
} vartable;

typedef set(variable) VarSet;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern variable	  *variable_new(varIdLeaf *id, Sort *s);
extern bool	   variable_equal(variable *v1, variable *v2);
extern varIdLeaf  *variable_id(variable *v);
# define	   variable_id(v)   ((v)->id)
extern Sort	  *variable_sort(variable *v);
# define	   variable_sort(v) ((v)->sort)

extern vartable   *vartable_null(void);
# define	   vartable_null()  ((vartable *)0)
extern vartable	  *vartable_new(void);
extern void	   vartable_free(vartable *vt);
extern bool	   vartable_declare(vartable *vt, varIdLeaf *id, Sort *s);
extern varIdLeaf  *vartable_invent(vartable *vt, char *id, int n, Sort *s, 
				       LSLToken loc);
extern VarSet	  *vartable_matchingVars(vartable *vt, varIdLeaf *id, Sort *s);
extern void	   vartable_merge(vartable *vt1, vartable *vt2, renaming *r);
extern bool	   vartable_overloaded(vartable *vt, varIdLeaf *id);
extern void	   vartable_print2LP(vartable *vt, symtable *st);
extern void	   vartable_recheck(vartable *vt, symtable *st, renaming *r);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
