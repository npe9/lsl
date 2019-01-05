Conditional(T): trait
  % This trait is given for documentation only.
  % It is implicit in LSL.
  introduces if__then__else__: Bool, T, T -> T
  asserts
    \forall x, y, z: T
      (if true then x else y) = x;
      (if false then x else y) = y
  implies \forall b: Bool, x: T
    (if b then x else x) = x
