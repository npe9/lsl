Permutation(E, C): trait
  % Test for having the same elements
  assumes Container
  includes
    Bag,
    CoerceContainer(C for DC, Bag[E] for RC) 
  introduces isPermutation: C, C -> Bool
  asserts \forall c1, c2: C
      isPermutation(c1, c2) <=> coerce(c1) = coerce(c2)
  implies \forall e: E, c1, c2: C
    isPermutation(c1, c2)
      => count(e, c1) = count(e, c2)