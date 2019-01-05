TotalPreOrder(<=, T): trait
  includes PreOrder
  asserts \forall x, y: T
    x <= y \/ y <= x
