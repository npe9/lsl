GreatestLowerBound(T): trait
  introduces 
    __ <= __:   T, T -> Bool
    __ \glb __: T, T -> T
  asserts \forall x, y, z: T
    (x \glb y) <= x;
    (x \glb y) <= y;
    (z <= x /\ z <= y) => z <= (x \glb y)
