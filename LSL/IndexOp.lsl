IndexOp(C): trait
  % Select the i-th  element in the container
  % (in enumeration order).
  assumes Integer, Container
  introduces __[__]: C, Int -> E
  asserts \forall c: C, i: Int
    c[0] = head(c);
    i >= 0 => c[i+1] = tail(c)[i]
