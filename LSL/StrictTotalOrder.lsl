StrictTotalOrder(<, T): trait
  includes StrictPartialOrder
  asserts \forall x, y: T
    x < y \/ y < x \/ x = y
