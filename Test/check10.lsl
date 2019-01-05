% Errors in inclusions

check10: trait
	includes check10a		% cycle
	includes check03		% error in subtrait
	includes check10b		% same error again lower in hierarchy
	includes check10c(N)		% actual without formal
	includes check10d(N)		% too few actuals
	includes check10e(N, N)		% too many actuals
	includes check10d(+, f, *)		% operator for sort
	includes check10d(N, __+__, *)	% too many __'s for unary formal
	includes check10e(+)		% arity of actual is too small
	includes check10e(.first)	% ditto
	includes check10d(N, +, *)	% ambiguous where __ belongs
	includes check10d(N, s, __*)	% too few __'s for formal
	includes check10e([__])		% wrong number of __'s
