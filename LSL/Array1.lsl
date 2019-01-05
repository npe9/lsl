Array1(E, I): trait
  % Basic one-dimensional array operators
  introduces
    assign: Array[I, E], I, E -> Array[I, E]
    __[__]: Array[I, E], I    -> E
  asserts
    \forall a: Array[I, E], i, j: I, e: E
      assign(a, i, e)[j] = (if i = j then e else a[j])
