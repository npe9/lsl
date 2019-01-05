StrictPartialOrder(<, T): trait
  includes Irreflexive(<), Transitive(<)
  implies
    Asymmetric(<)
    \forall x, y, z: T
      ~(x < x);
      (x < y /\ y < z) => x < z
