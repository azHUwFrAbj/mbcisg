# Cycle-Based-Sliding-Window-BCC

Design documents, codes and experiment instructions.

## Data Structures

### 1. MST

#### Fields

**Global:**

- `N`: Number of vertices
- `vid2Node`: Maps vertex ID to tree node

**Local (per node):**

- `size`: Size of the subtree
- `vid`: Vertex ID
- `parent`: Parent node
- `neighbours`: Neighbors (includes parent)
- `timestamp`: timestamp of the edge from this node to its parent

#### Key Operations

- `find_adjacent_edge(v, w)`: Returns the tree edge adjacent to `v` or `w` on their path. If the path doesn't exist,
  returns the edge linking to their parent. Faster than `find_path` since it avoids full path construction. Uses `size`
  for judgment. Complexity: $O(h)$.

---

### 2. BBF

#### Fields

**Global:**

- `Q`: Bidirectional queue of tree edges
- `edge2Q`: Maps tree edge to queue iterator for fast expiration
- `edge2Node`: Maps tree edge to BBF node

**Local (per node):**

- `gamma`: The tree edge the node represents
- `timestamp`: Timestamp of the node
- `out-nbr`: Out-neighbors (parents)
- `children`: In-neighbors (children)

#### Key Operations

- `LCA(e1, e2)`: Returns the LCA of the nodes representing edges `e1` and `e2`. Complexity $O(h)$.
- `swing(x)`: Repairs node `x` with multiple parents. Complexity $O(1)$.
- `swing_adv(x)`: Advanced swing using quick and big swing optimizations. Complexity $O(h)$

---

### 3. AUF (Augmented Union-Find)

#### Fields

- `parent[]`: Disjoint-set parent array (roots are edges with minimal timestamp)
- `rank[]`: Rank array for union-by-rank optimization
- `anchor[]`: the anchored node
- `edge2i`: Maps tree edge to its index in the `parent` array

#### Operations

- `find(x)`: Returns the root of the component containing `x`
- `union(x, y)`: Merges the sets containing `x` and `y`