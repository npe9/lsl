Field: trait
  includes
    RingWithUnit,
    Abelian(* for \circ)
  introduces __\inv: T -> T
  asserts \forall x: T
    x \neq 0 => x * (x\inv) = 1
