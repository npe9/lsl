# Makefile for testing the LSL checker

MV		= /bin/mv -f
RM		= /bin/rm -f
ECHO		= echo
DIFF		= diff

CHECKER		= ../Code/lsl
LIBDIR		= ../LSL
INITFILE	= ../LSL/lslinit.lsi

RESULTDIR	= ./Results
PREVIOUSDIR	= ./Previous

# Files to test known bugs in the LSL checker

LSLBUG_TESTS	= bug01.lsl

# Files to test LSL parser

GOOD_PARSES	= parse01.lsl parse02.lsl parse03.lsl parse04.lsl parse05.lsl \
		  parse06.lsl parse07.lsl parse08.lsl parse09.lsl parse10.lsl \
		  parse11.lsl parse12.lsl parse13.lsl parse14.lsl

BAD_PARSES	= parseError01.lsl parseError02.lsl

# Files to test LSL context-sensitive checking

CHECK_TESTS	= check03.lsl check04.lsl check05.lsl check06.lsl check07.lsl \
		  check08.lsl check09.lsl check10.lsl check11.lsl check12.lsl \
		  check13.lsl check14.lsl


# Files to test formulation of LSL semantics as input to LP

SEMANTIC_TESTS	= semAssumes1.lsl semCvts.lsl semDup1.lsl semEnum.lsl 	  \
		  semImplies1.lsl semImplies2.lsl semLit1.lsl semLit2.lsl \
		  semLit3.lsl semQual.lsl semTuple.lsl semUnion.lsl


# Files from the LSL Handbook

HANDBOOK_TESTS	= 							  \
	${LIBDIR}/AC.lsl		 ${LIBDIR}/Abelian.lsl	      	  \
	${LIBDIR}/AbelianGroup.lsl	 ${LIBDIR}/AbelianMonoid.lsl  	  \
	${LIBDIR}/AbelianSemigroup.lsl 	 ${LIBDIR}/Addition.lsl	      	  \
	${LIBDIR}/Antisymmetric.lsl	 ${LIBDIR}/ArithOps.lsl       	  \
	${LIBDIR}/Array1.lsl		 ${LIBDIR}/Array2.lsl         	  \
	${LIBDIR}/ArraySlice2.lsl	 ${LIBDIR}/Associative.lsl    	  \
	${LIBDIR}/Asymmetric.lsl	 ${LIBDIR}/Bag.lsl		  \
	${LIBDIR}/BagBasics.lsl		 ${LIBDIR}/BinaryTree.lsl	  \
	${LIBDIR}/Boolean.lsl		 ${LIBDIR}/ChoiceBag.lsl	  \
	${LIBDIR}/ChoiceSet.lsl		 ${LIBDIR}/CoerceContainer.lsl	  \
	${LIBDIR}/Commutative.lsl	 ${LIBDIR}/ComposeMaps.lsl	  \
	${LIBDIR}/Conditional.lsl	 ${LIBDIR}/Container.lsl	  \
	${LIBDIR}/DecimalLiterals.lsl	 ${LIBDIR}/Deque.lsl		  \
	${LIBDIR}/DerivedOrders.lsl	 ${LIBDIR}/Distributive.lsl	  \
	${LIBDIR}/ElementTest.lsl	 ${LIBDIR}/Enumerable.lsl	  \
	${LIBDIR}/Enumeration.lsl	 ${LIBDIR}/Equality.lsl		  \
	${LIBDIR}/Equivalence.lsl	 ${LIBDIR}/Exponentiation.lsl	  \
	${LIBDIR}/FPAssumptions.lsl	 ${LIBDIR}/Field.lsl		  \
	${LIBDIR}/FiniteMap.lsl		 ${LIBDIR}/FloatingPoint.lsl	  \
	${LIBDIR}/Functional.lsl	 ${LIBDIR}/Graph.lsl		  \
	${LIBDIR}/GreatestLowerBound.lsl ${LIBDIR}/Group.lsl		  \
	${LIBDIR}/Idempotent.lsl	 ${LIBDIR}/Identity.lsl		  \
	${LIBDIR}/IndexOp.lsl		 ${LIBDIR}/Infinite.lsl		  \
	${LIBDIR}/InsertGenerated.lsl	 ${LIBDIR}/IntCycle.lsl		  \
	${LIBDIR}/Integer.lsl		 ${LIBDIR}/IntegerAndNatural.lsl  \
	${LIBDIR}/IntegerAndPositive.lsl ${LIBDIR}/IntegerPredicates.lsl  \
	${LIBDIR}/Inverse.lsl		 ${LIBDIR}/Involutive.lsl	  \
	${LIBDIR}/Irreflexive.lsl	 ${LIBDIR}/IsPO.lsl		  \
	${LIBDIR}/IsTO.lsl		 ${LIBDIR}/JoinOp.lsl		  \
	${LIBDIR}/Lattice.lsl		 ${LIBDIR}/LeftDistributive.lsl	  \
	${LIBDIR}/LeftIdentity.lsl	 ${LIBDIR}/LeftInverse.lsl	  \
	${LIBDIR}/LexicographicOrder.lsl ${LIBDIR}/List.lsl		  \
	${LIBDIR}/ListStructure.lsl	 ${LIBDIR}/ListStructureOps.lsl	  \
	${LIBDIR}/MemberOp.lsl		 ${LIBDIR}/MinMax.lsl		  \
	${LIBDIR}/Monoid.lsl		 ${LIBDIR}/Multiplication.lsl	  \
	${LIBDIR}/Natural.lsl		 ${LIBDIR}/NaturalOrder.lsl	  \
	${LIBDIR}/OneToOne.lsl		 ${LIBDIR}/PairwiseExtension.lsl  \
	${LIBDIR}/PartialOrder.lsl	 ${LIBDIR}/Permutation.lsl	  \
	${LIBDIR}/PointwiseImage.lsl	 ${LIBDIR}/Positive.lsl		  \
	${LIBDIR}/PreOrder.lsl		 ${LIBDIR}/PriorityQueue.lsl	  \
	${LIBDIR}/Queue.lsl		 ${LIBDIR}/Rational.lsl		  \
	${LIBDIR}/ReduceContainer.lsl	 ${LIBDIR}/Reflexive.lsl	  \
	${LIBDIR}/Relation.lsl		 ${LIBDIR}/RelationBasics.lsl	  \
	${LIBDIR}/RelationOps.lsl	 ${LIBDIR}/RelationPredicates.lsl \
	${LIBDIR}/ReverseOp.lsl		 ${LIBDIR}/RightDistributive.lsl  \
	${LIBDIR}/RightIdentity.lsl	 ${LIBDIR}/RightInverse.lsl	  \
	${LIBDIR}/Ring.lsl		 ${LIBDIR}/RingWithUnit.lsl	  \
	${LIBDIR}/Semigroup.lsl		 ${LIBDIR}/Semilattice.lsl	  \
	${LIBDIR}/Sequence.lsl		 ${LIBDIR}/Set.lsl		  \
	${LIBDIR}/SetBasics.lsl		 ${LIBDIR}/SetToRelation.lsl	  \
	${LIBDIR}/SignedInt.lsl		 ${LIBDIR}/Stack.lsl		  \
	${LIBDIR}/StackBasics.lsl	 ${LIBDIR}/StrictPartialOrder.lsl \
	${LIBDIR}/StrictTotalOrder.lsl	 ${LIBDIR}/String.lsl		  \
	${LIBDIR}/Symmetric.lsl		 ${LIBDIR}/TotalOrder.lsl	  \
	${LIBDIR}/TotalPreOrder.lsl	 ${LIBDIR}/Transitive.lsl	  \
	${LIBDIR}/UnsignedInt.lsl


# Files used in other tests

SUBTRAITS	= parse10a.lsl parse10b.lsl parse11a.lsl check10a.lsl     \
		  check10b.lsl check10c.lsl check10d.lsl check10e.lsl     \
		  check13a.lsl check13a.lsl check14a.lsl bug01a.lsl	  \
		  semAssumes1a.lsl semAssumes1b.lsl semAssumes1c.lsl	  \
		  semAssumes1d.lsl semDup1a.lsl semDup1b.lsl semDup1c.lsl \
		  semImplies1a.lsl semImplies1b.lsl semLit3a.lsl


DISTRIBUTE	= Makefile ${LSLBUG_TESTS} ${GOOD_PARSES} ${BAD_PARSES}	\
		  ${CHECK_TESTS} ${HANDBOOK_TESTS} ${SEMANTIC_TESTS}	\
		  ${SUBTRAITS} ${INIT_TESTS}

COMMAND		= ${CHECKER} -i ${INITFILE}

.SILENT:
.IGNORE:
.SUFFIXES:
.SUFFIXES: .o .c

all: ${RESULTDIR}/lslbug.tst ${RESULTDIR}/parse.tst ${RESULTDIR}/check.tst \
     ${RESULTDIR}/handbook.tst ${RESULTDIR}/semantics.tst
#
	if (${DIFF} -r ${PREVIOUSDIR} ${RESULTDIR}); 			\
	   then ${ECHO} " ";						\
		${ECHO} "** Test System Passed **";			\
	   else	${ECHO} " ";						\
		${ECHO} "** Test System Failed **";			\
	fi

${RESULTDIR}/lslbug.tst: ${CHECKER} ${INITFILE} ${LSLBUG_TESTS}
#
	${ECHO} "Testing known bugs in LSL context-sensitive checks"
	make test INFILES="${LSLBUG_TESTS}"				\
	     	  OUTFILE=${RESULTDIR}/lslbug.tst

${RESULTDIR}/parse.tst: ${CHECKER} ${INITFILE} ${GOOD_PARSES} ${BAD_PARSES}
#
	${ECHO} "Testing LSL parser"
	make test INFILES="${GOOD_PARSES} ${BAD_PARSES}"		\
	     	  OUTFILE=${RESULTDIR}/parse.tst

${RESULTDIR}/check.tst: ${CHECKER} ${INITFILE} ${CHECK_TESTS}
#
	${ECHO} "Testing LSL context-sensitive checks"
	make test INFILES="${CHECK_TESTS}"				\
	     	  OUTFILE=${RESULTDIR}/check.tst


${RESULTDIR}/handbook.tst: ${CHECKER} ${INITFILE} ${HANDBOOK__TESTS}
#
	${ECHO} "Testing LSL handbook traits"
	make test INFILES="${HANDBOOK_TESTS}"				\
	     	  OUTFILE=${RESULTDIR}/handbook.tst

${RESULTDIR}/semantics.tst: ${CHECKER} ${INITFILE} ${SEMANTIC_TESTS}
#
	${RM} *_Axioms.lp *_Theorems.lp *_Checks.lp
	${ECHO} "Testing LP formulation of LSL semantics"
	make test INFILES="${SEMANTIC_TESTS}"				\
	     	  OUTFILE=${RESULTDIR}/semantics.tst			\
		  COMMAND="${COMMAND} -lp"
	${MV} *_Axioms.lp *_Theorems.lp *_Checks.lp ${RESULTDIR}

test:
	${ECHO} "**************************************"   > ${OUTFILE}
	${ECHO} "Testing ${COMMAND}" 			   >> ${OUTFILE}
	${ECHO} "**************************************"   >> ${OUTFILE}
	for i in ${INFILES}; do						\
		${ECHO} "************* Test input from $$i ***********" \
			>> ${OUTFILE};					\
		LARCH_PATH=".:${LIBDIR}" 				\
		${COMMAND} $$i >> ${OUTFILE};				\
		${ECHO} "************* End of input from $$i *********"	\
			>> ${OUTFILE};					\
	  done

distributionList:
	${ECHO} ${DISTRIBUTE}

clean:
	${RM} ${RESULTDIR}/*
	${RM} *_Axioms.lp *_Theorems.lp *_Checks.lp *~
