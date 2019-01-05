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
**      FILENAME:   vartable.c
**
**	PURPOSE:    Variable table abstraction
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.10.05 -- Original
**	{1} Wild    at Digital -- 90.11.09 -- Include osdfiles.h
**	{2} Garland at MIT     -- 91.12.20 -- Add invent, merge, print2LP
**	{3} Garland at MIT     -- 92.01.08 -- Recode as hash table, implement
**					      overloaded id detection
**	{4} Garland at MIT     -- 92.02.12 -- Add vartable_free
**	{5} Garland at MIT     -- 92.02.23 -- Add vartable_recheck
**	{6} Garland at MIT     -- 92.03.22 -- Weaken vartable_recheck pending 
**					      better fix
**	{7} Garland at MIT     -- 92.06.14 -- Use unparse_sort
**	{8} Garland at MIT     -- 93.11.11 -- Accumulate variables in print2LP
**	{9} Garland at MIT     -- 93.12.06 -- Add #include for clean compile
**      {n} Who     at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include	"general.h"
# include	"osdfiles.h"
# include	STDLIB
# include	"string2sym.h"
# include	"token.h"
# include	"error.h"
# include	"tokentable.h"
# include	"parse.h"
# include	"list.h"
# include	"set.h"
# include	"abstract.h"
# include	"operator.h"
# include	"renaming.h"
# include	"symtable.h"
# include	"vartable.h"
# include	"recheck.h"
# include	"unparse.h"
# include	"print.h"

/*
**
**  MACRO DEFINITIONS
**
*/

# define HTSIZE		97
# define VARPREFIX	"_"
# define VARSUFFIX	"_"

# define badTok error_reportBadToken

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

extern vartable		*vartable_new();
extern void	   	 vartable_free();
extern bool	 	 vartable_declare();
extern varIdLeaf	*vartable_invent();
extern VarSet		*vartable_matchingVars();
extern void		 vartable_merge();
extern bool		 vartable_overloaded();

extern variable		*variable_new();
extern bool		 variable_equal();
extern void	   	 vartable_print2LP();
extern void	   	 vartable_recheck();

static unsigned int	 vhash(LSLToken *t);


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


extern variable
*variable_new (varIdLeaf *id, Sort *s) {
    variable *v = (variable *)LSLMoreMem((variable *)0, sizeof(variable));
    v->id = id;
    v->sort = s;
    return v;
}


extern bool
variable_equal (variable *v1, variable *v2) {
    return (LSLSameLeaf(v1->id, v2->id) && sameSort(v1->sort, v2->sort));
}


extern vartable
*vartable_new (void) {
    int       i;
    vartable *vt = (vartable *)LSLMoreMem((void *)0, sizeof(vartable));
    vt->size = 0;
    vt->buckets = (varBucket *)LSLMoreMem((void *)0, HTSIZE*sizeof(varBucket));
    for (i = 0; i < HTSIZE; i++) {
	varBucket *b;
	b = &vt->buckets[i];
        b->var = (variable *)0;
	b->next = (varBucket *)0;
    }
    return vt;
}


extern void
vartable_free (vartable *vt) {
    int i;
    if (vt == (vartable *)0) return;
    for (i = 0; i < HTSIZE; i++) {
	varBucket *b, *next;
	for (b = vt->buckets[i].next; b != (varBucket *)0; b = next) {
	    next = b->next;
	    free(b);
	}
    }
    free(vt->buckets);
    free(vt);
}




extern bool
vartable_declare (vartable *vt, varIdLeaf *id, Sort *s) {
    varBucket *b          = &vt->buckets[vhash(id)];
    bool       overloaded = FALSE;
    if (b->var != (variable *)0) {
	for (;; b = b->next) {
	    if (LSLSameLeaf(id, b->var->id)) {
		if (sameSort(s, b->var->sort)) return FALSE;
		b->overloaded = TRUE;
		overloaded = TRUE;
	    }
	    if (b->next == (varBucket *)0) break;
	}
	b->next = (varBucket *)LSLMoreMem((void *)0, sizeof(varBucket));
	b = b->next;
    }
    b->var = variable_new(id, s);
    b->overloaded = overloaded;
    b->next = (varBucket *)0;
    vt->size++;
    return TRUE;
}


extern varIdLeaf
*vartable_invent (vartable *vt, char *id, int n, Sort *s, LSLToken loc) {
    LSLToken  *varToken = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
    char       buf[10];
    sprintf(buf, "%d", n);
    id = string_paste(VARPREFIX, 
		      string_paste(id, string_paste(buf, VARSUFFIX)));
    *varToken = LSLInsertToken(SIMPLEID, LSLString2Symbol(id), 0, FALSE);
    token_setFileName(*varToken, token_fileName(loc));
    token_setLine(*varToken, token_line(loc));
    token_setCol(*varToken, token_col(loc));
    vartable_declare(vt, varToken, s);
    return varToken;
}


extern VarSet
*vartable_matchingVars (vartable *vt, varIdLeaf *id, Sort *s) {
    VarSet    *vs = set_new(variable, variable_equal);
    varBucket *b  = &vt->buckets[vhash(id)];
    if (b->var == (variable *)0) return vs;
    for (; b != (varBucket *)0; b = b->next) {
	if (LSLSameLeaf(id, b->var->id) && 
	    (s == (Sort *)0 || sameSort(s, b->var->sort)))
	    set_insert(variable, vs, b->var);
    }
    return vs;
}


extern void
vartable_merge (vartable *vt1, vartable *vt2, renaming *r) {
    int i;
    varBucket *b;
    for (i = 0; i < HTSIZE; i++) {
	b = &vt2->buckets[i];
	if (b->var == (variable *)0) continue;
	for (; b != (varBucket *)0; b = b->next) {
	    /* ENHANCE: put "b->var" directly in "vt->buckets[i]" */
	    vartable_declare(vt1, b->var->id, 
			     renaming_mapSort(r, b->var->sort));
	}
    }
}


extern bool
vartable_overloaded (vartable *vt, varIdLeaf *id) {
    varBucket *b  = &vt->buckets[vhash(id)];
    if (b->var == (variable *)0) return FALSE;
    for (; b != (varBucket *)0; b = b->next) {
	if (LSLSameLeaf(id, b->var->id)) return b->overloaded;
    }
    return FALSE;
}


/* 
** Prints declarations for the variables in "*vt".  Also marks those variables
** that are overloaded by constants in "*st" (and hence must be qualified when 
** they occur in terms).
*/

extern void
vartable_print2LP(vartable *vt, symtable *st) {
    int i;
    varBucket *b;
    bool printed;
    unparse_addVariables(vt);
    if (vt->size == 0) return;
    print_str("declare variables");
    print_newline(1);
    print_setIndent(2);
    printed = FALSE;
    for (i = 0; i < HTSIZE; i++) {
	b = &vt->buckets[i];
	if (b->var == (variable *)0) continue;
	for (; b != (varBucket *)0; b = b->next) {
	    b->overloaded = (symtable_markOverloadedConstants(st, b->var->id) 
			     || b->overloaded);      
	    unparse_token(b->var->id);
	    print_str(": ");
	    unparse_sort(b->var->sort);
	    print_newline(1);
	}
    }
    print_str("..");
    print_newline(2);
    print_setIndent(0);
}


extern void
vartable_recheck (vartable *vt, symtable *st, renaming *r) {
    int 	i;
    varBucket  *b, *b1;
    bool 	rename = renaming_nSorts(r) > 0;
    for (i = 0; i < HTSIZE; i++) {
	b = &vt->buckets[i];
	if (b->var == (variable *)0) continue;
	for (; b != (varBucket *)0; b = b->next) {
	    varIdLeaf *v;
	    Sort      *s;
	    v = b->var->id;
	    s = b->var->sort;
	    if (rename) s = renaming_mapSort(r, s);
	    if (symtable_legalVar(st, v, s)) {
		if (rename)
		    for (b1 = b->next; b1 != (varBucket *)0; b1 = b1->next)
			if (LSLSameLeaf(v, b1->var->id) &&
			    sameSort(s, renaming_mapSort(r, b1->var->sort))) {
			    recheck_sortError();
			    badTok(v, "duplicate variable in declaration");
			    break;
			}
	    } else {
		/* IMPLEMENT: Note that "v" must be renamed when generating */
		/* output for LP.					    */
	    }
	}
    }
}

static unsigned int
vhash (LSLToken *t) {
    return token_textSym(*t) % HTSIZE;
}
