FPAssumptions(smallest, largest, gap, rational): trait
  includes Rational
  introduces
    smallest, largest, gap:   -> Q
    rational:               F -> Q
    float:                  Q -> F
    0, 1:                     -> F
  asserts \forall f: F
    smallest > 0;
    largest > smallest;
    rational(0) = 0;
    rational(1) = 1;
    rational(f) \neq 0 => abs(rational(f)) >= smallest;
    rational(f) <= largest;
    gap > 0;
    float(rational(f)) = f
