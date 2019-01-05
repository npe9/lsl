DerivedOrders(T): trait
  % Defines any three of the comparison operators,
  % given the fourth
  introduces
    __<=__, __>=__, __<__, __>__: T, T -> Bool
  asserts \forall x, y: T
    x < y <=> x <= y /\ ~(x = y);
    x >= y <=> y <= x;
    x > y <=> y < x
  implies 
    \forall x, y: T
      x <= y <=> x < y \/ x = y
    converts >=, <, >
    converts <=, <, >
    converts <=, >=, >
    converts <=, >=, <
