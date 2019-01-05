IsTO(<=, T): trait
  % <= is a total order on T
  introduces __<=__: T, T -> Bool
  asserts \forall x, y, z: T
    x <= x;
    (x <= y /\ y <= z) => x <= z;
    x <= y /\ y <= x <=> x = y;
    x <= y \/ y <= x
  implies IsPO, TotalPreOrder
