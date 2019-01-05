Array2(E, I1, I2): trait
  % Basic two-dimensional array operators
  introduces
    assign:     Array[I1, I2, E], I1, I2, E -> Array[I1, I2, E]
    __[__, __]: Array[I1, I2, E], I1, I2    -> E
  asserts
    \forall a: Array[I1, I2, E], i1, j1: I1, i2, j2: I2, e: E
      assign(a, i1, i2, e)[j1, j2] =
        (if i1 = j1 /\ i2 = j2 then e else a[j1, j2])
