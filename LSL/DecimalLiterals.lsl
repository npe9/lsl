DecimalLiterals(N): trait
  % A built-in trait schema given here
  % for documentation only
  introduces
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 %, ...
      : -> N
    succ: N -> N
  asserts equations
    1 = succ(0);
    2 = succ(1);
    3 = succ(2);
  % ... as far as needed for any literals
  % of sort N appearing in the including trait