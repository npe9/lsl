IntegerAndNatural(Int, N): trait
  % Conversions between Int's and N's
  includes
    Integer(Int),
    Natural(N)
  introduces
    int: N   -> Int
    nat: Int -> N
  asserts \forall n: N
    int(0) = 0;
    int(succ(n)) = succ(int(n));
    nat(int(n)) = n
