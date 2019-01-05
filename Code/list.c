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
**      FILENAME:   list.c
**
**	PURPOSE:    Implementation of parametrized abstract data type for lists
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.09.11 -- Original
**	{1} Garland at MIT     -- 90.10.05 -- Add list_compare, list_size
**	{2} Wild    at Digital -- 91.07.29 -- Include osdfiles.h.
**	{3} Garland at MIT     -- 92.01.01 -- Have list_compare handle null ptr
**	{4} Garland at MIT     -- 92.02.12 -- Add list_free_impl
**	{5} Garland at MIT     -- 93.12.06 -- Add #includes for clean compile
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include "osdfiles.h"
# include STDLIB
# include "general.h"
# include "list.h"


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

extern listRep *list_new_impl();
extern void	list_free_impl();
extern void	list_addh_impl();
extern bool	list_compare_impl();
extern void    *list_copy_impl();
extern void    *list_current_impl();
extern void	list_delete_impl();
extern void	list_pointToNext_impl();
extern void    *list_remh_impl();
extern void    *list_reml_impl();

static link    *newLink();


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

listRep
*list_new_impl(void) {
    listRep *L = (listRep *)LSLMoreMem((void *)0, sizeof(listRep));
    L->first = NULL;
    L->last = NULL;
    L->current = NULL;
    L->size = 0;
    return L;
}



static link
*newLink(void) {
    link *p = (link *)LSLMoreMem((void *)0, sizeof(link));
    p->data = NULL;
    p->next = NULL;
    p->prev = NULL;
    return p;
}


/*
** Ensures: trashed(*L)
*/

extern void
list_free_impl (listRep *L) {
    link *p, *q;
    if (L == (listRep *)0) return;
    for (p = L->first; p != 0; p = q) {
	q = p->next;
	free(p);
    }
    free(L);
}


/*
** Ensures:  *L' == *L + *e
** Modifies: *L
*/

void
list_addh_impl(listRep *L, void *elem) {
    link *p = newLink();
    p->data = elem;
    p->prev = L->last;
    if (L->first == NULL) {
	L->first = p;
	L->current = p;
    } else {
	(L->last)->next = p;
    }
    L->last = p;
    L->size++;
}


/*
** Ensures:  *L' == *e + *L
** Modifies: *L
*/

void
list_addl_impl(listRep *L, void *elem) {
    link *p = newLink();
    p->data = elem;
    p->next = L->first;
    if (L->last == NULL) {
	L->last = p;
	L->current = p;
    } else {
	(L->first)->prev = p;
    }
    L->first = p;
    L->size++;
}


/*
** Ensures: 
**	result == (*L1.size = *L2.size)
**		  & \forall i:Index 
**			i \in indices(*L1) => comparison(L1[i], L2[i])
*/

bool
list_compare_impl (listRep *L1, listRep *L2, compareType *comparison) {
    link *p1;
    link *p2;
    if (L1 == L2) return TRUE;
    if (L1 == (listRep *)0 || L2 == (listRep *)0) return FALSE;
    if (L1->size != L2->size) return FALSE;
    p2 = L2->first;
    for (p1 = L1->first; p1 != NULL;) {
	if (!comparison(p1->data, p2->data)) return FALSE;
	p1 = p1->next;
	p2 = p2->next;
    }
    return TRUE;
}


/*
** Ensures: result == L
*/

void
*list_copy_impl(listRep *L) {
    listRep *c = list_new_impl();
    link *p;
    for (p = L->first; p != 0; p = p->next) list_addh_impl(c, p->data);
    return c;
}


/*
** Ensures: result == *L.current
*/

void 
*list_current_impl(listRep *L) {
    if (L->current == NULL) {
	return NULL;
    } else {
	return (L->current)->data;
    }
}


/*
** Ensures:  *L' == delete(*L, *L.current)
** Modifies: *L
*/

void
list_delete_impl(listRep *L) {
    link *p = L->current;
    if (p == NULL) return;
    if (p->prev == NULL) {
	L->first = p->next;
    } else {
	(p->prev)->next = p->next;
    }
    if (p->next == NULL) {
	L->last = p->prev;
    } else {
	(p->next)->prev = p->prev;
    }
    L->current = p->next;
    L->size--;
}


/*
** Ensures:  *L.current == *L.current.next
** Modifies: *L.current
*/

void 
list_pointToNext_impl(listRep *L) {
    if (L->current != NULL) {
	L->current = (L->current)->next;
    }
}


/*
** Ensures: *L' == remh(*L)
** Modifies: *L
*/

void
*list_remh_impl(listRep *L) {
    link *p = L->last;
    if (p == NULL) {
	return p;
    } else {
	L->last = p->prev;
	if (L->last != NULL) (L->last)->next = NULL;
	if (L->first == p) L->first = NULL;
	if (L->current == p) L->current = NULL;
	L->size--;
	return p->data;
    }
}


/*
** Ensures: *L' == reml(*L)
** Modifies: *L
*/

void
*list_reml_impl(listRep *L) {
    link *p = L->first;
    if (p == NULL) {
	return p;
    } else {
	L->first = p->next;
	if (L->first != NULL) (L->first)->prev = NULL;
	if (L->last == p) L->last = NULL;
	if (L->current == p) L->current = p->next;
	L->size--;
	return p->data;
    }
}
