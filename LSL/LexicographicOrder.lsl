LexicographicOrder(E, C): trait
  % "Dictionary" order on C
  assumes
    Container,
    StrictTotalOrder(<, E)
  includes DerivedOrders(C)
  asserts \forall c1, c2: C
    c1 < c2 <=>
        c2 \neq empty
          /\ (c1 = empty
              \/ (if head(c1) = head(c2)
                  then tail(c1) < tail(c2)
                  else head(c1) < head(c2)))
  implies 
    TotalOrder(C)
    converts <=:C,C->Bool, >=:C,C->Bool,
      <:C,C->Bool, >:C,C->Bool
