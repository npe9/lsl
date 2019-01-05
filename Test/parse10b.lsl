% Trait for inclusion

parse10b (+, -, *): trait
	introduces
		__+:   N    -> N			% postfix
		-__  : N    -> N			% prefix
		__*__: N, N -> N			% infix

