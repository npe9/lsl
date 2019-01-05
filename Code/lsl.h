/*
**
**	Copyright (c) MIT, 1990, 1991, 1992, 1993, 1999
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   lsl.h
**
**	PURPOSE:    Interface to Larch Shared Language checker
**
**	     This program checks the syntax of an LSL trait.  Its behavior is
**	governed by the following command-line options:
**
**	-i init-file	     Use init-file instead of the one named
**			     INITFILENAME as the LSL initialization file.
**	-lp		     Generate .lp files for the trait when done.
**	-p		     Print the abstract syntax for the trait when done.
**	-q		     Qualify all terms and operators when printing.
**	-sorts		     Print list of sorts in trait.
**	-syms		     Print symbol table for trait.
**	-tr translationFile  Translate symbols for .lp files.
**	
**  MODIFICATION HISTORY:
**
**      {0} McKeeman at Digital -- 90.05.10 -- Original
**	{1} Garland  at MIT     -- 90.10.02 -- Remodularize
**	{2} Wild     at Digital -- 90.12.11 -- Add -debug option and print
**					       symtable, tree, and renaming
**					       for all checked traits.
**	{3} Garland  at MIT     -- 91.01.29 -- Add -sorts, change -s to -syms
**	{4} Garland  at MIT	-- 91.02.14 -- Change .sl to .lsl
**	{5} Wild     at Digital -- 91.02.20 -- Merge in MIT changes.
**	{6} Feldman  at Digital -- 91.03.19 -- Add SPECPATH
**	{7} Feldman  at Digital -- 91.03.20 -- Remove leading /usr/lib from
**					       Unix INITFILENAME.
**	{8} Wild     at Digital -- 91.05.07 -- Remove leading LSL$LIB from
**					       VMS file names.
**      {9} Garland  at MIT     -- 91.12.20 -- Add -lp command-line option
**     {10} Garland  at MIT     -- 92.01.23 -- Add -tr command-line option
**     {11} Garland  at MIT     -- 92.02.12 -- Update LSL_USAGE
**     {12} Garland  at MIT     -- 92.09.18 -- Update LSL_USAGE
**     {13} Garland  at MIT	-- 93.03.11 -- Cut down to official release
**     {14} Garland  at MIT	-- 93.08.18 -- Change SPEC_PATH to LARCH_PATH
**     {15} Garland  at MIT	-- 99.12.09 -- Revise this comment, date
**      {n} Who      at Where   -- yy.mm.dd -- What
**--
*/


/*
**
**  MACRO DEFINITIONS
**
*/

# define INITFILENAME	"lslinit.lsi"

# define LSL_SUFFIX		".lsl"
# define LSLINIT_SUFFIX 	".lsi"
# define SPECPATH		"LARCH_PATH"

# define LSL_USAGE			      \
  "Larch Shared Language Checker, Version 3.1beta3, 19 March 2001\n\
   Usage:   lsl [option ...] source-file ...\n\
   Options: -i init-file           use init-file for LSL lexical structure\n\
            -lp                    generate .lp files\n\
            -p                     prettyprint source-files\n\
            -q                     fully qualify -p output\n\
            -sorts                 print sorts in first source-file\n\
            -syms                  print symbols in first source-file\n\
            -tr translationFile    translate symbols for .lp files"

/*
**
**  TYPEDEFS
**
*/

typedef enum {
    DIGITAL,
    CXX,
    BADFLAG
} FlagNames;


/*
**
**  FUNCTION PROTOTYPES
**
*/

extern bool	GetDebugFlag(void);
extern bool	GetPrintQualFlag(void);
extern bool	GetPrintSymTableFlag(void);
extern bool	GetPrintTreeFlag(void);
extern bool	GetPrintRenameFlag(void);
extern bool	GetPrintSortsFlag(void);
extern bool	GetPrint2lpFlag(void);


/*
**
**  EXTERNAL VARIABLES (none)
**
*/
