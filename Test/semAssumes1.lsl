% Check that assumptions are discharged

semAssumes1: trait
  includes semAssumes1a		% which assumes semAssumes1b..d
  includes semAssumes1b		% to discharge semAssumes1b
  assumes semAssumes1c		% to discharge semAssumes1c
  asserts equations d		% to discharge semAssumes1d
