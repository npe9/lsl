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
**      FILENAME:   token.c
**
**	PURPOSE:    LSL token abstraction
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 90.10.05 -- Original
**	{1} Garland at MIT   -- 91.12.20 -- Add IfToken
**	{2} Garland at MIT   -- 93.11.09 -- Add ExistsToken, AllToken
**	{3} Garland at MIT   -- 93.11.26 -- Add IffToken
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include "general.h"
# include "string2sym.h"
# include "token.h"

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

extern LSLToken token_null();


/*
**
**  PUBLIC VARIABLES
**
*/

/* Place to store the \keyword (\forall ...) tokens.  These tokens will	    */
/* have to be modified when the extensionChar ("\") changes. 		    */

LSLToken    ForallToken;
LSLToken    ExistsToken;
LSLToken    AllToken;
LSLToken    TrueToken;
LSLToken    FalseToken;
LSLToken    NotToken;
LSLToken    AndToken;
LSLToken    OrToken;
LSLToken    ImpliesToken;
LSLToken    IffToken;
LSLToken    EqToken;
LSLToken    NeqToken;
LSLToken    EqualsToken;
LSLToken    EqsepToken;
LSLToken    SelectToken;
LSLToken    OpenToken;
LSLToken    SepToken;
LSLToken    CloseToken;
LSLToken    SimpleidToken;
LSLToken    ArrowToken;
LSLToken    MarkerToken;
LSLToken    CommentToken;
LSLToken    IfToken;
LSLToken    ComposeToken;


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

extern LSLToken
token_null (void) {
    LSLToken t = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    return t;
}
