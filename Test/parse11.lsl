% Trait with renamings

parse11: trait
	includes parse11a(a for 0, b for 1)
	includes parse11a(Int, succ, Int for M, * for +:N,N->M)
	introduces 2, 3, 4:->Int
	asserts equations
		s(a) = b		\eqsep
		succ(0) = 1		\eqsep
		succ(2) = 3		\eqsep
		2*2 = 4			\eqsep
		a+ = b
