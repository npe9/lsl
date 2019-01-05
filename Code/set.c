/*
**
**	Copyright (c) MIT, 1990, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   set.c
**
**	PURPOSE:    Implementation of parametrized abstract data type for sets
**
**	     This implementation represents a set as a linked list of elements.
**	It does not assume that the element data type can be linearly ordered. 
**	Hence operations such as union and intersection take quadratic time.
**
**           A more efficient implementation is possible if the elements can be
**	ordered.  The "new" procedure could take a second function parameter, 
**	which defines an ordering on the element data type.  The "union" and 
**	"intersection" procedures could then sort the lists of elements in the 
**	two sets, after which the operations could be performed in linear time.
**
**	     Still another more efficient implementation would represent a set
**	as an ordered binary tree.
**      
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.09.28 -- original
**	{1} Garland at MIT   -- 92.02.12 -- Add set_free_impl
**	{2} Garland at MIT   -- 93.12.06 -- Add #include for clean compile
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include	"osdfiles.h"
# include	STDLIB
# include	"general.h"
# include	"list.h"
# include	"set.h"


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

extern setRep 	*set_new_impl();
extern void	 set_free_impl();
extern void	 set_applyToAll_impl();
extern void	*set_choose_impl();
extern void	*set_chooseSatisfying_impl();
extern setRep	*set_copy_impl();
extern bool	 set_insert_impl();
extern setRep	*set_intersect_impl();
extern bool	 set_is_element_impl();
extern bool 	 set_is_subset_impl();
extern bool	 set_remove_impl();
extern setRep	*set_subset_impl();
extern setRep	*set_union_impl();

static void	 insert_unique(setRep *s, void *e);
# define	 insert_unique(pS, pE)				\
			{list_addh(void, (pS)->elems, pE);	\
			 ((pS)->size)++;}


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
** Ensures: new(*result)
*/

setRep
*set_new_impl (compareType *similar) {
    setRep *s = (setRep *)LSLMoreMem((void *)0, sizeof(setRep));
    s->elems = list_new(void);
    s->size = 0;
    s->similar = similar;
    return s;
}


/*
** Ensures: trashed(*s)
*/

extern void
set_free_impl (setRep *s) {
    if (s == (setRep *)0) return;
    list_free(void, s->elems);
    free(s);
}


/*
** Ensures: f applied to all members of *s
*/

void	 
set_applyToAll_impl(setRep *s, examineType *f) {
    list_traverse(void, s->elems, e) {
	(*f)(e);
    } end_traverse;
}


/*
** Ensures: if *s = {} then result = NULL else *result \in *s
*/

void
*set_choose_impl (setRep *s) {
    if (s->elems->first == 0) return 0;
    return s->elems->first->data;
}

 
/*
** Ensures:  (*result \in *s & examine(*result)) 
**		| (result = NULL & (\forall x: t)(x \in *s => ~examine(x)))
*/

void
*set_chooseSatisfying_impl (setRep *s, examineType *examine) {
    list_traverse(void, s->elems, e) {
	if ((*examine)(e)) return e;
    } end_traverse;
    return 0;
}

 
/*
** Ensures: new(*result) & *result = *s
*/

setRep
*set_copy_impl (setRep *s) {
    setRep *c = set_new_impl(s->similar);
    list_traverse(void, s->elems, e) {
	list_addh(void, c->elems, e);
    } end_traverse;
    c->size = s->size;
    return c;
}


/*
** Ensures: if *e \in *s 
**          then unchanged(*s) & result = false
**          else *s' = insert(*s, *e) & result = true
** Modifies: *s
*/

bool
set_insert_impl (setRep *s, void *e) {
    if (set_is_element_impl(e, s)) return FALSE;
    insert_unique(s, e);
    return TRUE;
}


/*
** Ensures:  new(*result) & *result = *s1 \intersect *s2
*/

setRep
*set_intersect_impl(setRep *s1, setRep *s2) {
    setRep *c = set_new_impl(s1->similar);
    list_traverse(void, s1->elems, e) {
	if (set_is_element_impl(e, s2)) {
	    insert_unique(c, e);
	}
    } end_traverse;
    return c;
}


/* 
** Ensures: result == *e \in *s
*/

bool	 
set_is_element_impl(void *e, setRep *s) {
    list_traverse(void, s->elems, e1) {
	if ((*(s->similar))(e, e1)) return TRUE;
    } end_traverse;
    return FALSE;
}


/*
** Ensures: result == *s1 \subseteq *s2
*/

bool 	 
set_is_subset_impl(setRep *s1, setRep *s2) {
    list_traverse(void, s1->elems, e) {
	if (!set_is_element_impl(e, s2)) return FALSE;
    } end_traverse;
    return TRUE;
}


/*
** Ensures: x \in *result == x \in *s & f(x)
*/

setRep
*set_subset_impl(setRep *s, examineType *f) {
    setRep *s1 = set_new_impl(s->similar);
    list_traverse(void, s->elems, e) {
	if ((*f)(e)) insert_unique(s1, e);
    } end_traverse;
    return s1;
}


/*
** Ensures:  new(*result) & *result = *s1 \union *s2
*/

setRep	 
*set_union_impl(setRep *s1, setRep *s2) {
    setRep *c = set_copy_impl(s1);
    list_traverse(void, s2->elems, e) {
	set_insert_impl(c, e);
    } end_traverse;
    return c;
}
