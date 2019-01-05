set script semLit3
set log semLit3

%%% Proof Obligations for trait semLit3

execute semLit3_Axioms

%% Inherited assumptions

declare variables
  i: N
  j: N
  ..


% subtrait 1.1: DecimalLiterals

set name DecimalLiteralsTheorem

prove
  (times10plus(0, j) = j)
  ..
prove
  (times10plus(i, succ(j)) = succ(times10plus(i, j)))
  ..
prove
  (times10plus(succ(i), 0) = succ(times10plus(i, 9)))
  ..
prove
  (1 = succ(0))
  ..
prove
  (2 = succ(1))
  ..
prove
  (3 = succ(2))
  ..
prove
  (4 = succ(3))
  ..
prove
  (5 = succ(4))
  ..
prove
  (6 = succ(5))
  ..
prove
  (7 = succ(6))
  ..
prove
  (8 = succ(7))
  ..
prove
  (9 = succ(8))
  ..

