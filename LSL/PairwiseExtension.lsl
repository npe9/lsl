PairwiseExtension(\circ, \odot, E, C): trait
  % Induces a binary operator on containers
  % from a binary operator on elements.
  assumes Container
  introduces
    __\circ __: E, E -> E
    __\odot __: C, C -> C
  asserts \forall e1, e2: E, c1, c2: C
    empty \odot empty = empty;
    (c1 \neq empty /\ c2 \neq empty)
      => c1 \odot c2 = insert(head(c1) \circ head(c2),
                           tail(c1) \odot tail(c2));
  implies converts \odot
    exempting \forall e: E, c: C
      empty \odot insert(e, c), insert(e, c) \odot empty
