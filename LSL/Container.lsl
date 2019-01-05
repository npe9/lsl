Container(E, C): trait
  % head and tail enumerate contents of a C
  includes InsertGenerated, Integer
  introduces
    isEmpty:   C    -> Bool
    count:     E, C -> Int
    __ \in __: E, C -> Bool
    head:      C    -> E
    tail:      C    -> C
  asserts 
    sort C partitioned by isEmpty, head, tail
    \forall e, e1: E, c: C
      isEmpty(empty);
      ~isEmpty(insert(e, c));
      count(e, empty) = 0;
      count(e, insert(e1, c)) =
        count(e, c) + (if e = e1 then 1 else 0);
      e \in c <=> count(e, c) > 0;
      ~isEmpty(c) =>
        count(e, insert(head(c), tail(c)))
          = count(e, c)
  implies
    \forall c: C
      ~isEmpty(c) => count(head(c), c) > 0;
    converts isEmpty, count, \in
