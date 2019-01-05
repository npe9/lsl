% Errors in equations

check05: trait
        introduces 
		a:->N 
		a:->T
		b:->N 
		c:->S 
	asserts
	    \forall i:M i = i			% sort not introduced
	    \forall c:S c = c			% variable duplicates constant
	    \forall i:N, i:N i = i 		% variable declared twice
            \forall b:S, b:T
		b = c		\eqsep		% ok
		a = c		\eqsep		% sorts don't match
		a = b				% ambiguous

