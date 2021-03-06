% Trait with consequences

parse08: trait
	introduces 
		0, 1: 				-> N
		s: 			N	-> N
		__+__, __*__, __/__: 	N, N 	-> N
		__<__:			N, N	-> Bool
	asserts
		N generated by 0, s
		N partitioned by <
		\forall i, j: N
			1 = s(0)			\eqsep
			i + 0 = i			\eqsep
			i + s(j) = s(i + j)		\eqsep
			i * 0 = 0			\eqsep
			i * s(j) = (i*j) + i		\eqsep
			j \neq 0 => j*(i/j) = i
	implies 
		N generated by 0, 1, +
		\forall i, j: N
			i + j = j + i			\eqsep
			i * j = j * i			\eqsep
			\not(s(i) = 0)
		converts *, +
                converts / exempting \forall i:N i/0

