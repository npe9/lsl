List(E): trait
  % Defines singleton and concatenation
  includes Deque(E, List for Deque)
  introduces 
    {__}:     E                -> List[E]
    __ || __: List[E], List[E] -> List[E]
  asserts \forall e: E, ls, ls1, ls2: List[E]
    {e} = empty |- e;
    ls || empty = ls;
    ls1 || (ls2 |- e) = (ls1 || ls2) |- e
  implies
    sort List[E] generated by empty, {__}, ||
    converts head, last, tail, init, len, {__}, ||
      exempting head(empty), last(empty),
        tail(empty), init(empty)
