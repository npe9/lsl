/*
**
**	Copyright (c) MIT, 1990, 1992
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   list.h
**
**	PURPOSE:    Interface to parametrized abstract data type for lists
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.09.11 -- Original
**	{1} Garland at MIT   -- 90.10.05 -- Add list_compare, list_size
**	{3} Garland at MIT   -- 92.02.12 -- Add list_free
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define list(t)		listRep
# define list_arg_e(t, x)	t    *__e 	    	  = (x)
# define list_arg_l(t, pL)	listRep *__l 	    	  = (pL)
# define list_arg_l1(t, pL)	listRep *__l1 	    	  = (pL)
# define list_arg_f2(t, pF)	bool (*__f)(t *_a, t *_b) = pF

# ifndef NULL
# define NULL 0
# endif


/*
**
**  TYPEDEFS
**
*/

typedef struct _link {
    void		*data;
    struct _link	*next;
    struct _link	*prev;
} link;

typedef struct {
    link		*first;
    link		*last;
    link		*current;
    unsigned int	size;
} listRep;

typedef bool compareType(); 	/* Two arguments of type t */


/*
**
**  FUNCTION PROTOTYPES
**
*/

/* 
** Ensures: new(*result)
*/

# define list_new(t)			\
    (list(t) *)list_new_impl()
extern listRep 	
*list_new_impl(void);


/*
** Ensures: trashed(*L)
*/

# define list_free(t, pL)		\
    {list_arg_l(t, pL);			\
    list_free_impl(__l);}
extern void
list_free_impl(listRep *L);


/*
** Ensures:  *L' == *L + *e
** Modifies: *L
*/

# define list_addh(t, pL, x)		\
    {list_arg_l(t, pL);			\
     list_arg_e(t, x);			\
     list_addh_impl(__l, __e);}
extern void
list_addh_impl(listRep *L, void *e);


/*
** Ensures:  *L' == *e + *L
** Modifies: *L
*/

# define list_addl(t, pL, x)		\
    {list_arg_l(t, pL);			\
     list_arg_e(t, x);			\
     list_addl_impl(__l, __e);}
extern void
list_addl_impl(listRep *L, void *e);



/*
** Ensures: 
**	result == (*L1.size = *L2.size)
**		  & \forall i:Index 
**			i \in indices(*L1) => comparison(L1[i], L2[i])
**	
*/

# ifdef __GNUC__
# define list_compare(t, pL, pL1, pF)	\
    ({list_arg_l(t, pL);		\
      list_arg_l1(t, pL1);		\
      list_arg_f2(t, pF);		\
      list_compare_impl(__l, __l1, __f);\
  })
# else
# define list_compare(t, pL, pL1, pF)	\
      list_compare_impl(pL, pL1, pF)
# endif
extern bool
list_compare_impl(listRep *L1, listRep *L2, compareType *comparison);


/*
** Ensures: result == l
*/

# ifdef __GNUC__
# define list_copy(t, pL)		\
    ({list_arg_l(t, pL);		\
      (list(t) *)list_copy_impl(__l);})
# else
# define list_copy(t, pL)		\
      (list(t) *)list_copy_impl(pL)
# endif
extern void
*list_copy_impl(listRep *L);


/*
** Ensures: result == *L.current
*/

# ifdef __GNUC__
# define list_current(t, pL)		\
    ({list_arg_l(t, pL);		\
      (t *)list_current_impl(__l);})
# else
# define list_current(t, pL)		\
      (t *)list_current_impl(pL)
# endif
extern void
*list_current_impl(listRep *L);


/*
** Ensures:  *L' == delete(*L, *L.current)
** Modifies: *L
*/

# define list_delete(t, pL)		\
    {list_arg_l(t, pL);			\
     list_delete_impl(__l);}
extern void
list_delete_impl(listRep *L);


/*
** Ensures: result == *L.current \neq nil
*/

# ifdef __GNUC__
# define list_more(t, pL)		\
    ({list_arg_l(t, pL);		\
      (__l->current != NULL);})
# else
# define list_more(t, pL)		\
      ((pL)->current != NULL)
# endif


/*
** Ensures:  *L.current == *L.first
** Modifies: *L.current
*/

# if __GNUC__
# define list_pointToFirst(t, pL)	\
    ({list_arg_l(t, pL);		\
      __l->current = __l->first;})
# else
# define list_pointToFirst(t, pL)	\
     (pL)->current = (pL)->first
# endif


/*
** Ensures:  *L.current == *L.last
** Modifies: *L.current
*/

# if __GNUC__
# define list_pointToLast(t, pL)	\
    ({list_arg_l(t, pL);		\
      __l->current = __l->last;})
# else
# define list_pointToLast(t, pL)	\
     (pL)->current = (pL)->last
# endif


/*
** Ensures:  *L.current == *L.current.next
** Modifies: *L.current
*/

# define list_pointToNext(t, pL)	\
    {list_arg_l(t, pL);			\
     list_pointToNext_impl(__l);}
extern void
list_pointToNext_impl(listRep *L);


/*
** Ensures: *L' == remh(*L)
** Modifies: *L
*/

# define list_remh(t, pL)	    	\
    {list_arg_l(t, pL);			\
     (t *)list_remh_impl(__l);}
extern void
*list_remh_impl(listRep *L);


/*
** Ensures: *L' == reml(*L)
** Modifies: *L
*/

# define list_reml(t, pL)	    	\
    {list_arg_l(t, pL);			\
     (t *)list_reml_impl(__l);}
extern void
*list_reml_impl(listRep *L);


/*
** Ensures: ???
*/

# ifdef __GNUC__
# define list_traverse(t, pL, e)	\
    {list_arg_l(t, pL);			\
     link *__p;				\
     for (__p = __l->first;		\
	  __p != NULL;			\
	  __p = __p->next) {		\
       t *e;				\
       e = __p->data;
# else
# define list_traverse(t, pL, e)	\
    {link *__p;				\
     for (__p = pL->first;		\
	  __p != NULL;			\
	  __p = __p->next) {		\
       t *e;				\
       e = __p->data;
# endif


# define end_traverse }}


/*
** Ensures: result == size(*L)
*/

# ifdef __GNUC__
# define list_size(t, pL)		\
    ({list_arg_l(t, pL);		\
      ((__l)->size);})
# else
# define list_size(t, pL)		\
      ((pL)->size)
# endif


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
