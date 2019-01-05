%%% Axioms for trait semCvts

%% Operator declarations

declare sorts
  E, Pair
  ..

declare operators
  a: -> E
  , b: -> E
  , c: -> E
  , __.left: Pair -> E
  , __.right: Pair -> E
  , f: E, E -> E
  , __ \op __: E, E -> E
  , succ: E -> E
  , set_left: Pair, E -> Pair
  , set_right: Pair, E -> Pair
  , [ __ , __ ]: E, E -> Pair
  ..

set automatic-ordering off

%% Assertions

declare variables
  _s0_: Pair
  _x1_: E
  _y1_: E
  _x2_: E
  _y2_: E
  ..


% main trait: semCvts

set name semCvts

assert
  sort E generated freely by a, b, c
  ;(succ(a)) = (b)
  ;(succ(b)) = (c)
  ;sort Pair generated freely by [ __ , __ ]
  ;([ _s0_.left, _s0_.right ]) = (_s0_)
  ;(([ _x1_, _x2_ ]).left) = (_x1_)
  ;(set_left(_s0_, _x1_).right) = (_s0_.right)
  ;(set_left([ _x1_, _x2_ ], _y1_)) = ([ _y1_, _x2_ ])
  ;(set_left(_s0_, _x1_).left) = (_x1_)
  ;(([ _x1_, _x2_ ]).right) = (_x2_)
  ;(set_right(_s0_, _x2_).left) = (_s0_.left)
  ;(set_right([ _x1_, _x2_ ], _y2_)) = ([ _x1_, _y2_ ])
  ;(set_right(_s0_, _x2_).right) = (_x2_)
  ..

set automatic-ordering on

