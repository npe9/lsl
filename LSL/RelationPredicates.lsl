RelationPredicates: trait
  % Tests for useful properties
  % of individual relations.
  assumes
    RelationBasics,
    RelationOps
  introduces
    antisymmetric, asymmetric, equivalence,
      functional, irreflexive, oneToOne, reflexive,
      symmetric, total, transitive: R -> Bool
    into, onto: R, R -> Bool
  asserts
    \forall r, r1, r2: R
      antisymmetric(r) <=> (r \I (r\inv)) \subseteq I;
      asymmetric(r) <=> r \I (r\inv) = \bot;
      equivalence(r) <=>
        reflexive(r) /\ symmetric(r) /\ transitive(r);
      functional(r) <=> ((r\inv) \circ r) \subseteq I;
      irreflexive(r) <=> r \I I = \bot;
      oneToOne(r) <=> r \circ (r\inv) = I;
      reflexive(r) <=> I \subseteq r;
      symmetric(r) <=> r = r\inv;
      total(r) <=> dom(r) = I;
      transitive(r) <=> r = r\superplus;
      into(r1, r2) <=> range(r1) \subseteq set(r2);
      onto(r1, r2) <=> set(r2) \subseteq range(r1);
  implies converts
    antisymmetric, asymmetric, equivalence,
    functional, irreflexive, oneToOne, reflexive,
    symmetric, total, transitive, into, onto
