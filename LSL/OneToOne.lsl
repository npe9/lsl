OneToOne(\rel): trait
  introduces __ \rel __: T, T -> Bool
  asserts \forall x, y, z: T
    (x \rel y /\ x \rel z) => y = z;
    (x \rel z /\ y \rel z) => x = y;