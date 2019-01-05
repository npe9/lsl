LeftDistributive(+, *, T): trait
  introduces
    __+__, __*__: T, T -> T
  asserts \forall x, y, z: T
    x * (y + z) = (x * y) + (x * z)
