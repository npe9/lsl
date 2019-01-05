RelationOps: trait
  % Useful non-primitive operators on relations.
  assumes RelationBasics
  includes 
    DerivedOrders(R, \subseteq for <=, \supseteq for >=, 
                  \subset for <, \supset for >)
  introduces
    __ \in __, __ \notin __:                      E, R       -> Bool
    set, dom, range, __\superplus, __\superstar:  R          -> R
    __ \I __, __ \circ __, __ - __, __ \times __: R, R       -> R
    domRestrict, rangeRestrict, image:            R, R       -> R
    skolem:                                       E, R, R, E -> E
  asserts
    \forall e, e1, e2, e3: E, r, r1, r2: R
      e \in r <=> e \langle r \rangle e;
      e \notin r <=> ~(e \in r);
      set(r) = r \I I;
      dom(r) = set(r \circ \top);
      range(r) = set(\top \circ r);
      e1 \langle r1 \I r2 \rangle e2 <=> e1 \langle r1 \rangle e2 /\ e1 \langle r2 \rangle e2;
      (e1 \langle r1 \rangle e2 /\ e2 \langle r2 \rangle e3)
        => e1 \langle r1 \circ r2 \rangle e3;
      e1 \langle r1 \circ r2 \rangle e2
        => (e1 \langle r1 \rangle skolem(e1, r1, r2, e2) 
          /\ skolem(e1, r1, r2, e2) \langle r2 \rangle e2);
      r\superplus = r \circ (r\superstar);
      r\superstar = I \U (r\superplus);
      (r1 = I \U r2 /\ r2 = r \circ r1) => 
        ((r\superstar) \subseteq r1 /\ (r\superplus) \subseteq r2);
      r1 - r2 = r1 \I (-r2);
      r1 \times r2 = set(r1) \circ \top \circ set(r2);
      r1 \subseteq r2 <=> r1 - r2 = \bot;
      domRestrict(r1, r2) = r1 \I (r2 \circ \top);
      image(r1, r2) = set(r1) \circ r2;
      rangeRestrict(r1, r2) = r1 \I (\top \circ r2)
  implies
    AbelianMonoid(\top for unit, \I for \circ, R for T),
    Distributive(\U for +, \I for *, R for T),
    Distributive(\I for +, \U for *, R for T),
    Idempotent(set, R),
    Monoid(I for unit, R for T),
    Lattice(R for T, \U for \lub, \I for \glb, \subseteq for <=, 
            \supseteq for >=, \subset for <, \supset for >),
    PartialOrder(R, \subseteq for <=, \supseteq for >=, \subset for <, 
                 \supset for >)
    \forall e: E, r, r1, r2: R
      e \in r <=> e \in set(r);
      -(r1 \U r2) = (-r1) \I (-r2);
      -(r1 \I r2) = (-r1) \U (-r2);
      (r1 \circ r2)\inv = (r2\inv) \circ (r1\inv)
    converts
      \in, \notin, set, dom, range, __\superplus, __\superstar, __-__, \times,
      \U, \I, \circ, -:R->R, \inv, \subseteq, \supseteq, \subset, \supset, 
      domRestrict, image, rangeRestrict
