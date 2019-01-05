Graph(N, G): trait
  % n1 \langle g \rangle n2 means that there is
  % an edge from n1 to n2 in g
  includes Relation(N for E, G for R)
  introduces
    nodes, undirected:                  G       -> G
    isPath:                             N, N, G -> Bool
    stronglyConnected, weaklyConnected: G       -> Bool
  asserts \forall n1, n2: N, g: G
    undirected(g) = g \U (g\inv);
    nodes(g) = dom(g) \U range(g);
    isPath(n1, n2, g) <=> n1 \langle g\superstar \rangle n2;
    stronglyConnected(g) <=> g\superstar = nodes(g) \times nodes(g);
    weaklyConnected(g) <=> stronglyConnected(undirected(g))
  implies
    \forall n1, n2: N, g: G
      (stronglyConnected(g) /\ n1 \in nodes(g)
          /\ n2 \in nodes(g))
        => isPath(n1, n2, g)
