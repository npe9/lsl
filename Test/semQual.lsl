% Test qualification of output for implication

semQual: trait
  introduces f: A -> B
  asserts \forall x:A x = x
  implies \forall x:B x = x


