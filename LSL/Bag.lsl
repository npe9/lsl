Bag(E): trait
  % Common bag operators
  includes
    BagBasics,
    DerivedOrders(Bag[E], \subseteq for <=, \supseteq for >=,
                  \subset for <, \supset for >)
  introduces
    delete:                E, Bag[E]      -> Bag[E]
    {__}:                  E              -> Bag[E]
    __\in __, __\notin __: E, Bag[E]      -> Bool
    size:                  Bag[E]         -> Int
    __\U __, __-__:        Bag[E], Bag[E] -> Bag[E]
  asserts
    \forall e, e1, e2: E, b, b1, b2: Bag[E]
      count(e1, delete(e2, b)) =
        (if e1 = e2 then max(0, count(e1, b) - 1)
         else count(e1, b));
      {e} = insert(e, {});
      e \in b <=> count(e, b) > 0;
      e \notin b <=> count(e, b) = 0;
      size({}) = 0;
      size(insert(e, b)) = size(b) + 1;
      count(e, b1 \U b2) =
        count(e, b1) + count(e, b2);
      count(e, b1 - b2) =
        max(0, count(e, b1) - count(e, b2));
      b1 \subseteq b2 <=> b1 - b2 = {};
  implies
    AbelianMonoid(\U for \circ, {} for unit, Bag[E] for T),
    JoinOp(\U, Bag[E], {} for empty),
    MemberOp(Bag[E], {} for empty),
    PartialOrder(Bag[E], \subseteq for <=, \supseteq for >=,
                 \subset for <, \supset for >)
    \forall e, e1, e2: E, b, b1, b2: Bag[E]
      insert(e, b) \neq {};
      count(e, b) >= 0;
      count(e, b) <= size(b);
      b1 \subseteq b2 => count(e, b1) <= count(e, b2)
    converts count, \in, \notin, {__}, \U, -:Bag[E],Bag[E]->Bag[E],
      delete, size, \subseteq, \supseteq, \subset, \supset
