/* 
**  Grammar for LSL2.3
*/

%{
/* C declarations */
%}

/* yacc declarations */

%token closeSym, eqSepSym, equationSym, mapSym, markerSym, openSym
%token quantifierSym, selectSym, sepSym
%token simpleId, eqOp, logicalOp, simpleOp
%token ASSERTS, ASSUMES, BY, CONVERTS, ELSE, ENUMERATION, EOF, EQUATIONS
%token EXEMPTING, IMPLIES, INCLUDES, FOR, GENERATED, IF, INTRODUCES, OF
%token PARTITIONED, THEN, TRAIT, TUPLE, UNION


%% /* Grammar rules */



trait:		  traitId                  ':' TRAIT traitBody EOF
    		| traitId formalParameters ':' TRAIT traitBody EOF
 
formalParameters: '(' formalList ')'
 
formalList:	  formal
    		| formalList ',' formal
;
 
formal:		  name
    		| name ':' signature
;
 
name:		  opId
    		| opForm
;
 
opForm:    	  IF markerSym THEN markerSym ELSE markerSym
		|           anyOp
    		| markerSym anyOp
		|           anyOp markerSym
    		| markerSym anyOp markerSym
		|           openSym middle closeSym
    		| markerSym openSym middle closeSym
		|           openSym middle closeSym markerSym
    		| markerSym openSym middle closeSym markerSym
		|           selectSym simpleId
    		| markerSym selectSym simpleId
;
 
anyOp:		  simpleOp
    		| logicalOp
    		| eqOp
;
 
middle:		  /* EMPTY */
		| placeList
;
 
placeList:			      markerSym
    		| placeList separator markerSym
;

separator:	  sepSym
    		| ','
;
 
signature:	  domain mapSym range
 
domain:		  /* EMPTY */
		| sortList
;
 
sortList:         sort
    		| sortList ',' sort
;
 
range:		  sort
;
 
sort:		  sortId
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
 
enumeration:	  sort ENUMERATION OF elementIdList
;
 
elementIdList:    elementId
    		| elementIdList ',' elementId
;
 
tuple:		  sort TUPLE OF fieldList
;
 
union:		  sort UNION OF fieldList
;
 
fieldList:	  fields
    		| fieldList ',' fields
;
 
fields:		  fieldIdList ':' sort
;
 
fieldIdList:      fieldId
    		| fieldIdList ',' fieldId
;
 
external:	  INCLUDES traitRefList
    		| ASSUMES  traitRefList
;
 
traitRefList:     traitRef
    		| traitRefList ',' traitRef
;
 
traitRef:	  traitId
    		| traitId	      '(' renaming ')'
    		| '(' traitIdList ')'
    		| '(' traitIdList ')' '(' renaming ')'
;
 
traitIdList:      traitId
    		| traitIdList ',' traitId
;
 
renaming:		       replaceList
    		| nameList
    		| nameList ',' replaceList
;
 
nameList:         name
    		| nameList ',' name
;
 
replaceList:      replace
    		| replaceList ',' replace
;
 
replace:    	  name FOR name
    		| name FOR name ':' signature
;
 
simpleTrait:	  opParts propParts
;
 
opParts:	  /* EMPTY */
		| opParts INTRODUCES opDcls
;
 
opDcls:           opDcl
    		| opDcls opDcl
;
 
opDcl:		  nameList ':' signature
;
 
propParts:	  /* EMPTY */
		| propParts ASSERTS props
;
 
props:		  /* EMPTY */
		| genPartProps eqPart
;
 
genPartProps:                  genPartProp
    		| genPartProps genPartProp
;
 
genPartProp:      sort GENERATED   BY operators
    		| sort PARTITIONED BY operators
;
 
eqPart:                           quantifiedEqSeqs
    		| EQUATIONS eqSeq quantifiedEqSeqs
;
 
operators:                      operator
    		| operators ',' operator
;
 
operator:    	  name
    		| name ':' signature
;
 
quantifiedEqSeqs: /* EMPTY */
		| quantifiedEqSeqs quantifier eqSeq
;
 
eqSeq:            equation
    		| eqSeq eqSepSym equation
;
 
quantifier:	  quantifierSym varDcl
    		| quantifier ','  varDcl
;
 
varDcl:		  varIds ':' sort
;
 
varIds:		  varId
    		| varIds ',' varId
;
 
equation:	  term
    		| term equationSym term
;
 
term:	    	  logicalTerm
    		| IF term THEN term ELSE term
;
 
logicalTerm:                            equalityTerm
    		| logicalTerm logicalOp equalityTerm
;
 
equalityTerm:                       simpleOpTerm
    		| simpleOpTerm eqOp simpleOpTerm
;
 
simpleOpTerm:	  prefixOpTerm
    		| secondary postfixOps
    		| secondary infixOpTerm
;
 
prefixOpTerm:	  secondary
    		| simpleOp prefixOpTerm
;
 
postfixOps:	  simpleOp
    		| postfixOps simpleOp
;
 
infixOpTerm:
			      simpleOp secondary
    		| infixOpTerm simpleOp secondary
;
 
secondary:	  primary
   		|         bracketed
   		|         bracketed primary
    		| primary bracketed
    		| primary bracketed primary
;
 
bracketed:	  matched ':' sort
    		| matched
;
 
matched:	  openSym args closeSym
    		| openSym      closeSym
;
 
args:             term
    		| args separator term
;
 
primary:	  '(' term ')'
		| simpleId
    		| simpleId '(' terms ')'
    		| primary selectSym simpleId
    		| primary ':' sort
;
 
terms:		  term
		| terms ',' term
;
 
consequences:	  IMPLIES conseqProps conversions
;
 
conseqProps:                   genPartProps eqPart
    		| traitRefList genPartProps eqPart
                |              genPartProps eqSeq
    		| traitRefList genPartProps eqSeq
;
 
conversions:	  /* EMPTY */
		| conversions conversion
;
 
conversion:	  CONVERTS operators
		| CONVERTS operators exemption
;
 
exemption:	  EXEMPTING            terms
		| EXEMPTING quantifier terms
;
 
traitId:	  simpleId
;
 
sortId:		  simpleId
;
 
varId:		  simpleId
;
 
fieldId:	  simpleId
;

elementId:	  simpleId
;
 
opId:		  simpleId
;
