semImplies1b(b, c): trait
  introduces b, c: -> Bool
  implies 
    semImplies1a(b), 		% b
    semImplies1(c, c, c)	% c
