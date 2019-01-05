% Trait with inclusions

parse10: trait
	includes parse10a, parse10a(Int, succ)
	includes parse10a(N, .next)
	includes parse10b(++, --, **)
	includes parse10b(R for N)
	introduces
		__/__: N, N -> R
		__/__: R, R -> R
		0, 1: 	    -> R
	asserts equations
		s(0) = 1		\eqsep
		succ(0) = 1		\eqsep
		0.next = 1		\eqsep
		0++ = 1	 		\eqsep
		--1 = 0			\eqsep
		0**0 = 0		\eqsep
		1/1:R = 1		\eqsep
		1:R/1 = 1
