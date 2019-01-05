% Errors in inclusions with renamings

check11: trait
	includes check10d(+ for A)		% operator for sort
	includes check10d(__+__ for s)		% too many __'s for unary op
	includes check10e(+ for f)		% arity of new op is too small
	includes check10e(.first for f)		% ditto
	includes check10d(* for s)		% ambiguous where __ belongs
	includes check10d(__* for +)		% too few __'s in new op
	includes check10e([__] for f)		% wrong number of __'s
	includes check10d(A, s, +, B for A)	% sort renamed twice
	includes check10d(B for A, C for A)	% ditto
	includes check10e(g, h for f)		% op renamed twice
	includes check10e(g for f, h for f)	% ditto
