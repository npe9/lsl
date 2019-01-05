` FILE: lsl.lex
` PURPOSE: Lexical Grammar for the Larch Shared Language
` MODIFICATIONS: modified on Wed Jun  6 11:03:45 PDT 1990 by horning
`		 Wild at Digital -- 90.07.31 -- updated to 2.3 grammar
` CONVENTIONS: This is a longest match grammar. The production RHS's are
`	       separated by a new line.  This file defines the initial
`	       lexical grammar before the init file has been processed.
`	       e* - zero or more e's
`	       e+ - one or more e's
`	       'xy' - a terminal symbol of characters 'x' and 'y'.
`              x ... y - all the lexemes between and including x and y.
`              x - y - all lexemes in x not including lexemes in y.
`              ALLCHAR - all possible characters (for now that can be
`                        represented in a octet (byte)).
`	       SPACE - the space terminal character.
`              TAB - the tab terminal character.
`              EOL - the end-of-line terminal character.
`              EOF - the end-of-file terminal character.
`	       % - comments (from % to end-of-line)
traitFile
   lexeme* EOF
 
lexeme
   token
   whiteChar
   comment
 
token
   simpleId
   reservedWord
   simpleOp
   reservedSym
   permChar
   singleChar
 
simpleId
   idSym - reservedWord
 
reservedWord
   'asserts' ... 'union'        % lower case only
 
simpleOp
   opSym - reservedSym
 
opSym
   opChar+
   '\' idChar+
 
idSym
   idChar+
   '\:' idchar*
 
reservedSym
   mapSym
   quantifierSym
   logicalOp
   eqOp
   equationSym
   eqSepSym
   selectSym
   openSym
   sepSym
   closeSym
   makerSym
   commentSym
 
mapSym
   '\arrow'
 
quantifierSym
    '\forall'
 
logicalOp
   '\and'
   '\or'
   '\implies'
 
eqOp
   '\eq'
   '\neq'
 
equationSym
   '\equals'
 
eqSepSym
   '\eqsep'
 
selectSym
   '\select'
 
openSym
   '\(' idChar*
 
sepSym
   '\,' idChar*
 
closeSym
   '\)' idChar*
 
markerSym
   '\marker'
 
commentSym
   '\comment'
 
comment
   commentSym commentChar* endCommentChar

idChar
   'a' ... 'z'
   'A' ... 'Z'
   '0' ... '9'
   '_'
 
opChar
   '*'
   '+'
   '-'
   '.'
   '/'
   '<'
   '='
   '>' 
 
whiteChar
   SPACE
   TAB
   EOL

extensionChar
    '\'		% Initial value of extensionChar.  Can be changed by init
		% file.

permChar
    comma
    colon
    lPar
    rPar

comma
    ','

colon
    ':'

lPar
    '('

rPar
    ')'

singleChar
   ALLCHAR - idChar - opChar - whiteChar - extensionChar - permChar
 
commentChar
   ALLCHAR - endCommentChar
 
endCommentChar
   EOL
