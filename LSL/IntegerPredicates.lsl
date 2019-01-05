IntegerPredicates(Int): trait
  % Frequently used subranges of the integers
  assumes Integer
  introduces
    InRange: Int, Int, Int -> Bool
    Natural, Positive, Signed, Unsigned: Int -> Bool
    maxSigned, maxUnsigned: -> Int
  asserts forall n, low, high: Int
    InRange(n, low, high) <=> low <= n /\ n <= high;
    Natural(n) <=> n >= 0;
    Positive(n) <=> n > 0;
    Signed(n) <=>
      InRange(n, -succ(maxSigned), maxSigned);
    Unsigned(n) <=> InRange(n, 0, maxUnsigned)
  implies \forall n: Int
    Positive(n) => Natural(n);
    Unsigned(n) => Natural(n)
