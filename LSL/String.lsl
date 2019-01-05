String(E): trait
  % Index, substring
  includes List(String for List)
  introduces 
    __[__]:       String[E], Int      -> E
    prefix:       String[E], Int      -> String[E]
    removePrefix: String[E], Int      -> String[E]
    substring:    String[E], Int, Int -> String[E]
  asserts \forall e: E, s: String[E], i, n: Int
    tail(empty) = empty;
    init(empty) = empty;
    s[0] = head(s);
    n >= 0 => s[n + 1] = tail(s)[n];
    prefix(empty, n) = empty;
    prefix(s, 0) = empty;
    n >= 0
      => prefix(e -| s, n + 1) = e -| prefix(s, n);
    removePrefix(s, 0) = s;
    n >= 0 
      => removePrefix(s, n + 1)
         = removePrefix(tail(s), n);
    substring(s, 0, n) = prefix(s, n);
    i >= 0
      => substring(s, i + 1, n)
         = substring(tail(s), i, n)
  implies
    IndexOp(String[E], -| for insert)
    sort String[E] partitioned by len, __[__]
    converts tail, init
