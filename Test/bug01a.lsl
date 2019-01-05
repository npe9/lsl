% Trait with errors when B is renamed to A

bug01a: trait
	introduces 
		c: -> A
		d: -> B
	asserts \forall c:B c:B = c:B		% variable/constant conflict

