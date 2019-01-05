%%% Theorems from trait semLit2

execute semLit2_Axioms

declare variables
  i: N
  j: N
  ..

%% Theorems


% subtrait 2: DecimalLiterals

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

