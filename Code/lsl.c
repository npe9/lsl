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
**      FILENAME:   lsl.c
**
**	PURPOSE:    Main module for Larch Shared Language checker
**
**  AUTHORS:
**	Gary Feldman, Technical Languages and Environments, DECspec project
**	Steve Garland, Massachusetts Institute of Technology
**	William McKeeman, Technical Languages and Environments, RAPIDcase proj.
**	Joe Wild, Technical Languages and Environments, DECspec project
**
**  MODIFICATION HISTORY:
**
**  X0.1-1	WM	90.05.09	Original
**  X0.1-2	SJG	90.05.17	Minor changes, LSLThisLine
**  X0.1-3	GAF	90.07.30	Fix LSLGetNextLine at EOF
**  X0.1-4	JPW	90.08.16	Add init file processing
**  X0.1-5	JPW	90.08.27	Change init file default to
**					"lslrc"
**  X0.1-6	JPW	90.08.30	Fix minor gcc errors.
**  X0.1-7	JPW	90.08.31	Change lslrc -> lslinit.sli.
**  X0.1-8	JPW	90.08.31	Change default location of
**					lslinit.sli and lsl2tex.dat.
**  X0.1-9	SJG	90.09.13	Change suffix to .sl
**					Add unparse
**					Fix test for suffix existence
**  X0.1-10	SJG	90.10.02	Remodularize
**  X0.1-11	SJG	90.10.18	Command-line options -p, -q, -s
**  X0.1-12	JPW	90.11.09	include osdfiles.h
**  X0.1-13	JPW	90.11.09	add -debug option.  Dumps everything.
**  X0.1-14	SJG	91.01.29	Change -s to -syms, add -sorts
**  X0.1-15	JPW	91.02.20	Merge in MIT changes.
**  X0.1-16	GAF	91.03.19	Add SPEC_PATH processing for init file.
**  X0.1-17	JPW	91.07.29	Add extra parameters to source_create.
**  X0.1-18	SJG	91.12.20	Add -lp command-line option, allow
**					multiple files on command-line, remove
**					-r and -syms options
**  X0.1-19     SJG     92.01.21        Restore -syms option, add -tr
**  X0.1-20	SJG	92.02.19	Reorder #include "error.h"
**  X0.1-21	SJG	93.01.07	LP output only for command line files
**  X0.1-22	SJG	93.03.12	Wired out -lp for official release
**  X0.1-23	SJG	93.03.26	Wired -lp back in
**  [@modification history entry@]...
**	
**--
*/

/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include <string.h>
# include	    "general.h"
# include	    "osdfiles.h"
# include	    "string2sym.h"
# include	    "token.h"
# include	    "error.h"
# include	    "source.h"
# include	    "scan.h"
# include	    "scanline.h"
# include	    "syntable.h"
# include	    "tokentable.h"
# include	    "parse.h"
# include	    "list.h"
# include	    "set.h"
# include	    "abstract.h"
# include	    "operator.h"
# include	    "renaming.h"
# include	    "symtable.h"
# include	    "vartable.h"
# include	    "prop.h"
# include	    "propSet.h"
# include	    "checking.h"
# include	    "unparse.h"
# include	    "semantics.h"
# include	    "lsl2lp.h"
# include	    "lslinit.h"
# include	    "lsl.h"


/*
**
**  MACRO DEFINITIONS
**
*/

# define MAXTRFILES 100

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

static void         LSLMessage(char *msg);
# define 	    LSLMessage(m) 		puts(m)

extern int	    main();
extern bool	    GetDebugFlag(void);
extern bool	    GetPrintQualFlag(void);
extern bool	    GetPrintSymTableFlag(void);
extern bool	    GetPrintTreeFlag(void);
extern bool	    GetPrintRenameFlag(void);
extern bool         GetPrintSortsFlag(void);
extern bool         GetPrint2lpFlag(void);


/*
**
**  STATIC VARIABLES
**
*/

static int	    DigitalExtensions;
static bool	    debugFlag;
static bool	    printQualFlag;
static bool	    printSymTableFlag;
static bool	    printTreeFlag;
static bool	    printRenameFlag;
static bool         printSortsFlag;
static bool	    print2lpFlag;

/*
**
** FUNCTIONS
**
**
*/


/*
**++
**  FUNCTION NAME:
**
**      main
**
**  FORMAL PARAMETERS:
**
**      [@description or none@]
**
**  RETURN VALUE:
**
**      [@description or none@]
**
**  INVARIANTS:
**   
**      [@tbs@]
**   
**  DESCRIPTION:
**
**      main entry into LSL checker
**
**	syntax: $command  [-flag ]* [filename]*
**
**  SIDE EFFECTS:
**
**      [@description or none@]
**
**  EXCEPTIONS:
**   
**      [@tbs@]
**--
*/
int
main(int argc, char *argv[]) {
    int	    		 i;
    int	    		 rc;				/* for error return */
    source  		*initFile 	      = (source *)0;
    char                *trFiles[MAXTRFILES];
    int                  nTrFiles             = 0;
    int			 firstSource	      = -1;
    traitNodeList	*allTraits	      = list_new(traitNode);

    /*
    ** Validate command
    */

    if (argc <= 1 ) {
	error_reportAndFail(LSL_USAGE);
    }

    DigitalExtensions = FALSE;				/* default	    */
    debugFlag	      = FALSE;				/* default	    */
    printQualFlag     = FALSE;				/* default	    */
    printSymTableFlag = FALSE;				/* default	    */
    printTreeFlag     = FALSE;				/* default	    */
    printRenameFlag   = FALSE;				/* default	    */
    printSortsFlag    = FALSE;				/* default	    */
    print2lpFlag      = FALSE;				/* default	    */
    
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
	    if (i++ < argc) {			/* init file name   */
	      initFile = source_create(argv[i], LSLINIT_SUFFIX, FALSE, FALSE);
	    } else {
	      error_reportAndFail(LSL_USAGE);
	    }
	} else if (strcmp(argv[i], "-lp") == 0) {
	    print2lpFlag = TRUE;
	} else if (strcmp(argv[i], "-p") == 0) {
	    printTreeFlag = TRUE;
	} else if (strcmp(argv[i], "-q") == 0) {
	    printQualFlag = TRUE;
	} else if (strcmp(argv[i], "-sorts") == 0) {
	    printSortsFlag = TRUE;
	} else if (strcmp(argv[i], "-syms") == 0) {
	    printSymTableFlag = TRUE;
/* Remove?
	} else if (strcmp(argv[i], "-debug") == 0) {
	    debugFlag = TRUE;
	    printSymTableFlag = TRUE;
	    printQualFlag = TRUE;
	    printTreeFlag = TRUE;
	    printRenameFlag = TRUE;
	    printSortsFlag = TRUE;
*/
/* Remove the following once translation file isn't needed */
	} else if (strcmp(argv[i], "-tr") == 0) {
	    if (i++ < argc) {
	      if (nTrFiles >= MAXTRFILES)
		error_reportAndFail("too many -tr options");
	      trFiles[nTrFiles++] = argv[i];
	    } else {
	      error_reportAndFail(LSL_USAGE);
	    }
	} else {
	    firstSource = i;
	    break;
        }
    }
    if (firstSource < 0) {
	error_reportAndFail(LSL_USAGE);
    }


    /* 
    ** Open init file
    */

    if (initFile == (source *) 0) {
	initFile = source_create(INITFILENAME, LSLINIT_SUFFIX, FALSE, FALSE);
	if (!source_getPath(SPECPATH, initFile, token_null())) {
	    LSLMessage(string_paste("Continuing without LSL init file: ",
				source_fileName(initFile)));
	} else if (!source_open(initFile)) {
	    LSLMessage(string_paste("Continuing without LSL init file: ",
				    source_fileName(initFile)));
	}
    } else if (!source_open(initFile)) {
        LSLMessage(string_paste("Continuing without LSL init file: ",
				source_fileName(initFile)));
    }


    /*
    ** Initialize checker
    */

    rc = error_setQuit();

    if (rc == 0) {					/* first try	    */
	LSLStringInit();
	LSLStringReset();

	LSLSynTableInit();
	LSLSynTableReset();

	LSLTokenTableInit();
	LSLTokenTableReset();

	LSLScanLineInit();
	LSLScanLineReset();

	LSLScanInit();

	if (source_isOpen(initFile)) {
	    LSLScanReset(initFile);
	    LSLProcessInitFileInit();
	    LSLProcessInitFileReset();
	    LSLProcessInitFile();
	    LSLProcessInitFileCleanup();
	}
    }
    if (error_count() > 0) {
	error_reportAndFail("Abort: LSL init file error");
    }
    LSLParseInit();
    LSLAbstractInit();


    /*
    ** Load, parse, and check traits
    */

    for (i = firstSource; i < argc; i++) {
    	LSLToken *id;
	bool	  exists;
	id = (LSLToken *)LSLMoreMem((void *)0, sizeof(LSLToken));
	*id = LSLInsertToken(SIMPLEID, LSLString2Symbol(argv[i]), 0, FALSE);
	exists = FALSE;
	list_traverse(traitNode, allTraits, t1) {
	    if (token_textSym(*id) == token_textSym(*traitNode_name(t1))) {
		traitNode_commandLineArg(t1) = TRUE;
		exists = TRUE;
		break;
	    }
	} end_traverse;
	if (!exists) {
	    traitNode *t ;
	    token_setFileName(*id, "command line argument");
	    token_setLine(*id, i);
	    token_setCol(*id, 0);
	    t = parseAndCheckTrait(id, allTraits);
	    traitNode_commandLineArg(t) = TRUE;
	    if (i == firstSource && !traitAttr_parseError(t)) {
	      if (GetPrintSortsFlag()) symtable_printSorts(trait_symtable(t));
	      if (GetPrintSymTableFlag()) symtable_unparse(trait_symtable(t));
	    }
	}
    }

    /*
    ** Generate output for LP
    */

    if (print2lpFlag && error_count() == 0) {
	doSemantics(allTraits);
	outputLP(allTraits, trFiles, nTrFiles);
    }

    /* 
    ** Postprocessing
    */

    LSLStringCleanup();
    LSLSynTableCleanup();
    LSLTokenTableCleanup();
    LSLScanLineCleanup();
    LSLScanCleanup();
    LSLParseCleanup();
    LSLAbstractCleanup();

    if (error_count() > 0) 
      error_reportAndFail("Abort: error in checking LSL traits");
    if (!GetPrintSortsFlag() && !GetPrintSymTableFlag()) 
      LSLMessage("Finished checking LSL traits");
    exit(SUCCESS);
}


bool
GetDebugFlag (void) {
    return debugFlag;
}

bool
GetPrintQualFlag (void) {
    return printQualFlag;
}

bool
GetPrintSymTableFlag (void) {
    return printSymTableFlag;
}

bool
GetPrintTreeFlag (void) {
    return printTreeFlag;
}

bool
GetPrintRenameFlag (void) {
    return printRenameFlag;
}

bool
GetPrintSortsFlag (void) {
    return printSortsFlag;
}

bool
GetPrint2lpFlag (void) {
    return print2lpFlag;
}
