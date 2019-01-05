% Test discharging implications textually (buggy)

semImplies2: trait
  includes semImplies1a(c)
  introduces b: -> Bool
  asserts equations b
  implies
    semImplies1a(c)	
    equations b
