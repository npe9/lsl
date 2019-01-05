%%% Axioms for trait semLit1

%% Operator declarations

declare sorts
  N
  ..

declare operators
  0: -> N
  , 123: -> N
  , 456: -> N
  , 789: -> N
  , 10: -> N
  , 010: -> N
  , succ: N -> N
  , times10plus: N, N -> N
  , 1: -> N
  , 2: -> N
  , 3: -> N
  , 4: -> N
  , 5: -> N
  , 6: -> N
  , 7: -> N
  , 8: -> N
  , 9: -> N
  , 12: -> N
  , 45: -> N
  , 78: -> N
  , 01: -> N
  ..

% Numerals

set name DecimalLiterals
set activity off
set immunity on
assert
  123 = times10plus(12, 3);
  12 = times10plus(1, 2);
  456 = times10plus(45, 6);
  45 = times10plus(4, 5);
  789 = times10plus(78, 9);
  78 = times10plus(7, 8);
  10 = times10plus(1, 0);
  010 = times10plus(01, 0);
  01 = times10plus(0, 1);
  ..
set activity on
set immunity off

set automatic-ordering off

%% Assertions

declare variables
  i: N
  j: N
  ..


% main trait: semLit1

set name semLit1

assert
  (0 = 0)
  ;(123 = 456)
  ;(789 = 0)
  ;(10 = 010)
  ..

% subtrait 1: DecimalLiterals

set name DecimalLiterals

push-settings
set activity off
set immunity on

assert
  (times10plus(0, j) = j)
  ;(times10plus(i, succ(j)) = succ(times10plus(i, j)))
  ;(times10plus(succ(i), 0) = succ(times10plus(i, 9)))
  ;(1 = succ(0))
  ;(2 = succ(1))
  ;(3 = succ(2))
  ;(4 = succ(3))
  ;(5 = succ(4))
  ;(6 = succ(5))
  ;(7 = succ(6))
  ;(8 = succ(7))
  ;(9 = succ(8))
  ..

pop-settings

set automatic-ordering on

