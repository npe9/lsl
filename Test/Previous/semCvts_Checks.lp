set script semCvts
set log semCvts

%%% Proof Obligations for trait semCvts

execute semCvts_Axioms

%% Conversions

freeze semCvts

%% converts a, .left exempting \forall p: Pair p.left

thaw semCvts

declare operators
  a': -> E
  , __.left': Pair -> E
  ..


% subtrait 0: semCvts (a': -> E for a: -> E, __.left': Pair -> E for __.left:
%   Pair -> E)

set name semCvts

assert
  sort E generated freely by a', b, c
  ;(succ(a')) = (b)
  ;([ _s0_.left', _s0_.right ]) = (_s0_)
  ;(([ _x1_, _x2_ ]).left') = (_x1_)
  ;(set_left(_s0_, _x1_).left') = (_x1_)
  ;(set_right(_s0_, _x2_).left') = (_s0_.left')
  ..

declare variables
  p: Pair
  _x1_: Pair
  ..

set name exemptions

assert
  p.left = p.left'
  ..

set name conversionChecks

prove (_x1_:Pair.left) = (_x1_:Pair.left')
qed

