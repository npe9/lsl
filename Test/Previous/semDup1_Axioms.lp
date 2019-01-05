%%% Axioms for trait semDup1

%% Operator declarations

declare sorts
  M, N
  ..

declare operators
  a: -> Bool
  , b: -> Bool
  , c: -> Bool
  , x: -> M
  , x: -> N
  ..

set automatic-ordering off

%% Assertions


% main trait: semDup1

set name semDup1

assert
  (a)
  ;(b)
  ;(c)
%   duplicate of equation in main trait: semDup1
  ..

% subtrait 1: semDup1a

% duplicate of equation in main trait: semDup1

% subtrait 2: semDup1b

% duplicate of equation in main trait: semDup1
% duplicate of equation in main trait: semDup1

% subtrait 2.1: semDup1a

% duplicate of subtrait 1: semDup1a

% subtrait 3: semDup1c (M for X)

% duplicate of equation in main trait: semDup1
set name semDup1c

assert
  (x:M = x:M)
  ..

% subtrait 3.1: semDup1a

% duplicate of subtrait 1: semDup1a

% subtrait 4: semDup1c (N for X)

% duplicate of equation in main trait: semDup1
set name semDup1c

assert
  (x:N = x:N)
  ..

% subtrait 4.1: semDup1a

% duplicate of subtrait 1: semDup1a

set automatic-ordering on

