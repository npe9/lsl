ArithOps(N): trait
  % Defines operators div and mod relative to + and *
  % for positive denominators
  assumes TotalOrder(N)
  includes Multiplication(N)
  introduces div, mod: N, N -> N
  asserts \forall x, y: N
    y > 0 => 0 <= mod(x, y);
    y > 0 => mod(x, y) < y;
    y > 0 => mod(x, y) + (div(x, y) * y) = x
