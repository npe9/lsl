Multiplication(N): trait
  % Define the operator * in terms of 0, succ, and +
  includes
    AbelianMonoid(* for \circ, 1 for unit, N for T),
    Addition(N)
  introduces
    1:          -> N
    __*__: N, N -> N
  asserts \forall x, y: N
    1 = succ(0);
    x * 0 = 0;
    x * succ(y) = x + (x * y)
