%%% Axioms for trait semUnion

%% Operator declarations

declare sorts
  U, A, B, U_tag
  ..

declare operators
  __.first: U -> A
  , first: A -> U
  , first: -> U_tag
  , __.second: U -> A
  , second: A -> U
  , second: -> U_tag
  , __.third: U -> B
  , third: B -> U
  , third: -> U_tag
  , succ: U_tag -> U_tag
  , tag: U -> U_tag
  ..

set automatic-ordering off

%% Assertions

declare variables
  _x1_: A
  _x2_: A
  _x3_: B
  ..


% main trait: semUnion

set name semUnion

assert
  sort U_tag generated freely by first: -> U_tag, second: -> U_tag, third: ->
    U_tag
  ;(succ(first)) = (second)
  ;(succ(second)) = (third)
  ;sort U generated freely by first: A -> U, second: A -> U, third: B -> U
  ;sort U partitioned by __.first, __.second, __.third, tag
  ;(first(_x1_).first) = (_x1_)
  ;(tag(first(_x1_))) = (first)
  ;(second(_x2_).second) = (_x2_)
  ;(tag(second(_x2_))) = (second)
  ;(third(_x3_).third) = (_x3_)
  ;(tag(third(_x3_))) = (third)
  ..

set automatic-ordering on

