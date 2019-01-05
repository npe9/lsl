Equality(T): trait
  % This trait is given for documentation only.
  % It is implicit in LSL.
  introduces __ = __, __ \neq __: T, T -> Bool
  asserts
    sort T partitioned by =
    \forall x, y, z: T
      x = x;
      x = y <=> y = x;
      (x = y /\ y = z) => x = z;
      x \neq y <=> ~(x = y)
  implies Equivalence(= for \equiv)
