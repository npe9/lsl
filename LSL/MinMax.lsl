MinMax(T): trait
  assumes TotalOrder
  introduces
    min, max: T, T -> T
  asserts \forall x, y: T
    min(x, y) = (if x <= y then x else y);
    max(x, y) = (if x >= y then x else y)
  implies
    AC(min, T),
    AC(max, T)
    converts min, max
