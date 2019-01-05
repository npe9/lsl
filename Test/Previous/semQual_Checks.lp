set script semQual
set log semQual

%%% Proof Obligations for trait semQual

execute semQual_Axioms

%% Implications

declare variables
  x: B
  ..


% main trait: semQual

set name semQualTheorem

prove
  (x:B = x:B)
  ..

