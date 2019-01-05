RelationBasics(E, R): trait
  % e1 \langle r \rangle e2 means e1 is related to e2 by r.
  introduces 
    __ \langle __ \rangle __: E, R, E -> Bool
    \bot, \top, I:                    -> R
    [__, __]:                 E, E    -> R
    -__, __\inv:              R       -> R
    __ \U __:                 R, R    -> R
  asserts
    sort R partitioned by __ \langle __ \rangle __
    \forall e, e1, e2, e3, e4: E, r, r1, r2: R
      ~(e1 \langle \bot \rangle e2);
      e1 \langle \top \rangle e2;
      e1 \langle I \rangle e2 <=> e1 = e2;
      e1 \langle [e2, e3] \rangle e4 <=> e1 = e2 /\ e3 = e4;
      e1 \langle -r \rangle e2 <=> ~(e1 \langle r \rangle e2);
      e1 \langle r\inv \rangle e2 <=> e2 \langle r \rangle e1;
      e1 \langle r1 \U r2 \rangle e2 <=> e1 \langle r1 \rangle e2 \/ e1 \langle r2 \rangle e2
  implies
    AbelianMonoid(\bot for unit, \U for \circ, R for T),
    Involutive(__\inv, R),
    Involutive(-__, R)
    equations
      -\bot = \top;
      -\top = \bot;
      \bot\inv = \bot;
      \top\inv = \top
    converts \U, -__, __\inv
