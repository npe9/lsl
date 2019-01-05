Sequence(E): trait
  % Comparison, subsequences
  assumes StrictPartialOrder(>, E)
  includes
    LexicographicOrder,
    String(Seq for String)
  introduces
    isPrefix, isSubstring, isSubsequence: Seq[E], Seq[E] -> Bool
    find:                                 Seq[E], Seq[E] -> Int
  asserts \forall  e, e1, e2: E, s, s1, s2: Seq[E]
    isPrefix(s1, s2) <=> s1 = prefix(s2, len(s1));
    isSubstring(s1, s2) <=>
      isPrefix(s1, s2) \/ isSubstring(s1, tail(s2));
    isSubsequence(empty, s);
    ~isSubsequence(e -| s, empty);
    isSubsequence(e1 -| s1, e2 -| s2) <=>
      (e1 = e2 /\ isSubsequence(s1, s2))
        \/ isSubsequence(e1 -| s1, s2);
    find(empty, empty) = 0;
    find(s1, e -| s2) = 
      (if isPrefix(s1, e -| s2) then 0
       else find(s1, s2) + 1)
  implies
    IsPO(isPrefix, Seq[E]),
    IsPO(isSubstring, Seq[E]),
    IsPO(isSubsequence, Seq[E])
    \forall s, s1, s2: Seq[E], i, n: Int
      isPrefix(prefix(s, n), s);
      isSubstring(substring(s, i, n), s);
      isSubstring(s1, s2) => isSubsequence(s1, s2)
    converts 
        isPrefix, isSubstring, isSubsequence, find
      exempting \forall s: Seq[E], e: E  find(e -| s, empty)
