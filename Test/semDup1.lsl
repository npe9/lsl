% Check detection of duplicate inclusions, assertions

semDup1: trait
  includes semDup1a, semDup1b, semDup1c(M), semDup1c(N)
  introduces a, b, c: -> Bool
  asserts equations a; b; c; c

