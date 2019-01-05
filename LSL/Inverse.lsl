Inverse: trait
  assumes Identity, Semigroup
  includes LeftInverse, RightInverse
  implies
    Involutive(__\inv for op)
    \forall x, y: T
      unit\inv = unit;
      (x \circ y)\inv = (y\inv) \circ (x\inv)
