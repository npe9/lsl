/*
**
**	Copyright (c) MIT, 1990, 1991
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   lslinit.h
**
**	PURPOSE:    Interface to the LSL init file processing routines.
**
**  MODIFICATION HISTORY:
**
**      {0} Wild at Digital -- 90.08.09 -- created
**	{1} Wild at Digital -- 90.08.27 -- fix redef on line errors
**      {n} Who  at Where   -- yy.mm.dd -- What
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

typedef enum {
			/* initFile:				    */
    INITFILE1,		/*    initLines EOF */



                        /* initLines: */
    INITLINES1,         /*		*/
    INITLINES2,         /*     initLine  */
    INITLINES3,         /*     initLines initLine */

                        /* initLine: */
    INITLINE1,		/*     EOL   */
    INITLINE2,          /*     classification EOL */

                        /* classification: */
    CLASSIFICATION1,    /*     charClass */
    CLASSIFICATION2,    /*     tokenClass */
    CLASSIFICATION3,    /*     synClass */

                        /* charClass: */
    CHARCLASS1,         /*     ENDCOMMENTCHAR endCommentChars */
    CHARCLASS2,         /*     IDCHAR idChars */
    CHARCLASS3,         /*     OPCHAR opChars */
    CHARCLASS4,         /*     EXTENSIONCHAR extensionChar */
    CHARCLASS5,         /*     SINGLECHAR singChars */
    CHARCLASS6,         /*     WHITECHAR whiteChars */


			/* endCommentChars: */
    ENDCOMMENTCHARS1,	/*     endCommentChar */
    ENDCOMMENTCHARS2,	/*     endCommentChars endCommentChar */

    			/* idChars: */
    IDCHARS1,		/*     idChar */
    IDCHARS2,		/*     idChars idChar */

			/* opChars */
    OPCHARS1,		/*    opChar */
    OPCHARS2,		/*    opChars opChar */

			/* ExtensionChar */
    EXTENSIONCHAR1,	/*     singleChar */

			/* singChars */
    SINGCHARS1,		/*     singChar */
    SINGCHARS2,		/*     singChars singChar */

			/* whiteChars */
    WHITECHARS1,	/*     whiteChar */
    WHITECHARS2,	/*     whiteChars whiteChar */


			/* endCommentChar */
    ENDCOMMENTCHAR1,	/*     commentChar */
    
			/* idChar */
    IDCHAR1,		/*     singleChar */
    
			/* opChar */
    OPCHAR1,		/*     singleChar */
    
			/* singChar */
    SINGCHAR1,		/*     singleChar */
    
			/* whiteChar */
    WHITECHAR1,		/*     singleChar */

                        /* tokenClass: */
    TOKENCLASS1,        /*     QUANTIFIERSYM quantifierSymToks */
    TOKENCLASS2,        /*     LOGICALOP logicalOpToks */
    TOKENCLASS3,        /*     EQOP eqOpToks */
    TOKENCLASS4,        /*     EQUATIONSYM equationSymToks */
    TOKENCLASS5,        /*     EQSEPSYM eqSepSymToks */
    TOKENCLASS6,        /*     SELECTSYM selectSymToks */
    TOKENCLASS7,        /*     OPENSYM openSymToks */
    TOKENCLASS8,        /*     SEPSYM sepSymToks */
    TOKENCLASS9,        /*     CLOSESYM closeSymToks */
    TOKENCLASS10,       /*     SIMPLEID simpleIdToks */
    TOKENCLASS11,       /*     MAPSYM mapSymToks */
    TOKENCLASS12,       /*     MARKERSYM markerSymToks */
    TOKENCLASS13,       /*     COMMENTSYM commentSymToks */

			/* quantifierSymToks */
    QUANTIFIERSYMTOKS1,/*     quantifierSymTok */
    QUANTIFIERSYMTOKS2,/*     quantifierSymToks quantifierSymTok */

			/* logicalOpToks */
    LOGICALOPTOKS1,	/*     logicalOpTok */
    LOGICALOPTOKS2,	/*     logicalOpToks logicalOpTok */

			/* eqOpToks */
    EQOPTOKS1,		/*     eqOpTok */
    EQOPTOKS2,		/*     eqOpToks eqOpTok */

			/* equationSymToks */
    EQUATIONSYMTOKS1, /*     equationSymTok */
    EQUATIONSYMTOKS2,	/*     equationSymToks equationSymTok */

			/* eqSepSymToks */
    EQSEPSYMTOKS1,	/*     eqSepSymTok */
    EQSEPSYMTOKS2,	/*     eqSepSymToks eqSepSymTok */

			/* selectSymToks */
    SELECTSYMTOKS1,	/*     selectSymTok */
    SELECTSYMTOKS2,	/*     selectSymToks selectSymTok */

			/* openSymToks */
    OPENSYMTOKS1,	/*     openSymTok */
    OPENSYMTOKS2,	/*     openSymToks openSymTok */

			/* sepSymToks */
    SEPSYMTOKS1,	/*     sepSymTok */
    SEPSYMTOKS2,	/*     sepSymToks sepSymTok */

    			/* closeSymToks */
    CLOSESYMTOKS1,	/*     closeSymTok */
    CLOSESYMTOKS2,	/*     closeSymToks closeSymTok */

			/* simpleIdToks */
    SIMPLEIDTOKS1,	/*     simpleIdTok */
    SIMPLEIDTOKS2,	/*     simpleIdToks simpleIdTok */

			/* mapSymToks */
    MAPSYMTOKS1,		/*     mapSymTok */
    MAPSYMTOKS2,		/*     mapSymToks mapSymTok */

			/* markerSymToks */
    MARKERSYMTOKS1,	/*     markerSymTok */
    MARKERSYMTOKS2,	/*     markerSymToks markerSymTok */

			/* commentSymToks */
    COMMENTSYMTOKS1,	/*     commentSymTok */
    COMMENTSYMTOKS2,	/*     commentSymToks commentSymTok */

                     	/* quantifierSymTok */
    QUANTIFIERSYMTOK1,	/*     token */

                  	/* logicalOpTok */
    LOGICALOPTOK1,	/*     token */

             		/* eqOpTok */
    EQOPTOK1,		/*     token */

			/* equationSymTok */
    EQUATIONSYMTOK1,	/*     token */

			/* eqSepSymTok */
    EQSEPSYMTOK1,	/*     token */

			/* selectSymTok */
    SELECTSYMTOK1,	/*     token */

			/* openSymTok */
    OPENSYMTOK1,	/*     token */

			/* sepSymTok */
    SEPSYMTOK1,		/*     token */

			/* closeSymTok */
    CLOSESYMTOK1,	/*     token */

			/* simpleIdTok */
    SIMPLEIDTOK1,	/*     token */

			/* mapSymTok */
    MAPSYMTOK1,		/*     token */

			/* markerSymTok */
    MARKERSYMTOK1,	/*     token */

			/* commentSymTok */
    COMMENTSYMTOK1,	/*     token */

                        /* synClass: */
    SYNCLASS1,          /*     SYNONYM oldToken newToken */

                        /* oldToken: */
    OLDTOKEN1,          /*     token */

                        /* newToken: */
    NEWTOKEN1           /*     token */

}	    LSLInitRuleCode;

/*
**
**  FUNCTION PROTOTYPES
**
*/

extern void	    LSLProcessInitFile(void);

extern void	    LSLProcessInitFileInit(void);
extern void	    LSLProcessInitFileReset(void);
extern void	    LSLProcessInitFileCleanup(void);

/*
**
**  EXTERNAL VARIABLES (none)
**
*/

