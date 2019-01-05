%%% Axioms for trait semEnum

%% Operator declarations

declare sorts
  E
  ..

declare operators
  a: -> E
  , b: -> E
  , c: -> E
  , d: -> E
  , e: -> E
  , f: -> E
  , succ: E -> E
  ..

set automatic-ordering off

%% Assertions


% main trait: semEnum

set name semEnum

assert
  sort E generated freely by a, b, c, d, e, f
  ;(succ(a)) = (b)
  ;(succ(b)) = (c)
  ;(succ(c)) = (d)
  ;(succ(d)) = (e)
  ;(succ(e)) = (f)
  ..

set automatic-ordering on

