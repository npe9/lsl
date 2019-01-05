%%% Axioms for trait semTuple

%% Operator declarations

declare sorts
  T, A, B
  ..

declare operators
  __.first: T -> A
  , __.second: T -> A
  , __.third: T -> B
  , set_first: T, A -> T
  , set_second: T, A -> T
  , set_third: T, B -> T
  , [ __ , __ , __ ]: A, A, B -> T
  ..

set automatic-ordering off

%% Assertions

declare variables
  _s0_: T
  _x1_: A
  _y1_: A
  _x2_: A
  _y2_: A
  _x3_: B
  _y3_: B
  ..


% main trait: semTuple

set name semTuple

assert
  sort T generated freely by [ __ , __ , __ ]
  ;([ _s0_.first, _s0_.second, _s0_.third ]) = (_s0_)
  ;(([ _x1_, _x2_, _x3_ ]).first) = (_x1_)
  ;(set_first(_s0_, _x1_).second) = (_s0_.second)
  ;(set_first(_s0_, _x1_).third) = (_s0_.third)
  ;(set_first([ _x1_, _x2_, _x3_ ], _y1_)) = ([ _y1_, _x2_, _x3_ ])
  ;(set_first(_s0_, _x1_).first) = (_x1_)
  ;(([ _x1_, _x2_, _x3_ ]).second) = (_x2_)
  ;(set_second(_s0_, _x2_).first) = (_s0_.first)
  ;(set_second(_s0_, _x2_).third) = (_s0_.third)
  ;(set_second([ _x1_, _x2_, _x3_ ], _y2_)) = ([ _x1_, _y2_, _x3_ ])
  ;(set_second(_s0_, _x2_).second) = (_x2_)
  ;(([ _x1_, _x2_, _x3_ ]).third) = (_x3_)
  ;(set_third(_s0_, _x3_).first) = (_s0_.first)
  ;(set_third(_s0_, _x3_).second) = (_s0_.second)
  ;(set_third([ _x1_, _x2_, _x3_ ], _y3_)) = ([ _x1_, _x2_, _y3_ ])
  ;(set_third(_s0_, _x3_).third) = (_x3_)
  ..

set automatic-ordering on

