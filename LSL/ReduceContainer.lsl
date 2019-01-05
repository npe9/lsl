ReduceContainer(unit, \circ): trait
  % Insert the operator in enumeration order.
  assumes Container
  introduces 
    unit:             -> E
    __ \circ __: E, E -> E
    reduce:      C    -> E
  asserts \forall c: C
    reduce(c) =
      (if c = empty then unit
       else head(c) \circ reduce(tail(c)))
  implies converts reduce
