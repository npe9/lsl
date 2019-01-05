/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993, 1994
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   checking.h
**
**	PURPOSE:    Interface to context-sensitive checking
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.09.27 -- Original
**	{1} Garland at MIT   -- 91.12.20 -- Define, expose lsl2lp attributes,
**					    change external interface
**      {2} Garland at MIT   -- 92.01.24 -- Expose adjustMarkers
**      {3} Garland at MIT   -- 92.02.16 -- Add DecimalLiterals
**      {4} Garland at MIT   -- 92.06.14 -- Composite sorts
**	{5} Garland at MIT   -- 93.01.07 -- Add traitAttr_lemmas
**	{6} Garland at MIT   -- 93.11.10 -- Add quantified terms
**	{7} Garland at MIT   -- 94.04.20 -- Add "generated freely"
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define DecLitsNAME "SpecialDecimalLiteralsTrait"


/*
**
**  TYPEDEFS
**
*/


/* Attributes for nodes in abstract syntax tree */

typedef struct _enumerationAttr {
    OpList	*elems;
    operator	*succ;
} enumerationAttr;

typedef struct _exemptionAttr {
    vartable	*vars;
} exemptionAttr;

typedef struct _parameterAttr {
    parameterKind	 kind;
    union {
	Sort		*sort;
	operator	*op;
    } choice;
} parameterAttr;

typedef struct _genByAttr {
    OpList	*ops;
} genByAttr;

typedef struct _partByAttr {
    OpList	*ops;
} partByAttr;

typedef struct _operatorAttr {
    operator	*op;
} operatorAttr;

typedef struct _quantEqnsAttr {
    vartable	*vars;
} quantEqnsAttr;

typedef struct _replaceAttr {
    parameterKind	 kind;
    union {
	Sort		*sort;
	operator	*op;
    } choice;
} replaceAttr;

typedef enum {OPTERM, VARTERM, ZEROARYTERM, BADTERM, QUANTIFIEDTERM} termKind;

typedef struct _termAttr {
    termKind	 kind;
    OpSet	*possibleOps;
    VarSet 	*possibleVars;
    Sort	*sort;
} termAttr;

typedef enum {CHECKING, CORRECT, DANGLE, INCORRECT, UNCHECKED} checkingStatus;

typedef struct _traitAttr {
    bool		 parseError;
    checkingStatus	 axiomsCorrect;
    checkingStatus	 consequencesCorrect;
    symtable		*symtab;
    vartable		*assertVars;
    vartable		*assumeVars;
    vartable		*checkVars;
    vartable		*conseqVars;
    vartable		*lemmaVars;
    propSet		*assertions;
    propSet		*assumptions;
    propSet		*checkAssumes;		/* Inherited assumptions */
    propSet		*consequences;
    propSet		*lemmas;
} traitAttr;

typedef struct _traitRefAttr {
    traitNodeList	*traits;
    renaming		*rename;
} traitRefAttr;

typedef struct _tupleAttr {
    OpList	*gets;
    OpList	*sets;
    operator	*make;
} tupleAttr;

typedef struct _unionAttr {
    OpList		*toS;
    OpList		*fromS;
    operator		*tag;
    Sort		*tagS;
    enumerationNode 	*labels;
} unionAttr;



/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void		 attributeVarTerm(termNode *n, Sort *s);
extern void		 attributeOpTerm(termNode *n, operator *op);
extern void		 attributeQuantifiedTerm(termNode *n);
extern genByNode	*buildGenByNode(Sort *s, bool freely, OpList *ops, 
					LSLToken loc);
extern partByNode	*buildPartByNode(Sort *s, OpList *ops, LSLToken loc);
extern operator 	*parameter_operator(parameterNode *n);
extern operator 	*operator_declaration(operatorNode *n);
extern Sort     	*term_sort(termNode *n);
extern symtable 	*trait_symtable(traitNode *n);

extern nameNode	        *adjustMarkers(nameNode *name, operator *op);

/*
** Private: allTraits
** Ensures: 	
**   	*toCheck \in allTraits
**	&  \forall s, t: TraitNode
**		((t \in allTraits' & s \in traitRefs(t)) => s \in allTraits
**		 & s->attr->axiomsCorrect \in {CORRECT, INCORRECT}
**		 & s->attr->conseqCorrect \in {CORRECT, INCORRECT})
**	) |
**	(result = FALSE & "missing trait or trait cycle")
*/

extern traitNode       *parseAndCheckTrait(traitIdLeaf *id, 
					   traitNodeList *allTraits);

extern OpList		*enumerationAttr_elems(enumerationNode *n);
# define		 enumerationAttr_elems(pn)			\
    			 	((pn)->attr->elems)

extern operator		*enumerationAttr_succ(enumerationNode *n);
# define		 enumerationAttr_succ(pn)			\
    			 	((pn)->attr->succ)

extern vartable		*exemptionAttr_vars(exemptionNode *n);
# define		 exemptionAttr_vars(pn)				\
    				((pn)->attr->vars)

extern parameterKind	 parameterAttr_kind(parameterNode *n);
# define		 parameterAttr_kind(pn)				\
    				((pn)->attr->kind)

extern operator		*parameterAttr_op(parameterNode *n);
# define		 parameterAttr_op(pn)				\
    				((pn)->attr->choice.op)

extern Sort		*parameterAttr_sort(parameterNode *n);
# define		 parameterAttr_sort(pn)				\
    				((pn)->attr->choice.sort)

extern OpList		*genByAttr_ops(genByNode *n);
# define		 genByAttr_ops(pn)				\
    				((pn)->attr->ops)

extern OpList		*partByAttr_ops(partByNode *n);
# define		 partByAttr_ops(pn)				\
    				((pn)->attr->ops)

extern operator		*operatorAttr_op(operatorNode *n);
# define		 operatorAttr_op(pn)				\
    				((pn)->attr->op)

extern vartable		*quantEqnsAttr_vars(quantEqnsNode *n);
# define		 quantEqnsAttr_vars(pn)				\
    				((pn)->attr->vars)

extern parameterKind	 replaceAttr_kind(replaceNode *n);
# define		 replaceAttr_kind(pn)				\
    				((pn)->attr->kind)

extern operator		*replaceAttr_op(replaceNode *n);
# define		 replaceAttr_op(pn)				\
    				((pn)->attr->choice.op)

extern Sort		*replaceAttr_sort(replaceNode *n);
# define		 replaceAttr_sort(pn)				\
    				((pn)->attr->choice.sort)

extern termKind		 termAttr_kind(termNode *n);
# define		 termAttr_kind(pn)				\
    				((pn)->attr->kind)
extern OpSet		*termAttr_possibleOps(termNode *n);
# define		 termAttr_possibleOps(pn)			\
    				((pn)->attr->possibleOps)
extern VarSet		*termAttr_possibleVars(termNode *n);
# define		 termAttr_possibleVars(pn)			\
    				((pn)->attr->possibleVars)
extern Sort		*termAttr_sort(termNode *n);
# define		 termAttr_sort(pn)				\
    				((pn)->attr->sort)

extern bool		 traitAttr_parseError(traitNode *n);
# define		 traitAttr_parseError(pn)			\
    			   ((pn)->attr->parseError)
extern bool		 traitAttr_axiomsChecked(traitNode *n);
# define		 traitAttr_axiomsChecked(pn)			\
    			   ((traitAttr_axiomsCorrect(pn) == CORRECT) || \
    			    (traitAttr_axiomsCorrect(pn) == INCORRECT))
extern checkingStatus	 traitAttr_axiomsCorrect(traitNode *n);
# define		 traitAttr_axiomsCorrect(pn)			\
    				((pn)->attr->axiomsCorrect)
extern checkingStatus	 traitAttr_consequencesCorrect(traitNode *n);
# define		 traitAttr_consequencesCorrect(pn)		\
    				((pn)->attr->consequencesCorrect)
extern struct _symtable	*traitAttr_symtab(traitNode *n);
# define		 traitAttr_symtab(pn)				\
    				((pn)->attr->symtab)
extern struct _vartable *traitAttr_assertVars(traitNode *n);
# define		 traitAttr_assertVars(pn)			\
    				((pn)->attr->assertVars)
extern struct _vartable *traitAttr_assumeVars(traitNode *n);
# define		 traitAttr_assumeVars(pn)			\
    				((pn)->attr->assumeVars)
extern struct _vartable *traitAttr_checkVars(traitNode *n);
# define		 traitAttr_checkVars(pn)			\
    				((pn)->attr->checkVars)
extern struct _vartable *traitAttr_conseqVars(traitNode *n);
# define		 traitAttr_conseqVars(pn)			\
    				((pn)->attr->conseqVars)
extern struct _vartable *traitAttr_lemmaVars(traitNode *n);
# define		 traitAttr_lemmaVars(pn)			\
    				((pn)->attr->lemmaVars)
extern propSet		*traitAttr_assertions(traitNode *n);
# define		 traitAttr_assertions(pn)			\
    				((pn)->attr->assertions)
extern propSet		*traitAttr_assumptions(traitNode *n);
# define		 traitAttr_assumptions(pn)			\
    				((pn)->attr->assumptions)
extern propSet		*traitAttr_checkAssumes(traitNode *n);
# define		 traitAttr_checkAssumes(pn)			\
    				((pn)->attr->checkAssumes)
extern propSet		*traitAttr_consequences(traitNode *n);
# define		 traitAttr_consequences(pn)			\
    				((pn)->attr->consequences)
extern propSet		*traitAttr_lemmas(traitNode *n);
# define		 traitAttr_lemmas(pn)				\
    				((pn)->attr->lemmas)


extern traitNodeList	*traitRefAttr_traits(traitNode *n);
# define		 traitRefAttr_traits(pn)			\
    				((pn)->attr->traits)
extern renaming		*traitRefAttr_rename(traitNode *n);
# define		 traitRefAttr_rename(pn)			\
    				((pn)->attr->rename)

extern OpList		*tupleAttr_gets(tupleNode *n);
# define		 tupleAttr_gets(pn)				\
    			 	((pn)->attr->gets)

extern OpList		*tupleAttr_sets(tupleNode *n);
# define		 tupleAttr_sets(pn)				\
    			 	((pn)->attr->sets)

extern operator		*tupleAttr_make(tupleNode *n);
# define		 tupleAttr_make(pn)				\
    			 	((pn)->attr->make)

extern OpList		*unionAttr_toS(unionNode *n);
# define		 unionAttr_toS(pn)				\
    			 	((pn)->attr->toS)

extern OpList		*unionAttr_fromS(unionNode *n);
# define		 unionAttr_fromS(pn)				\
    			 	((pn)->attr->fromS)

extern operator		*unionAttr_tag(unionNode *n);
# define		 unionAttr_tag(pn)				\
    			 	((pn)->attr->tag)

extern Sort		*unionAttr_tagS(unionNode *n);
# define		 unionAttr_tagS(pn)				\
    			 	((pn)->attr->tagS)

extern enumerationNode	*unionAttr_labels(unionNode *n);
# define		 unionAttr_labels(pn)				\
    			 	((pn)->attr->labels)


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
