% Shorthands

parse06: trait
	Color   enumeration of red, white, blue
	Pair    tuple of       first, second: Color
	Mixture union of       tint, hue: Color, p: Bool
	introduces 
		p: -> Pair
		m: -> Mixture
	asserts equations
		succ(red) \neq succ(white)			\eqsep
		p.first = red					\eqsep
		p.second = white				\eqsep
		[red, white] = p				\eqsep
		set_first(p, red) = p				\eqsep
		hue(red) = m					\eqsep
		m.hue = red					\eqsep
		tag(m) = hue
		
		
