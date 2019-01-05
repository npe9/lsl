IntegerAndPositive(Int, P): trait
  % Conversions between Int's and P's
  includes
    Integer(Int),
    Positive(P)
  introduces
    int: P   -> Int
    pos: Int -> P
  asserts \forall p: P
    int(1) = 1;
    int(succ(p)) = succ(int(p));
    pos(int(p)) = p
