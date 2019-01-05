IsPO(<=, T): trait
  % <= is a partial order on T
  introduces __<=__: T, T -> Bool
  asserts \forall x, y, z: T    
    x <= x;
    (x <= y /\ y <= z) => x <= z;
    x <= y /\ y <= x <=> x = y
  implies
    Antisymmetric(<=),
    PreOrder,
    Reflexive(<=),
    Transitive(<=)
    sort T partitioned by <=
