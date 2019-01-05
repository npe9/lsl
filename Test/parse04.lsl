% Trait with equations

parse04: trait
	introduces 
		0, 1:->Nat
		s:Nat->Nat
	asserts equations 
		1 = s(0)		\eqsep
		~(0 = s(0))		\eqsep
		true			\eqsep
		~false

