ReverseOp: trait
  % An operator on lists commonly used
  % to demonstrate theorem provers.
  assumes List
  introduces reverse: List[E] -> List[E]
  asserts \forall e: E, l, l1, l2: List[E]
    reverse(empty) = empty;
    reverse(e -| l) = reverse(l) |- e
  implies \forall e: E, l, l1, l2: List[E]
      reverse(reverse(l)) = l;
      l \neq empty => head(reverse(l)) = last(l);
      l \neq empty 
        => tail(reverse(l)) = reverse(init(l));
      len(reverse(l)) = len(l);
      reverse(l1 || l2) = reverse(l2) || reverse(l1)
    converts reverse
