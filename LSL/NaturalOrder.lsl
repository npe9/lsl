NaturalOrder(N): trait
  % The natural numbers with an ordering
  includes
    Enumerable(N),
    TotalOrder(N)
  asserts \forall x: N
    x < succ(x)
  implies
    Infinite(N)
    \forall x, y: N 
      0 <= x;
      x < succ(y) <=> x <= y;
      succ(x) < succ(y) <=> x < y
    converts <=, >=, <, >
