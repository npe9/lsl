ArraySlice2(E, I1, I2): trait
  % A two-dimensional array treated as a vector of vectors
  includes 
    Array1(E, I2),
    Array1(Array[I2, E], I1)
  introduces
    assign:     Array[I1, Array[I2, E]], I1, I2, E -> Array[I1, Array[I2, E]]
    __[__, __]: Array[I1, Array[I2, E]], I1, I2    -> E
  asserts
    \forall a: Array[I1, Array[I2, E]], i1: I1, i2: I2, e: E
      a[i1, i2] = (a[i1])[i2];
      assign(a, i1, i2, e) =
        assign(a, i1, assign(a[i1], i2, e))
