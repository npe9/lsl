PointwiseImage: trait
  % Apply elemOp to each element
  assumes
    InsertGenerated(DE for E, DC for C),
    InsertGenerated(RE for E, RC for C)
  introduces
    elemOp:      DE -> RE
    containerOp: DC -> RC
  asserts \forall dc: DC, de: DE
    containerOp(empty) = empty;
    containerOp(insert(de, dc)) =
      insert(elemOp(de), containerOp(dc))
  implies converts containerOp
