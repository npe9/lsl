CoerceContainer(DC, RC): trait
  % Insert each element of DC in a new RC
  assumes
    InsertGenerated(DC for C),
    InsertGenerated(RC for C)
  introduces coerce: DC -> RC
  asserts \forall dc: DC, e: E
    coerce(empty) = empty;
    coerce(insert(e, dc)) = insert(e, coerce(dc))
  implies converts coerce
