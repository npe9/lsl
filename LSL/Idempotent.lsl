Idempotent(op, T): trait
  introduces op: T -> T
  asserts \forall x: T
    op(op(x)) = op(x)
