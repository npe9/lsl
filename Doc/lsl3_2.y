/* 
**  Grammar for LSL3.2
**  
**  FIX:
**    Fix term syntax to match LP
*/

%{
/* C declarations */
%}

/* yacc declarations */

%token		IF, THEN, ELSE
%left 		IFF
%nonassoc	IMPLIES
%left		OR, AND
%nonassoc	EQ, NEQ
%nonassoc	OPERATOR
%token		OPEN, CLOSE, '[', ']'
%left		SELECT, ':'
%token		IDENTIFIER
%nonassoc	ALL, EXISTS

%token EQSEPsym, EQUATIONsym, MAPsym, MARKERsym, QUANTIFIERsym, SEPsym
%token ASSERTS, ASSUMES, BY, CONVERTS, ENUMERATION, EOF, EQUATIONS
%token EXEMPTING, FOR, FREELY, GENERATED, IMPLIESword, INCLUDES, INTRODUCES, OF
%token PARTITIONED, SORT, TRAIT, TUPLE, UNION


%% /* Grammar rules */



trait:		  traitId                 ':' TRAIT traitBody EOF
    		| traitId '(' formals ')' ':' TRAIT traitBody EOF
 
formals:	  formal
    		| formals ',' formal
;
 
formal:		  name ':' signature
  		| opForm
    		| compoundSort
		| sortOrOpId
;
 
name:		  opId
    		| opForm
;
 
opForm:    	  IF MARKERsym THEN MARKERsym ELSE MARKERsym
		|           OPERATOR
    		| MARKERsym OPERATOR
		|           OPERATOR MARKERsym
    		| MARKERsym OPERATOR MARKERsym
		|           openSym middle closeSym
    		| MARKERsym openSym middle closeSym
		|           openSym middle closeSym MARKERsym
    		| MARKERsym openSym middle closeSym MARKERsym
		|           SELECT IDENTIFIER
    		| MARKERsym SELECT IDENTIFIER
;
 
openSym:	  OPEN
  		| '['
;

closeSym:	  CLOSE
  		| ']'
;

middle:		  /* EMPTY */
		| places
;
 
places:		  MARKERsym
    		| places separator MARKERsym
;

separator:	  SEPsym
    		| ','
;
 
signature:	  domain MAPsym range
 
domain:		  /* EMPTY */
		| sorts
;
 
sorts:	          sort
    		| sorts ',' sort
;
 
range:		  sort
;
 
sort:		  sortId
		| compoundSort
;

compoundSort:	  sortId '[' sorts ']'
;
 
traitBody:                      simpleTrait
    		|               simpleTrait consequences
    		| traitContexts simpleTrait
    		| traitContexts simpleTrait consequences
;
 
traitContexts:    traitContext
    		| traitContexts traitContext
;
 
traitContext:     shorthand
    		| external
;
 
shorthand:	  enumeration
    		| tuple
    		| union
;
 
enumeration:	  sort ENUMERATION OF elementIds
;
 
elementIds:       elementId
    		| elementIds ',' elementId
;
 
tuple:		  sort TUPLE OF fieldList
;
 
union:		  sort UNION OF fieldList
;
 
fieldList:	  fields
    		| fieldList ',' fields
;
 
fields:		  fieldIds ':' sort
;
 
fieldIds:   	  fieldId
    		| fieldIds ',' fieldId
;
 
external:	  INCLUDES traitRefs
    		| ASSUMES  traitRefs
;
 
traitRefs:        traitRef
    		| traitRefs ',' traitRef
;
 
traitRef:	  traitId
    		| traitId	   '(' renaming ')'
    		| '(' traitIds ')'
    		| '(' traitIds ')' '(' renaming ')'
;
 
traitIds:         traitId
    		| traitIds ',' traitId
;
 
renaming:		      replaces
    		| actuals
    		| actuals ',' replaces
;

actuals:	  actual
		| actuals ',' actual
;

actual:		  opForm
		| compoundSort
		| sortOrOpId
;

replaces:         replace
    		| replaces ',' replace
;
 
replace:    	  actual FOR formal
;
 
simpleTrait:	  opParts propParts
;
 
opParts:	  /* EMPTY */
		| opParts INTRODUCES opDcls
;
 
opDcls:           opDcl
    		| opDcls ',' opDcl
;
 
opDcl:		  names ':' signature
;
 
names:            name
    		| names ',' name
;
 
propParts:	  /* EMPTY */
		| propParts ASSERTS props
;
 
props:			       eqPart
		| genPartProps eqPart
;
 
genPartProps:     genPartProp
    		| genPartProps genPartProp
;
 
genPartProp:      SORT sort GENERATED FREELY BY operators
		| SORT sort GENERATED BY operators
    		| SORT sort PARTITIONED BY operators
  		| sortId GENERATED FREELY BY operators
  		| sortId GENERATED BY operators
  		| sortId PARTITIONED BY operators
;
 
eqPart:                          quantifiedEqns
    		| EQUATIONS eqns quantifiedEqns
;
 
operators:        operator
    		| operators ',' operator
;
 
operator:    	  name
    		| name ':' signature
;
 
quantifiedEqns:   /* EMPTY */
		| quantifiedEqns varDcls eqns
;
 
eqns:             equation
    		| eqns EQSEPsym equation
  		| eqns EQSEPsym
;
 
varDcls:	  QUANTIFIERsym varDcl
    		| varDcls ','  varDcl
;
 
varDcl:		  varIds ':' sort
;
 
varIds:		  varId
    		| varIds ',' varId
;
 
equation:	  term
    		| term EQUATIONsym term
;
 
term:	    	  IF term THEN term ELSE term
    		| subterm
;
 
subterm:          subterm IFF subterm
  		| subterm IMPLIES subterm
  		| subterm AND subterm
  		| subterm OR subterm
  		| subterm EQ subterm
  		| subterm NEQ subterm
  		| subterm OPERATOR subterm
  		| subterm OPERATOR
  		| prefixTerm
  		| secondary
;
 
prefixTerm:	  quantifier primary
  		| OPERATOR secondary
  		| quantifier prefixTerm
  		| OPERATOR prefixTerm
;

quantifier:	  ALL variable
  		| EXISTS variable
;

variable:	  varId
  		| varId ':' sort
;
 
secondary:	  primary
  		| primaryHead                                bracketed bracketTail
  		| primaryHead primaryTail SELECT primaryHead bracketed bracketTail
  		|                                            bracketed bracketTail
;
 
primary:	  primaryHead primaryTail
;

primaryTail:	  /* EMPTY */
  		| primaryTail ':' sort
  		| primaryTail SELECT primaryHead
;

primaryHead:	  IDENTIFIER 
  		| IDENTIFIER '(' terms ')'
		| '(' term ')'
;

terms:		  term
  		| terms ',' term
;

optionalTerms:	  /* EMPTY */
  		| terms
;

bracketed:	  '[' optionalTerms ']'
    		| '[' optionalTerms ']' ':' sort
;

bracketTail:	  /* EMPTY */
    		| primary
    		| SELECT primaryHead primaryTail
;

consequences:	  IMPLIESword conseqProps conversions
;
 
conseqProps:    			 eqPart
		| traitRefs		 eqPart
                |           genPartProps eqPart
    		| traitRefs genPartProps eqPart
                |           genPartProps eqns
    		| traitRefs genPartProps eqns
;
 
conversions:	  /* EMPTY */
		| conversions conversion
;
 
conversion:	  CONVERTS operators
		| CONVERTS operators exemption
;
 
exemption:	  EXEMPTING         terms
		| EXEMPTING varDcls terms
;
 
traitId:	  IDENTIFIER
;
 
sortId:		  IDENTIFIER
;
 
varId:		  IDENTIFIER
;
 
fieldId:	  IDENTIFIER
;

elementId:	  IDENTIFIER
;
 
opId:		  IDENTIFIER
;

sortOrOpId:	  IDENTIFIER
;
