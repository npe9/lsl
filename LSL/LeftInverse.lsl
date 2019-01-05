LeftInverse: trait
  assumes LeftIdentity
  introduces __\inv: T -> T
  asserts \forall x: T
    (x\inv) \circ x = unit
