# Holm-FD-BCC-Simplified

A simplified version of Holm's fully dynamic biconnectivity algorithm, where the involvement of replacement edge make
the implementation easier. Top-tree and the other data structures in the paper are not implemented in this repository.

The main reference paper:
> Jacob Holm, Kristian de Lichtenberg, and Mikkel Thorup. 2001. Poly-logarithmic deterministic fully-dynamic algorithms
> for connectivity, minimum spanning tree, 2-edge, and biconnectivity. J. ACM 48, 4 (July 2001),
> 723–760. https://doi.org/10.1145/502090.502095

# Implementations

## Notations

Here is the map between the notations in the paper and the notations we used in pseudocode:

| Notation  | Paper's Notation                  | Meanings                                                                                                                                                                                                                                  | Impl         |
|-----------|-----------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------|
| $M_v$     | $C^*_{v,\cdot}(\cdot)$            | $M_v[w,i]=C^*_{v,i}(w)$, the neighbours of $w$ that are $c^*$-biconnected at level $i$.                                                                                                                                                   | map\*, set\* |
| $M_{M_v}$ | -                                 | $M_{M_v}[w]=M_w$, managing all $M_v$ for $v \in V$                                                                                                                                                                                        | hash map\*   |
| $M_c$     | $c(\cdot)$                        | $M_c[(u, v)]=M_c[u, v]=c(e)$, the cover level of a tree edge                                                                                                                                                                              | map          |
| $M_l$     | $l(\cdot)$                        | $M_l[(u, v)]=M_l[u, v]=l(e)$, the level of a non-tree edge                                                                                                                                                                                | map          |
| $M_{N_l}$ | -                                 | The non-tree neighbours of $v$ at level $l$, such that $M_{N_l}[v, l]= \{w\|(v, w) \not \in T \wedge l(v, w) = l \}$                                                                                                                      | map          |
| $M_{V_l}$ | -                                 | The vertices at level $l$, such that $M_{V_l}[l]= \{w\|(v, w) \not \in T \wedge l(v, w) = l \}$                                                                                                                                           | map          |
| $M^*$     | $C_{\cdot,\cdot}^*(\cdot\|\cdot)$ | $M^\*[x,y,z,j]=C_{y,j}^\*(x\|z)$, not-yet-maintained neighbour: The decision whether to merge $C^\*_{y,j}(x)$ and $C^\*_{y,j}(z)$ is not made.                                                                                            | map          |
| $R$       | -                                 | $R[e]$ is the replacement edge of $e$ for a tree edge $e$                                                                                                                                                                                 | map          |
| $F$       | $F$                               | The spanning forest with weight and replacement edge as shown in claim 1. It is likely to be truth that we maintain a fully dynamic spanning tree that can answer Biconnectivity query, with the help of replacement edge recordings $R$. | tree         |

## Implementations

Here we show where we implement our pseudocode data structures.

| Notation        | Implemented in                                                                  | Status |
|-----------------|---------------------------------------------------------------------------------|--------|
| $F$'s structure | `TopTree` (Structure related operation);`EdgeInfo`(weight and replacement edge) | Done   |
| $F$'s edge info | `EdgeInfo`                                                                      | Done   |
| $M_v$           | `NeighbourPartition`                                                            | Done   |
| $M_{M_v}$       | `NeighbourPartition`                                                            | Done   |
| $M_c$           | `EdgeInfo`                                                                      | Done   |
| $M_l$           | `EdgeInfo`                                                                      | Done   |
| $R$             | `EdgeInfo`                                                                      | Done   |
| $M_{N_l}$       | `LevelInfo`                                                                     | Done   |
| $M_{V_l}$       | `LevelInfo`                                                                     | Done   |
| $M^*$           | `NotYetMergeSet`                                                                | Done   |

