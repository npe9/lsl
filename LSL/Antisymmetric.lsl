Antisymmetric(\rel): trait
  introduces __ \rel __: T, T -> Bool
  asserts \forall x, y: T
    (x \rel y /\ y \rel x) => x = y
