JoinOp(\join, C): trait
  % Container combining operator
  % e.g., union, concatenation
  assumes InsertGenerated
  introduces __\join __: C, C -> C
  asserts \forall e: E, c, c1, c2: C
    empty \join c = c;
    insert(e, c1) \join c2 = insert(e, c1 \join c2)
  implies 
    Associative(\join, C)
    converts \join
