/*
**
**	Copyright (c) MIT, 1990, 1991, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   unparse.h
**
**	PURPOSE:    Interface to unparsing routines for abstract syntax
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT     -- 90.09.14 -- Original
**      {1} Wild    at Digital -- 90.12.10 -- Add unparse_indent.
**      {2} Garland at MIT     -- 91.01.26 -- Add unparse_sort (later removed).
**	{3} Wild    at Digital -- 91.02.20 -- Merge in MIT changes.
**	{4} Garland at MIT     -- 91.12.20 -- Expose more for lsl2lp
**	{5} Garland at MIT     -- 91.12.20 -- Add unparse_sort (again)
**	{6} Garland at MIT     -- 93.11.11 -- Make symbols accumulate
**      {n} Who     at Where   -- yy.mm.dd -- What
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

extern void unparse_setForLP(bool lp);
extern void unparse_resetSymbols(void);
extern void unparse_resetVariables(void);
extern void unparse_addSymbols(symtable *st);
extern void unparse_addVariables(vartable *vt);

extern void unparse_trait(traitNode *n);
extern void unparse_name(nameNode *n);
extern void unparse_signature(signatureNode *n);
extern void unparse_sort(sortNode *n);
extern void unparse_genBy(genByNode *n);
extern void unparse_partBy(partByNode *n);
extern void unparse_equation(equationNode *n);
extern void unparse_term(termNode *n);
extern void unparse_conversion(conversionNode *n);
extern void unparse_token(LSLToken *tok);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
