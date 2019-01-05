# Makefile for building the LSL Checker

CC		= gcc
CFLAGS  	= ${IFLAGS} -Wall -g -o $@
#CFLAGS  	= ${IFLAGS} -o $@
IFLAGS		= -DNOSTDLIB=1
LFLAGS		= 
DEFINE		= -DTRACING=0
LINK		= ${CC}
RM		= /bin/rm -f

.SUFFIXES:
.SUFFIXES: .c .o


SOURCES = abstract.c checking.c error.c general.c list.c lsl.c lsl2lp.c     \
	  lslinit.c operator.c osdfiles.c parse.c print.c prop.c propSet.c  \
	  recheck.c reduce.c renaming.c scan.c scanline.c semantics.c set.c \
	  shift.c source.c string2sym.c symtable.c syntable.c token.c 	    \
	  tokentable.c unparse.c vartable.c 

OBJECTS = abstract.o checking.o error.o general.o list.o lsl.o lsl2lp.o     \
	  lslinit.o operator.o osdfiles.o parse.o print.o prop.o propSet.o  \
	  recheck.o reduce.o renaming.o scan.o scanline.o semantics.o set.o \
	  shift.o source.o string2sym.o symtable.o syntable.o token.o 	    \
	  tokentable.o unparse.o vartable.o 


lsl: ${OBJECTS}
	${LINK}	${OBJECTS} ${LFLAGS} -o lsl


reduce.o: reduce.c
	${CC} ${CFLAGS} ${DEFINE} -c reduce.c

lslinit.o: lslinit.c
	${CC} ${CFLAGS} ${DEFINE} -c lslinit.c

shift.o: shift.c
	${CC} ${CFLAGS} ${DEFINE} -c shift.c

depend:
	makedepend ${IFLAGS} ${SOURCES}

tidy:
	${RM} *~ core

clean: tidy
	${RM} ${OBJECTS} lsl Makefile.bak


# DO NOT DELETE THIS LINE -- make depend depends on it.


abstract.o: abstract.h error.h general.h list.h osdfiles.h parse.h
abstract.o: string2sym.h token.h tokentable.h /usr/include/bits/setjmp.h
abstract.o: /usr/include/bits/sigset.h /usr/include/features.h
abstract.o: /usr/include/gnu/stubs.h /usr/include/setjmp.h
abstract.o: /usr/include/sys/cdefs.h
checking.o: abstract.h checking.h error.h general.h list.h lsl.h operator.h
checking.o: osdfiles.h parse.h print.h prop.h propSet.h recheck.h renaming.h
checking.o: scan.h set.h source.h spec_stdlib.h string2sym.h symtable.h
checking.o: token.h tokentable.h unparse.h /usr/include/bits/pthreadtypes.h
checking.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
checking.o: /usr/include/bits/sigset.h /usr/include/bits/stdio_lim.h
checking.o: /usr/include/bits/types.h /usr/include/bits/wchar.h
checking.o: /usr/include/features.h /usr/include/_G_config.h
checking.o: /usr/include/gconv.h /usr/include/gnu/stubs.h /usr/include/libio.h
checking.o: /usr/include/setjmp.h /usr/include/stdio.h /usr/include/string.h
checking.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
checking.o: /usr/include/xlocale.h
checking.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
checking.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
checking.o: vartable.h
error.o: error.h general.h osdfiles.h string2sym.h token.h
error.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
error.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
error.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
error.o: /usr/include/bits/wchar.h /usr/include/features.h
error.o: /usr/include/_G_config.h /usr/include/gconv.h
error.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
error.o: /usr/include/stdio.h /usr/include/sys/cdefs.h /usr/include/wchar.h
error.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
error.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
general.o: error.h general.h osdfiles.h spec_stdlib.h string2sym.h token.h
general.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
general.o: /usr/include/features.h /usr/include/gnu/stubs.h
general.o: /usr/include/setjmp.h /usr/include/string.h
general.o: /usr/include/sys/cdefs.h /usr/include/xlocale.h
general.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
list.o: general.h list.h osdfiles.h spec_stdlib.h
lsl2lp.o: abstract.h checking.h error.h general.h list.h lsl2lp.h lsl.h
lsl2lp.o: operator.h osdfiles.h parse.h print.h prop.h propSet.h renaming.h
lsl2lp.o: scan.h set.h source.h string2sym.h symtable.h token.h tokentable.h
lsl2lp.o: unparse.h /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
lsl2lp.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
lsl2lp.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
lsl2lp.o: /usr/include/bits/wchar.h /usr/include/features.h
lsl2lp.o: /usr/include/_G_config.h /usr/include/gconv.h
lsl2lp.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
lsl2lp.o: /usr/include/stdio.h /usr/include/sys/cdefs.h /usr/include/wchar.h
lsl2lp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
lsl2lp.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h vartable.h
lslinit.o: error.h general.h lslinit.h scan.h scanline.h shift.h source.h
lslinit.o: string2sym.h syntable.h token.h tokentable.h
lslinit.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
lslinit.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
lslinit.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
lslinit.o: /usr/include/bits/wchar.h /usr/include/features.h
lslinit.o: /usr/include/_G_config.h /usr/include/gconv.h
lslinit.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
lslinit.o: /usr/include/stdio.h /usr/include/string.h /usr/include/sys/cdefs.h
lslinit.o: /usr/include/wchar.h /usr/include/xlocale.h
lslinit.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
lslinit.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
lsl.o: abstract.h checking.h error.h general.h list.h lsl2lp.h lsl.h lslinit.h
lsl.o: operator.h osdfiles.h parse.h prop.h propSet.h renaming.h scan.h
lsl.o: scanline.h semantics.h set.h source.h string2sym.h symtable.h
lsl.o: syntable.h token.h tokentable.h unparse.h
lsl.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
lsl.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
lsl.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
lsl.o: /usr/include/bits/wchar.h /usr/include/features.h
lsl.o: /usr/include/_G_config.h /usr/include/gconv.h /usr/include/gnu/stubs.h
lsl.o: /usr/include/libio.h /usr/include/setjmp.h /usr/include/stdio.h
lsl.o: /usr/include/string.h /usr/include/sys/cdefs.h /usr/include/wchar.h
lsl.o: /usr/include/xlocale.h
lsl.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
lsl.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h vartable.h
operator.o: abstract.h error.h general.h list.h operator.h osdfiles.h parse.h
operator.o: print.h renaming.h set.h string2sym.h symtable.h token.h unparse.h
operator.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
operator.o: /usr/include/features.h /usr/include/gnu/stubs.h
operator.o: /usr/include/setjmp.h /usr/include/sys/cdefs.h vartable.h
osdfiles.o: general.h osdfiles.h spec_stdlib.h /usr/include/bits/endian.h
osdfiles.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
osdfiles.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
osdfiles.o: /usr/include/bits/stat.h /usr/include/bits/stdio_lim.h
osdfiles.o: /usr/include/bits/types.h /usr/include/bits/wchar.h
osdfiles.o: /usr/include/endian.h /usr/include/features.h
osdfiles.o: /usr/include/_G_config.h /usr/include/gconv.h
osdfiles.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/stdio.h
osdfiles.o: /usr/include/string.h /usr/include/sys/cdefs.h
osdfiles.o: /usr/include/sys/select.h /usr/include/sys/stat.h
osdfiles.o: /usr/include/sys/sysmacros.h /usr/include/sys/types.h
osdfiles.o: /usr/include/time.h /usr/include/wchar.h /usr/include/xlocale.h
osdfiles.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
osdfiles.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
parse.o: abstract.h error.h general.h list.h osdfiles.h parse.h reduce.h
parse.o: scan.h scanline.h shift.h source.h string2sym.h token.h tokentable.h
parse.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
parse.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
parse.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
parse.o: /usr/include/bits/wchar.h /usr/include/features.h
parse.o: /usr/include/_G_config.h /usr/include/gconv.h
parse.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
parse.o: /usr/include/stdio.h /usr/include/sys/cdefs.h /usr/include/wchar.h
parse.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
parse.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
print.o: error.h general.h lsl.h print.h string2sym.h token.h
print.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
print.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
print.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
print.o: /usr/include/bits/wchar.h /usr/include/features.h
print.o: /usr/include/_G_config.h /usr/include/gconv.h
print.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
print.o: /usr/include/stdio.h /usr/include/string.h /usr/include/sys/cdefs.h
print.o: /usr/include/wchar.h /usr/include/xlocale.h
print.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
print.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
prop.o: abstract.h checking.h error.h general.h list.h lsl.h operator.h
prop.o: osdfiles.h parse.h print.h prop.h propSet.h renaming.h set.h
prop.o: string2sym.h symtable.h token.h unparse.h
prop.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
prop.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
prop.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
prop.o: /usr/include/bits/wchar.h /usr/include/features.h
prop.o: /usr/include/_G_config.h /usr/include/gconv.h /usr/include/gnu/stubs.h
prop.o: /usr/include/libio.h /usr/include/setjmp.h /usr/include/stdio.h
prop.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
prop.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
prop.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h vartable.h
propSet.o: abstract.h checking.h error.h general.h list.h operator.h
propSet.o: osdfiles.h parse.h print.h prop.h propSet.h renaming.h set.h
propSet.o: spec_stdlib.h string2sym.h symtable.h token.h unparse.h
propSet.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
propSet.o: /usr/include/features.h /usr/include/gnu/stubs.h
propSet.o: /usr/include/setjmp.h /usr/include/sys/cdefs.h vartable.h
recheck.o: abstract.h checking.h error.h general.h list.h lsl.h operator.h
recheck.o: osdfiles.h parse.h prop.h propSet.h recheck.h renaming.h set.h
recheck.o: source.h string2sym.h symtable.h token.h tokentable.h
recheck.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
recheck.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
recheck.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
recheck.o: /usr/include/bits/wchar.h /usr/include/features.h
recheck.o: /usr/include/_G_config.h /usr/include/gconv.h
recheck.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
recheck.o: /usr/include/stdio.h /usr/include/sys/cdefs.h /usr/include/wchar.h
recheck.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
recheck.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h vartable.h
reduce.o: abstract.h general.h list.h osdfiles.h parse.h reduce.h set.h
reduce.o: shift.h string2sym.h token.h
renaming.o: abstract.h checking.h error.h general.h list.h operator.h
renaming.o: osdfiles.h parse.h print.h prop.h propSet.h renaming.h set.h
renaming.o: spec_stdlib.h string2sym.h symtable.h token.h unparse.h
renaming.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
renaming.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
renaming.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
renaming.o: /usr/include/bits/wchar.h /usr/include/features.h
renaming.o: /usr/include/_G_config.h /usr/include/gconv.h
renaming.o: /usr/include/gnu/stubs.h /usr/include/libio.h
renaming.o: /usr/include/setjmp.h /usr/include/stdio.h
renaming.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
renaming.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
renaming.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
renaming.o: vartable.h
scanline.o: error.h general.h lsl.h scan.h scanline.h source.h string2sym.h
scanline.o: syntable.h token.h tokentable.h /usr/include/bits/pthreadtypes.h
scanline.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
scanline.o: /usr/include/bits/sigset.h /usr/include/bits/stdio_lim.h
scanline.o: /usr/include/bits/types.h /usr/include/bits/wchar.h
scanline.o: /usr/include/features.h /usr/include/_G_config.h
scanline.o: /usr/include/gconv.h /usr/include/gnu/stubs.h /usr/include/libio.h
scanline.o: /usr/include/setjmp.h /usr/include/stdio.h /usr/include/string.h
scanline.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
scanline.o: /usr/include/xlocale.h
scanline.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
scanline.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
scan.o: error.h general.h scan.h scanline.h source.h string2sym.h token.h
scan.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
scan.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
scan.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
scan.o: /usr/include/bits/wchar.h /usr/include/features.h
scan.o: /usr/include/_G_config.h /usr/include/gconv.h /usr/include/gnu/stubs.h
scan.o: /usr/include/libio.h /usr/include/setjmp.h /usr/include/stdio.h
scan.o: /usr/include/string.h /usr/include/sys/cdefs.h /usr/include/wchar.h
scan.o: /usr/include/xlocale.h
scan.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
scan.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
semantics.o: abstract.h checking.h error.h general.h list.h lsl.h operator.h
semantics.o: osdfiles.h parse.h prop.h propSet.h renaming.h semantics.h set.h
semantics.o: source.h string2sym.h symtable.h token.h tokentable.h
semantics.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
semantics.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
semantics.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
semantics.o: /usr/include/bits/wchar.h /usr/include/features.h
semantics.o: /usr/include/_G_config.h /usr/include/gconv.h
semantics.o: /usr/include/gnu/stubs.h /usr/include/libio.h
semantics.o: /usr/include/setjmp.h /usr/include/stdio.h
semantics.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
semantics.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
semantics.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
semantics.o: vartable.h
set.o: general.h list.h osdfiles.h set.h spec_stdlib.h
shift.o: error.h general.h shift.h string2sym.h token.h
shift.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
shift.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
shift.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
shift.o: /usr/include/bits/wchar.h /usr/include/features.h
shift.o: /usr/include/_G_config.h /usr/include/gconv.h
shift.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
shift.o: /usr/include/stdio.h /usr/include/sys/cdefs.h /usr/include/wchar.h
shift.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
shift.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
source.o: error.h general.h osdfiles.h source.h spec_stdlib.h string2sym.h
source.o: token.h /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
source.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
source.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
source.o: /usr/include/bits/wchar.h /usr/include/features.h
source.o: /usr/include/_G_config.h /usr/include/gconv.h
source.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
source.o: /usr/include/stdio.h /usr/include/string.h /usr/include/sys/cdefs.h
source.o: /usr/include/wchar.h /usr/include/xlocale.h
source.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
source.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
string2sym.o: error.h general.h string2sym.h token.h
string2sym.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
string2sym.o: /usr/include/features.h /usr/include/gnu/stubs.h
string2sym.o: /usr/include/setjmp.h /usr/include/sys/cdefs.h
symtable.o: abstract.h error.h general.h list.h operator.h osdfiles.h parse.h
symtable.o: print.h renaming.h set.h spec_stdlib.h string2sym.h symtable.h
symtable.o: token.h tokentable.h unparse.h /usr/include/bits/pthreadtypes.h
symtable.o: /usr/include/bits/sched.h /usr/include/bits/setjmp.h
symtable.o: /usr/include/bits/sigset.h /usr/include/bits/stdio_lim.h
symtable.o: /usr/include/bits/types.h /usr/include/bits/wchar.h
symtable.o: /usr/include/features.h /usr/include/_G_config.h
symtable.o: /usr/include/gconv.h /usr/include/gnu/stubs.h /usr/include/libio.h
symtable.o: /usr/include/setjmp.h /usr/include/stdio.h /usr/include/string.h
symtable.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
symtable.o: /usr/include/xlocale.h
symtable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
symtable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
symtable.o: vartable.h
syntable.o: error.h general.h string2sym.h syntable.h token.h tokentable.h
syntable.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
syntable.o: /usr/include/features.h /usr/include/gnu/stubs.h
syntable.o: /usr/include/setjmp.h /usr/include/sys/cdefs.h
token.o: general.h string2sym.h token.h
tokentable.o: error.h general.h string2sym.h token.h tokentable.h
tokentable.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
tokentable.o: /usr/include/features.h /usr/include/gnu/stubs.h
tokentable.o: /usr/include/setjmp.h /usr/include/sys/cdefs.h
unparse.o: abstract.h checking.h error.h general.h list.h lsl.h operator.h
unparse.o: osdfiles.h parse.h print.h prop.h propSet.h renaming.h set.h
unparse.o: string2sym.h symtable.h token.h unparse.h
unparse.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
unparse.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
unparse.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
unparse.o: /usr/include/bits/wchar.h /usr/include/features.h
unparse.o: /usr/include/_G_config.h /usr/include/gconv.h
unparse.o: /usr/include/gnu/stubs.h /usr/include/libio.h /usr/include/setjmp.h
unparse.o: /usr/include/stdio.h /usr/include/string.h /usr/include/sys/cdefs.h
unparse.o: /usr/include/wchar.h /usr/include/xlocale.h
unparse.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
unparse.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h vartable.h
vartable.o: abstract.h error.h general.h list.h operator.h osdfiles.h parse.h
vartable.o: print.h recheck.h renaming.h set.h spec_stdlib.h string2sym.h
vartable.o: symtable.h token.h tokentable.h unparse.h
vartable.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
vartable.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
vartable.o: /usr/include/bits/stdio_lim.h /usr/include/bits/types.h
vartable.o: /usr/include/bits/wchar.h /usr/include/features.h
vartable.o: /usr/include/_G_config.h /usr/include/gconv.h
vartable.o: /usr/include/gnu/stubs.h /usr/include/libio.h
vartable.o: /usr/include/setjmp.h /usr/include/stdio.h
vartable.o: /usr/include/sys/cdefs.h /usr/include/wchar.h
vartable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
vartable.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
vartable.o: vartable.h
