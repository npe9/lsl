InsertGenerated(E, C): trait
  % C's contain finitely many E's
  introduces
    empty:       -> C
    insert: E, C -> C
  asserts 
    sort C generated by empty, insert
