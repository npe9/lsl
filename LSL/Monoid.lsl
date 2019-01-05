Monoid: trait
  introduces
     __\circ __: T, T -> T
    unit:             -> T
  asserts \forall x, y, z: T
    (x \circ y) \circ z = x \circ (y \circ z);
    unit \circ x = x;
    x \circ unit = x
  implies Semigroup, Identity
