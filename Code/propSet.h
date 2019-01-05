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
**      FILENAME:   propSet.h
**
**	PURPOSE:    Interface to abstraction for sets of propositions
**
**
**  MODIFICATION HISTORY:
**
**	{0} Garland  at MIT     -- 91.12.26 -- Original
**	{1} Garland  at MIT	-- 92.01.12 -- Add LP translation to print
**	{2} Garland  at MIT	-- 92.02.12 -- Add propSet_free
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

typedef struct _propBucket {
    prop		*elem;	/* pointer to the prop */
    struct _propBucket	*next;	/* pointer to next entry in bucket */
} propBucket;

typedef prop *propList;		/* array of pointers to props */

typedef struct {
    propList    *list;		/* array of pointers to props */
    propBucket	*hashTable;	/* hash table of these props */
    bool	 nontrivial;	/* TRUE if array contains nontrait prop */
    int		 size;		/* number of all props in array */
    int		 alloc;		/* allocated size of array */
} propSet;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern propSet	*propSet_new(void);
extern void	 propSet_free(propSet *ps);
extern bool	 propSet_insert(propSet *ps, prop *p);
extern void	 propSet_extend(propSet *ps1, traitNode *t, int child, 
				 renaming *r, propSet *ps2);
extern void 	 propSet_extendCvt (propSet *ps1, traitNode *t, renaming *r, 
				    propSet *ps2);
extern bool	 propSet_nontrivial(propSet *ps);
# define	 propSet_nontrivial(p) ((p)->nontrivial)
extern void	 propSet_print(propSet *ps, bool assert, renaming *r);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
