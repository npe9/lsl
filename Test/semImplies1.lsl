% Test collection of implications (buggy)

semImplies1(a, b, c): trait
  introduces a, b, c: -> Bool
  implies
    semImplies1a(a),	% a
    semImplies1b(b, c) 	% b, c
