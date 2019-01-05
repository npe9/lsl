% Bad parametrized trait

check09 (N, +:N,N,N->N, *, f): trait			% no op, op or sort
	introduces
		+__: 		A	-> Bool
		__+__, __*__:	N,N	-> N
	asserts \forall i, j: N, a: A
		i + j = j + i		\eqsep
		+a
	implies
		check09					% trait has errors
