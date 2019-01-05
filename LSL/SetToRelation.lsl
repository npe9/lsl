SetToRelation: trait
  % Maps a (finitely generated) set
  % to the relation that represents it.
  assumes SetBasics, RelationBasics
  introduces
    relation: Set[E] -> R
  asserts
    \forall e: E, s: Set[E]
      relation({}) = \bot;
      relation(insert(e, s)) = [e, e] \U relation(s)
  implies
    \forall e: E, s: Set[E]
      e \in s <=> e \langle relation(s) \rangle e
    converts relation
