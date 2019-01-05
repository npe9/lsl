% Parametrized trait

parse09 (N, +:N,N->N, *): trait
	introduces
		+__: 		A	-> Bool
		__+__, __*__:	N,N	-> N
	asserts \forall i, j: N, a: A
		i + j = j + i		\eqsep
		+a
	implies
		parse09,
		parse09(A, *, +),
		parse09(+ for *)
