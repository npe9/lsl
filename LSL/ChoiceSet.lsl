ChoiceSet(E): trait
  % A set with a weakly-specified choose operator
  includes Set
  introduces
    choose:  Set[E] -> E
    rest:    Set[E] -> Set[E]
    isEmpty: Set[E] -> Bool
  asserts \forall e, e1: E, s: Set[E]
    s \neq {} => choose(s) \in s;
    s \neq {} => rest(s) = delete(choose(s), s);
    isEmpty(s) <=> s = {}
  implies
    sort Set[E] partitioned by choose, rest, isEmpty
    \forall e: E, s: Set[E]
      s \neq {} => s = insert(choose(s), rest(s))
