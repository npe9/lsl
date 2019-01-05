PreOrder(<=, T): trait
  includes Reflexive(<=), Transitive(<=)
  implies \forall x, y, z: T
    x <= x;
    (x <= y /\ y <= z) => x <= z
