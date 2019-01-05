**************************************
Testing ../Code/lsl -i ../LSL/lslinit.lsi
**************************************
************* Test input from check03.lsl ***********
./check03.lsl:4,19: `__ + __' number of __'s in opForm does not match signature
Abort: error in checking LSL traits
************* End of input from check03.lsl *********
************* Test input from check04.lsl ***********
./check04.lsl:8,20: not expecting two different qualifications
Abort: error in checking LSL traits
************* End of input from check04.lsl *********
************* Test input from check05.lsl ***********
./check05.lsl:10,22: `M' sort not introduced
./check05.lsl:11,20: `c' variable duplicates constant of same sort
./check05.lsl:12,25: `i' duplicate variable in declaration
./check05.lsl:15,18: `__ \eq __' not declared with matching domain sorts
        Possible sorts for arg 1: N, T
        Possible sorts for arg 2: S
./check05.lsl:16,18: `__ \eq __' ambiguous operator
Abort: error in checking LSL traits
************* End of input from check05.lsl *********
************* Test input from check06.lsl ***********
./check06.lsl:4,43: `white' duplicate element in enumeration
./check06.lsl:5,46: `Pair' field sort must differ from tuple sort
./check06.lsl:6,49: `hue' duplicate field in union
Abort: error in checking LSL traits
************* End of input from check06.lsl *********
************* Test input from check07.lsl ***********
./check07.lsl:19,31: `s1' no matching operator
./check07.lsl:20,34: `h' no matching operator with generated sort as range
./check07.lsl:21,31: `-' ambiguous operator
./check07.lsl:22,18: all generators have the generated sort in their domains
./check07.lsl:23,34: `0' ambiguous operator
./check07.lsl:24,31: `0' ambiguous operator
./check07.lsl:28,33: `>' no matching operator
./check07.lsl:29,33: `f' no matching operator with partitioned sort in domain
./check07.lsl:30,33: `-' ambiguous operator
./check07.lsl:31,33: `f' ambiguous operator
./check07.lsl:31,36: `g' ambiguous operator
./check07.lsl:32,36: `f' ambiguous operator
./check07.lsl:33,33: `f' ambiguous operator
Abort: error in checking LSL traits
************* End of input from check07.lsl *********
************* Test input from check08.lsl ***********
./check08.lsl:10,18: all generators have the generated sort in their domains
./check08.lsl:11,27: `A' sort not introduced
./check08.lsl:12,24: `j' undeclared constant or variable
./check08.lsl:13,25: `-' no matching operator
./check08.lsl:14,37: `i' undeclared constant or variable
./check08.lsl:15,37: `0' exempted term lacks converted operator
Abort: error in checking LSL traits
************* End of input from check08.lsl *********
************* Test input from check09.lsl ***********
./check09.lsl:3,12: `+' no matching operator for formal
./check09.lsl:3,27: `f' no matching sort or operator for formal
./check09.lsl:11,16: `check09' reference to trait containing errors
Abort: error in checking LSL traits
************* End of input from check09.lsl *********
************* Test input from check10.lsl ***********
./check10a.lsl:4,17: `check10' cycle in trait hierarchy
./check03.lsl:4,19: `__ + __' number of __'s in opForm does not match signature
./check10b.lsl:4,17: `check03' reference to trait containing errors
./check10.lsl:7,17: `check10c' no formals for traitRef
./check10.lsl:8,17: `check10d' wrong number of actual parameters for traitRef
./check10.lsl:9,17: `check10e' wrong number of actual parameters for traitRef
./check10.lsl:10,26: `+' actual parameter must be a sort
./check10.lsl:11,29: `__ + __' too many __'s in new name for operator
./check10.lsl:12,26: `+' new name unsuitable for old operator with arity > 2
./check10.lsl:13,26: `.first' old operator for new selectOp name does not have arity 1
./check10.lsl:14,29: `+' ambiguous where __ belongs in new name for operator
./check10.lsl:15,32: `__ *' too few __'s in new name for operator
./check10.lsl:16,26: `[ __ ]' wrong number of __'s in new name for operator
Abort: error in checking LSL traits
************* End of input from check10.lsl *********
************* Test input from check11.lsl ***********
./check11.lsl:4,32: `A' nonexistent old operator in renaming
./check11.lsl:5,26: `__ + __' too many __'s in new name for operator
./check11.lsl:6,26: `+' new name unsuitable for old operator with arity > 2
./check11.lsl:7,26: `.first' old operator for new selectOp name does not have arity 1
./check11.lsl:8,26: `*' ambiguous where __ belongs in new name for operator
./check11.lsl:9,26: `__ *' too few __'s in new name for operator
./check11.lsl:10,26: `[ __ ]' wrong number of __'s in new name for operator
./check11.lsl:11,41: `A' old sort already used in renaming
./check11.lsl:12,41: `A' old sort already used in renaming
./check11.lsl:13,35: `f' old operator already used in renaming
./check11.lsl:14,41: `f' old operator already used in renaming
Abort: error in checking LSL traits
************* End of input from check11.lsl *********
************* Test input from check12.lsl ***********
Finished checking LSL traits
************* End of input from check12.lsl *********
************* Test input from check13.lsl ***********
./check13.lsl:4,17: `check13a' error in traitRef caused by renamed sorts
./check13a.lsl:4,32: `B' field sort must differ from tuple sort
./check13.lsl:4,17: `check13a' error in traitRef caused by renamed sorts
./check13a.lsl:10,18: all generators have the generated sort in their domains
./check13.lsl:4,17: `check13a' error in traitRef caused by renamed sorts
./check13a.lsl:12,24: `x' duplicate variable in declaration
Abort: error in checking LSL traits
************* End of input from check13.lsl *********
************* Test input from check14.lsl ***********
./check14.lsl:4,21: `-' ambiguous where __ belongs in new name for operator
Abort: error in checking LSL traits
************* End of input from check14.lsl *********
