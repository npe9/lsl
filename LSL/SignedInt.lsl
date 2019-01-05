SignedInt(maxSigned, N): trait
  % Typical machine arithmetic, signed complement.
  includes IntCycle(minSigned, maxSigned, N)
  asserts equations
    succ(minSigned) = -maxSigned
  implies equations
    minSigned + maxSigned = -1;
    abs(minSigned) = minSigned
