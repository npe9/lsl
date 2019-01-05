Set(E): trait
  % Common set operators
  includes
    SetBasics,
    Integer,
    DerivedOrders(Set[E], \subseteq for <=, \supseteq for >=,
                  \subset for <, \supset for >)
  introduces
    __\notin __:             E, Set[E]      -> Bool
    delete:                  E, Set[E]      -> Set[E]
    {__}:                    E              -> Set[E]
    __\U __, __\I __, __-__: Set[E], Set[E] -> Set[E]
    size:                    Set[E]         -> Int
  asserts
    \forall e, e1, e2: E, s, s1, s2: Set[E]
      e \notin s <=> ~(e \in s);
      {e} = insert(e, {});
      e1 \in delete(e2, s) <=> e1 \neq e2 /\ e1 \in s;
      e \in (s1 \U s2) <=> e \in s1 \/ e \in s2;
      e \in (s1 \I s2) <=> e \in s1 /\ e \in s2;
      e \in (s1 - s2)  <=> e \in s1 /\ e \notin s2;
      size({}) = 0;
      size(insert(e, s)) =
        (if e \notin s then size(s) + 1 else size(s));
      s1 \subseteq s2 <=> s1 - s2 = {}
  implies
    AbelianMonoid(\U for \circ, {} for unit, Set[E] for T), 
    AC(\I, Set[E]),
    JoinOp(\U, Set[E], {} for empty),
    MemberOp(Set[E], {} for empty),
    PartialOrder(Set[E], \subseteq for <=, \supseteq for >=,
                 \subset for <, \supset for >)
    sort Set[E] generated by {}, {__}, \U
    \forall e: E, s, s1, s2: Set[E]
      s1 \subseteq s2 => (e \in s1 => e \in s2);
      size(s) >= 0
    converts
      \in, \notin, {__}, delete, size, \U, \I, -:Set[E],Set[E]->Set[E],
      \subseteq, \supseteq, \subset, \supset