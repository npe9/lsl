RightInverse: trait
  assumes RightIdentity
  introduces __\inv: T -> T
  asserts \forall x: T
    x \circ (x\inv) = unit
