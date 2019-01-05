ComposeMaps(D, T, R): trait
  % Map[T, R] \circ Map[D, T] is a map from D to R.
  assumes
    FiniteMap(T, R),
    FiniteMap(D, T)
  includes FiniteMap(D, R)
  introduces __ \circ __: Map[T, R], Map[D, T] -> Map[D, R]
  asserts \forall m1: Map[T, R], m2: Map[D, T], d: D
    apply(m1 \circ m2, d) = apply(m1, apply(m2, d));
    defined(m1 \circ m2, d) =
      defined(m2, d) /\ defined(m1, apply(m2, d))
