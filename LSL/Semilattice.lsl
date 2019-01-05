Semilattice(T): trait
  assumes PartialOrder
  includes GreatestLowerBound
  introduces
    \bot:          -> T
    __\lub __: T, T -> T
  asserts \forall x, y, z: T
    \bot <= x;
    x \lub y = y \lub x;
    x \glb y = y \glb x;
    x <= (x \lub y);
    (x <= z /\ y <= z) => (x \lub y) <= z
  implies 
    AbelianMonoid(\lub for \circ, \bot for unit),
    AbelianSemigroup(\glb for \circ)
