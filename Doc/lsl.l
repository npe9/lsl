.\" Copyright (c) Massachusetts Institute of Technology, 1990, 1993, 1999
.\" All rights reserved.
.\"
.TH LSL l "December 9, 1999"
.UC 4
.SH NAME
lsl \- Larch Shared Language (LSL) Checker (Release 3.1beta3)
.SH SYNOPSIS
.B lsl
[
.I options
]
.I sourceFile .\|.\|.
.SH DESCRIPTION
The commands
.B lsl
.I abc .\|.\|.
and
.B lsl
.I abc.lsl .\|.\|.
check the LSL traits in the files
.I abc.lsl .\|.\|.
for syntax and type errors.
They also check all traits referenced by these files (in
.BR assumes ,
.BR implies ,
or
.B includes
clauses).  The LSL Checker expects to find the specification for a trait named
.I T
in a file named
.IR T.lsl .
.SH OPTIONS
.TP 8
.B \-lp
Generates
.I .lp
files for use with Version 3.1b of the Larch Prover (LP) to check the 
semantic assertions contained in LSL specifications.  For each trait
.IR T ,
the file
.I T_Axioms.lp
axiomatizes the assertions and local assumptions of
.IR T .
If 
.I T
has an
.B implies
clause, the file
.I T_Theorems.lp
axiomatizes the implied propositions.  If 
.I T
requires semantic checking, the file
.I T_Checks.lp
contains the appropriate proof obligations.  NOTE: The implementation of
.B \-lp
is incomplete and will be changed in Release 4.0).
.TP
.B \-p
Prettyprints
.I sourceFile .\|.\|.
on the standard output.
.TP
.B \-q
Fully qualifies all operators and terms in the output produced by the
.B \-lp
and
.B \-p
options.  NOTE: Until the implementation of
.B \-lp
is complete in Release 4.0, the
.B \-q
option should always be used when
.B \-lp
is used.
.TP
.BI \-i " initFile"
Causes the lexicial structure of LSL to be customized by
.I initFile.lsi
instead of the  default initialization file
.IR lslinit.lsi .
If no initialization file is found, the source files are processed without it.
.TP
.B \-sorts
Prints the sort list of the trait in the first source file on the standard
output.  (This list is used by Larch interface language checkers.)
.TP
.B \-syms
Prints signatures for the operators in the operator list of the trait in the
first source file on the standard output.  (This list is used by Larch
interface languages checkers.)
.TP
.BI \-tr " translationFile"
Causes the operators specified in 
.I translationFile 
to be replaced by new operators in the output produced by the 
.B \-lp 
option.  Replacements are described using the syntax
.IP
.I LPidentifier 
.B for 
.I name 
: 
.I signature
.PP
.SH USAGE
.PP
The LSL Checker searches for its initialization file, as well as for further
.I .lsl
files, in the current directory or, if the environment variable LARCH_PATH is
defined, in one of the directories named by that environment variable.  It is
usually convenient to define LARCH_PATH to consist of the current directory
followed by the directory containing the LSL library, for example,
.IP
setenv LARCH_PATH .:/usr/local/lib/LSL
.PP
(directory names are separated by colons).
.SH CHANGES
.PP
The definition of
.I sort
has been generalized to
.IP
.I sort
::=
.I simpleId
|
.I simpleId
[
.I sort .\|.\|.
]
.PP
The definition of 
.I term
has been generalized to allow quantified variables.
.SH "COMMON USER PROBLEMS AND THEIR SOLUTION"
.PP
If the checker reports that it is continuing without its initialization file,
ensure that LARCH_PATH is defined and that
.I lslinit.lsi
is on that path.
.PP
If the checker reports errors in a trait that it checked successfully in
another directory (e.g., when checking a trait that includes, assumes, or
implies the previously checked trait), ensure that the initialization files
in all directories on the LARCH_PATH are consistent.
.PP
If the checker reports errors in a library trait, ensure that the current
directory does not contain a trait with the same name as that or some other
library trait.
.SH "FUTURE ENHANCEMENTS"
.PP
The option
.B \-lp
will work better in Release 4.0.
.SH FILES
.ta \w'lslinit.lsi   'u
lslinit.lsi	Customization for lexical structure of LSL
.SH "SEE ALSO"
John V. Guttag and James J. Horning (editors), with Stephen. J. Garland, Kevin
D. Jones, Andres Modet, and Jeannette M. Wing, ``Larch: Languages and Tools for
Formal Specification,'' Springer-Verlag, 1993.
.SH "AUTHORS"
Stephen J. Garland, MIT Laboratory for Computer Science
.HP
Gary A. Feldman, William McKeeman, and Joseph P. Wild, 
.br
DEC Technical Languages and Environments 
