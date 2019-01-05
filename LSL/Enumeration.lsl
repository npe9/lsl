Enumeration(T): trait
  % Enumeration, comparison, and ordinal position 
  % operators, often used with "enumeration of"
  assumes Integer
  includes DerivedOrders
  introduces
    first, last:    -> T
    succ, pred: T   -> T
    ord:        T   -> Int
    val:        Int -> T
  asserts 
    sort T generated by first, succ
    sort T generated by last, pred
    \forall x, y: T
      ord(first) = 0;
      x \neq last => ord(succ(x)) = ord(x) + 1;
      x \neq last => pred(succ(x)) = x;
      val(ord(x)) = x;
      x <= y <=> ord(x) <= ord(y);
      x <= last
  implies
    TotalOrder
    sort T generated by val
    sort T partitioned by ord
    \forall x: T
      x \neq first => succ(pred(x)) = x;
      x \neq last => x < succ(x);
      first <= x;
      ord(x) >= 0
    converts
      first:->T, succ:T->T, pred:T->T, ord,
        <=:T,T->Bool, >=:T,T->Bool,
        <:T,T->Bool, >:T,T->Bool
      exempting succ(last), pred(first)