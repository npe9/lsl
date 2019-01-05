ElementTest(pass, E, C, T): trait
  % filter collects elements accepted by pass
  assumes InsertGenerated
  introduces 
    pass:     E, T -> Bool
    filter:   C, T -> C
    allPass:  C, T -> Bool
    somePass: C, T -> Bool
  asserts \forall c: C, e: E, t: T
    filter(empty, t) = empty;
    filter(insert(e, c), t) = 
       (if pass(e, t) then insert(e, filter(c, t))
        else filter(c, t));
    allPass(empty, t);
    allPass(insert(e, c), t) <=>
      pass(e, t) /\ allPass(c, t);
    somePass(c, t) <=> filter(c, t) \neq empty
  implies converts filter, somePass, allPass
