Addition(N): trait
  % Define the operator + in terms of 0 and succ
  includes AbelianMonoid(+ for \circ, 0 for unit, N for T)
  introduces
    0: -> N
    succ: N -> N
    __+__: N, N -> N
  asserts \forall x, y: N
    x + 0 = x;
    x + succ(y) = succ(x + y)
