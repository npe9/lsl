Infinite(N): trait
  introduces
    0:      -> N
    succ: N -> N
  asserts \forall x, y: N
    succ(x) \neq 0;
    succ(x) = succ(y) <=> x = y
