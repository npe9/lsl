% Test semantics of AC, Commutative

semAC: trait
  includes AC(A)
  includes Commutative(B)
  introduces __+__: C, C -> C
  implies AC(C)

