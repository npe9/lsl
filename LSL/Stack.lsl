Stack(E): trait
  % Common LIFO operators
  includes StackBasics, Integer
  introduces
    count:     E, Stack[E] -> Int
    __ \in __: E, Stack[E] -> Bool
    size:      Stack[E]    -> Int
    isEmpty:   Stack[E]    -> Bool
  asserts
    \forall e: E, stk: Stack[E]
      size(empty) = 0;
      size(push(e, stk)) = size(stk) + 1;
      isEmpty(stk) <=> stk = empty
  implies 
    Container(Stack[E] for C, push for insert, 
              top for head, pop for tail)
    sort Stack[E] partitioned by top, pop, isEmpty
    \forall stk: Stack[E]
      size(stk) >= 0
    converts top, pop, count, \in, size, isEmpty
      exempting top(empty), pop(empty)
