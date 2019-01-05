% Trait with many opForms

parse05: trait
	introduces
		0, 1: 					-> N
		s, +__, -__, __!:	N 		-> N
		__+__, __*__: 		N, N 		-> N
		__<__: 			N, N		-> Bool
		__[__]: 		A, N 		-> N
		{}:           				-> N
		__{}, {__}, {}__:      	N 		-> N
		__{}__:       		N, N  		-> N
		__{__}__:     		N, N, N  	-> N
		__{__,__}__:  		N, N, N, N  	-> N
		__{__,__,__}: 		N, N, N, N  	-> N
		__.first, __.second: 	N 		-> N
	asserts \forall i, j, k: N, a: A
		i = i						\eqsep
		1 = s(0)					\eqsep
		i + s(j) = s(i+j)				\eqsep
		0 < 1						\eqsep
		(i < j /\ j < k) => i < k			\eqsep
		+i = i						\eqsep
		- -i = i					\eqsep
		0! = 1						\eqsep
		(i+1)! = (i+1) * (i!)				\eqsep
		a[0] = 0					\eqsep
		a[i+j] + a[i] = k = i < k			\eqsep
		{} = 0						\eqsep
		0{0,0}0 = 1					\eqsep
                ({}){{},{}}({}) = {{}}				\eqsep
		0:N = 0:N:N					\eqsep
		0.first.second = 0:N.first:N.second:N 		\eqsep
		a[0]:N + 0 = 0



