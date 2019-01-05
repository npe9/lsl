% Test proof obligations for conversion

semCvts: trait
   E enumeration of a, b, c
   Pair tuple of left, right: E
   introduces
      f: E, E -> E
      __ \op __: E, E -> E
   implies converts a, .left exempting \forall p: Pair p.left
