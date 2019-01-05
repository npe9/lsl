Group: trait
  introduces
    __\circ __: T, T -> T
    unit:            -> T
    __\inv:     T    -> T
  asserts \forall x, y, z: T
    (x \circ y) \circ z = x \circ (y \circ z);
    unit \circ x = x;
    (x\inv) \circ x = unit;
  implies Monoid, Inverse
