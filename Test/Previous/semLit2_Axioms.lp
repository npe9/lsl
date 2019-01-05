%%% Axioms for trait semLit2

%% Operator declarations

declare sorts
  N
  ..

declare operators
  succ: N -> N
  , times10plus: N, N -> N
  , 0: -> N
  , 1: -> N
  , 2: -> N
  , 3: -> N
  , 4: -> N
  , 5: -> N
  , 6: -> N
  , 7: -> N
  , 8: -> N
  , 9: -> N
  ..

set automatic-ordering off

%% Assertions

declare variables
  i: N
  j: N
  ..


% main trait: semLit2


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

