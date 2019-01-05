%%% Axioms for trait semQual

%% Operator declarations

declare sorts
  A, B
  ..

declare operators
  f: A -> B
  ..

set automatic-ordering off

%% Assertions

declare variables
  x: A
  ..


% main trait: semQual

set name semQual

assert
  (x = x)
  ..

set automatic-ordering on

