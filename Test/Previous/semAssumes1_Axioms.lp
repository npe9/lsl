%%% Axioms for trait semAssumes1

%% Operator declarations

declare operators
  d: -> Bool
  , b: -> Bool
  , c: -> Bool
  ..

set automatic-ordering off

%% Assertions


% main trait: semAssumes1

set name semAssumes1

assert
  (d)
  ..

% subtrait 1: semAssumes1a


% subtrait 2: semAssumes1b

set name semAssumes1b

assert
  (b)
  ..

%% Assumptions


% subtrait 3: semAssumes1c

set name semAssumes1c

assert
  (c)
  ..

set automatic-ordering on

