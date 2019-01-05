/*
**
**	Copyright (c) MIT, 1991, 1992, 1993
**	All Rights Reserved.  Unpublished rights reserved under the copyright 
**	laws of the United States.
**
**++
**  FACILITY:  LSL Checker
**
**  MODULE DESCRIPTION:
**
**      FILENAME:   print.c
**
**	PURPOSE:    Print routines for output
**
**  MODIFICATION HISTORY:
**
**      {0} Garland at MIT   -- 91.12.20 -- Original
**      {1} Garland at MIT   -- 92.01.24 -- Line break only at space
**	{2} Garland at MIT   -- 92.02.19 -- Reorder #include "error.h"
**	{3} Garland at MIT   -- 93.12.06 -- Add #includes for clean compile
**      {n} Who     at Where -- yy.mm.dd -- What
**--
*/


/*
**
**  INCLUDE FILES
**
*/

# include <stdio.h>
# include <string.h>
# include	"general.h"
# include	"string2sym.h"
# include	"token.h"
# include 	"error.h"
# include	"print.h"
# include	"lsl.h"

/*
**
**  MACRO DEFINITIONS
**
*/

# define MAXLINELEN      78
# define CONTINUE_INDENT 2
# define BUFSIZE         500


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

extern bool		 print_closeFile(void);
extern bool		 print_openFile(char *name, char *suffix);
extern void		 print_char(char c);
extern void		 print_str(char *s);
extern void		 print_setIndent(unsigned int n);
extern void		 print_adjustIndent(int n);
extern void		 print_setPrefix(char *s);
extern void		 print_newline(unsigned int n);

static void		 startLine(void);
static void              outputWords(void);


/*
**
**  STATIC VARIABLES
**
*/

static FILE		*out = 0; /*1/11/00 compiler complained about '= stdout';*/
static unsigned int	 indent 	= 0;
static unsigned int	 extraIndent	= 0;	     /* for continued lines */
static unsigned int	 linelen	= 0;
static char		 prefix[BUFSIZE];
static char              buf[BUFSIZE];
static char              buf1[BUFSIZE];
static bool		 emptyLine 	= TRUE;
static bool		 someFlushed    = FALSE;


/*
**
**  FUNCTION DEFINITIONS
**
*/

extern bool
print_closeFile (void) {
    if (strlen(buf) > 0) print_newline(1);
    if (out == 0 || out == stdout) return FALSE;   /* Hacked 1/11/00 */
    fclose(out);
    out = stdout;
    return TRUE;
}


extern bool
print_openFile (char *name, char *suffix) {
    char 	*ps;
    char	*name1;
    name1 = (char *)LSLMoreMem((void *)0, strlen(name) + strlen(suffix) + 1);
    strcpy(name1, name);
    ps = strrchr(name1, '/');
    if (ps == (char *)0) ps = name1;
    if (strchr(ps, '.') == NULL) {
	strcat(name1, suffix);
    }
    print_closeFile();
    out = fopen(name1, "w");
    if (out == 0) out = stdout;
    if (GetDebugFlag()) setbuf(out, (char *)0);
    buf[0] = '\0';
    return out != stdout;
}


extern void
print_char (char c) {
    int buflen = strlen(buf);
    if (buflen + 1 >= BUFSIZE) 
      error_programFailure("output string is too long");
    buf[buflen] = c;
    buf[buflen + 1] = '\0';
    if (c == ' ') outputWords();
}


extern void
print_str (char *s) {
    int buflen = strlen(buf);
    if (buflen + strlen(s) >= BUFSIZE)
      error_programFailure("output string is too long");
    strcat(buf, s);
    if (strchr(s, ' ') != 0) outputWords();
}


extern void
print_newline (unsigned int n) {
    int i;
    if (strlen(buf) > 0) print_char(' ');                   /* Flush buffer */
    for (i = 0; i < n; i++) fputc('\n', out);
    extraIndent = 0;
    emptyLine = TRUE;
    linelen = 0;
    buf[0] = '\0';
}


extern void
print_setIndent (unsigned int n) {
    indent = n;
}


extern void
print_adjustIndent (int n) {
    if (n < 0 && -n > indent)
      error_programFailure("print_adjustIndent");
    indent = indent + n;
}


extern void
print_setPrefix (char *s) {
    if (strlen(s) >= BUFSIZE) 
      error_programFailure("output string is too long");
    strcpy(prefix, s);
}


static void
startLine (void) {
    int i;
    /* Added 1/11/00, because Linux C wouldn't accept stdout as initial
       value for the static variable out.
    */
    if (out == 0) out = stdout;
    fputs(prefix, out);
    for (i = 0; i < indent + extraIndent; i++) fputc(' ', out);
    emptyLine = FALSE;
    someFlushed = FALSE;
    linelen = strlen(prefix) + indent + extraIndent;
}


static void
outputWords (void) {
    char *word, *nextBlank;
    int   nSpaces, wordlen;
    for (;;) {                          /* Output whitespace and next word */
      for (nSpaces = 0; buf[nSpaces] == ' '; nSpaces++);
      word = &buf[nSpaces];
      nextBlank = strchr(word, ' ');
      if (nextBlank == 0) break;        /* Exit if no whitespace after word */
      wordlen = nextBlank - word;
      if (emptyLine) startLine();
      if (linelen + nSpaces + wordlen > MAXLINELEN && someFlushed) {
	fputc('\n', out);
	extraIndent = CONTINUE_INDENT;
	startLine();
      }
      if (someFlushed) {
	strncpy(buf1, buf, nSpaces + wordlen);
	buf1[nSpaces + wordlen] = '\0';
	linelen += nSpaces + wordlen;
      } else {
	strncpy(buf1, word, wordlen);
	buf1[wordlen] = '\0';
	linelen += wordlen;
      }
      fputs(buf1, out);
      someFlushed = TRUE;
      strcpy(buf1, nextBlank);              /* Is it safe to combine these */
      strcpy(buf, buf1);                    /* two copies?                 */
    }
}
