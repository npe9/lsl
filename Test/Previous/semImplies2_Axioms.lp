%%% Axioms for trait semImplies2

%% Operator declarations

declare operators
  c: -> Bool
  , b: -> Bool
  ..

set automatic-ordering off

%% Assertions


% main trait: semImplies2

set name semImplies2

assert
  (b)
  ..

% subtrait 1: semImplies1a (c: -> Bool for a: -> Bool)

set name semImplies1a

assert
  (c)
  ..

set automatic-ordering on

