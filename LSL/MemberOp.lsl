MemberOp(C): trait
  assumes InsertGenerated
  introduces
    __ \in __, __ \notin __: E, C -> Bool
  asserts \forall e, e1, e2: E, c: C
    e \notin c <=> ~(e \in c);
    e \notin empty;
    e1 \in insert(e2, c) <=> e1 = e2 \/ e1 \in c
  implies converts \in, \notin
