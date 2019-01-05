% Test semantics of DecimalLiterals

semLit1: trait
  includes DecimalLiterals
  asserts equations
    0 = 0;
    123 = 456;
    789 = 0;
    10 = 010

