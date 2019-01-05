Exponentiation(T): trait
  % Repeatedly apply an infix * operator
  assumes
    Enumerable(N),
    Monoid(* for \circ, 1 for unit)
  introduces __**__: T, N -> T
  asserts \forall x: T, y: N
    x**0 = 1;
    x**succ(y) = x * (x**y)
  implies \forall x: T
    x**succ(0) = x
