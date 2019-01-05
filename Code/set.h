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
**      FILENAME:   set.h
**
**	PURPOSE:    Interface to parametrized abstract data type for sets
**
**	     It would be nice if we could strengthen the type checking for this
**	abstraction in two ways: to distinguish set(t) from set(t'), and to
**	ensure (at compile time) that two set arguments to a procedure both use
**	the same underlying notion of element equality.
**
**	USAGE:	# include "general.h"
**		# include "list.h"
**		# include "set.h"
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.09.28 -- original
**	{1} Garland at MIT   -- 92.02.12 -- Add set_free
**	{2} Garland at MIT   -- 93.12.06 -- Use __s in set_traverse
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define set(t)			setRep
# define set_arg_e(t, x)	t    *__e 	    	  = (x)
# define set_arg_s(t, pS)	setRep *__s 	    	  = (pS)
# define set_arg_s1(t, pS)	setRep *__s1 	    	  = (pS)
# define set_arg_f1(t, pF)	bool (*__f)(t *_a)  	  = pF
# define set_arg_f2(t, pF)	bool (*__f)(t *_a, t *_b) = pF

# ifndef NULL
# define NULL 0
# endif


/*
**
**  TYPEDEFS
**
*/

typedef bool examineType();	/* One argument of type t  */

typedef struct {
    listRep 	*elems;
    int		size;
    compareType	*similar;
} setRep;


/*
**
**  FUNCTION PROTOTYPES
**
*/

/* 
** Ensures: new(*result)
** Note:    *similar will be used as the notion of element equality on all
**	    subsequent operations involving *result
*/

# ifdef __GNUC__
# define set_new(t, pF) 			\
    ({set_arg_f2(t, pF); 			\
      (set(t) *)set_new_impl(__f);})
# else
# define set_new(t, pF) 			\
      (set(t) *)set_new_impl(pF)
# endif
extern setRep
*set_new_impl(compareType *similar);


/*
** Ensures: trashed(*s)
*/

# define set_free(t, pS)			\
    {set_arg_s(t, pS);				\
     set_free_impl(__s);}
extern void
set_free_impl (setRep *s);



/*
** Ensures: f applied to all members of *s
** Note:    We need to find a better way to specify this procedure.
*/

# define set_applyToAll(t, pS, pF) 		\
    {set_arg_s(t, pS); 				\
     set_arg_f1(t, pF); 			\
     set_applyToAll_impl(__s, __f);}
extern void
set_applyToAll_impl(setRep *s, examineType *f);


/*
** Ensures:  if *s = {} then result = NULL else *result \in *s
*/

# ifdef __GNUC__
# define set_choose(t, pS) 			\
    ({set_arg_s(t, pS); 			\
      (t *)set_choose_impl(__s);})
# else
# define set_choose(t, pS) 			\
      (t *)set_choose_impl(pS)
# endif
extern void
*set_choose_impl(setRep *s);


/*
** Ensures:
*/

# ifdef __GNUC__
# define set_current(t, pS)			\
    ({set_arg_s(t, pS);				\
      list_current(t, __s->elems);})
# else
# define set_current(t, pS)			\
      list_current(t, (pS)->elems)
# endif


/*
** Ensures:  (*result \in *s & examine(*result)) 
**		| (result = NULL & (\forall x: t)(x \in *s => ~examine(x)))
*/

# ifdef __GNUC__
# define set_chooseSatisfying(t, pS, pF) 	\
    ({set_arg_s(t, pS); 			\
      set_arg_f1(t, pF);			\
      (t *)set_chooseSatisfying_impl(__s, __f);})
# else
# define set_chooseSatisfying(t, pS, pF)	\
      (t *)set_chooseSatisfying_impl(pS, pF)
# endif
extern void
*set_chooseSatisfying_impl(setRep *s, examineType *examine);


/*
** Ensures:  new(*result) & *result = *s
*/

# ifdef __GNUC__
# define set_copy(t, pS) 			\
    ({set_arg_s(t, pS); 			\
      (set(t) *)set_copy_impl(__s);})
# else
# define set_copy(t, pS) 			\
      (set(t) *)set_copy_impl(pS)
# endif
extern setRep
*set_copy_impl(setRep *s);


/*
** Ensures: if *e \in *s 
**          then unchanged(*s) & result = false
**          else *s' = insert(*s, *e) & result = true
** Modifies: *s
*/

# ifdef __GNUC__
# define set_insert(t, pS, x) 			\
    ({set_arg_s(t, pS); 			\
      set_arg_e(t, x); 				\
      set_insert_impl(__s, (void *)__e);})
# else
# define set_insert(t, pS, x) 			\
      set_insert_impl(pS, x)
# endif
extern bool
set_insert_impl(setRep *s, void *e);


/*
** Ensures:  new(*result) & *result = *s1 \intersect *s2
** Note:     We should check that *s1->similar == *s2->similar.
*/

# ifdef __GNUC__
# define set_intersect(t, pS1, pS2) 		\
    ({set_arg_s(t, pS1); 			\
      set_arg_s1(t, pS2); 			\
      (set(t) *)set_intersect_impl(__s, __s1);})
# else
# define set_intersect(t, pS1, pS2) 		\
      (set(t) *)set_intersect_impl(pS1, pS2)
# endif
extern setRep
*set_intersect_impl(setRep *s1, setRep *s2);


/* 
** Ensures: result == *e \in *s
*/

# ifdef __GNUC__
# define set_is_element(t, x, pS) 		\
    ({set_arg_e(t, x); 				\
      set_arg_s(t, pS); 			\
      set_is_element_impl((void *)__e, __s);})
# else
# define set_is_element(t, x, pS) 		\
      set_is_element_impl((void *)x, pS)
# endif
extern bool
set_is_element_impl(void *e, setRep *s);


/*
** Ensures:  result == *s1 \subseteq *s2
** Note:     We should check that *s1->similar == *s2->similar.
*/

# ifdef __GNUC__
# define set_is_subset(t, pS1, pS2) 		\
    ({set_arg_s(t, pS1); 			\
      set_arg_s1(t, pS2); 			\
      set_is_subset_impl(__s, __s1);})
# else
# define set_is_subset(t, pS1, pS2) 		\
      set_is_subset_impl(pS1, pS2)
# endif
extern bool
set_is_subset_impl(setRep *s1, setRep *s2);


/*
** Ensures:  result == size(*s1)
*/

# ifdef __GNUC__
# define set_size(t, pS) 			\
    ({set_arg_s(t, pS); 			\
      ((__s)->size);})
# else
# define set_size(t, pS) 			\
      ((pS)->size)
# endif


/*
** Ensures:  \forall x:E  x \in *result == x \in *s & f(x)
** Note:     f(x) in the ensures clause is a shorthand for the postcondition
**	     in the interface specification for f.
*/

# ifdef __GNUC__
# define set_subset(t, pS, pF) 			\
    ({set_arg_s(t, pS); 			\
      set_arg_f2(t, pF); 		       	\
      (set(t) *)set_subset_impl(__s, __f);})
# else
# define set_subset(t, pS, pF) 			\
      (set(t) *)set_subset_impl(pS, pF)
# endif
extern setRep
*set_subset_impl(setRep *s, examineType *f);


/*
** Ensures: ???
*/

# ifdef __GNUC__
# define set_traverse(t, pS, e)		\
    {set_arg_s(t, pS); __s = __s;};		\
     list_traverse(t, (pS)->elems, e)
# else
# define set_traverse(t, pS, e)		\
    list_traverse(t, (pS)->elems, e)
# endif


/*
** Ensures:  new(*result) & *result = *s1 \union *s2
** Note:     We should check that *s1->similar == *s2->similar.
*/

# ifdef __GNUC__
# define set_union(t, pS1, pS2) 		\
    ({set_arg_s(t, pS1); 			\
      set_arg_s1(t, pS2); 			\
      (set(t) *)set_union_impl(__s, __s1);})
# else
# define set_union(t, pS1, pS2) 		\
     (set(t) *)set_union_impl(pS1, pS2)
# endif
extern setRep
*set_union_impl(setRep *s1, setRep *s2);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
