#include "top_tree.h"
#include "tree.h"

#include <limits.h>

#define is_point(n) (((tt_node *) n)->num_boundary < 2)
#define is_path(n) (((tt_node *) n)->num_boundary == 2)

static inline int int_max(int a, int b) {
    if (a < b)
        return b;
    else
        return a;
}

static void recompute_spine_weight(tt_node *node) {
    if (is_point(node)) {
        node->spine_weight = INT_MIN;
if (!node->is_leaf) {
            tt_int_node *int_node = (tt_int_node *) node;
            int_node->info.edgenum = 0;
            int_node->info.edgenum = int_node->children[0]->edgenum + int_node->children[1]->edgenum;
            }
        } else if (node->is_leaf) {
        tt_leaf_node *leaf = (tt_leaf_node *) node;
        leaf->info.spine_weight = leaf->edge->weight;
        leaf->info.edgenum = 1;
        } else {
        tt_int_node *int_node = (tt_int_node *) node;
        int spine_weight_0 = int_node->children[0]->spine_weight;
        int spine_weight_1 = int_node->children[1]->spine_weight;
        int_node->info.spine_weight = int_max(spine_weight_0, spine_weight_1);
        int_node->info.edgenum = 0;
        int_node->info.edgenum = int_node->children[0]->edgenum + int_node->children[1]->edgenum;
        }
}

tt_leaf_node *find_maximum(tt_node *root) {
    tt_node *node = root;
    while (!node->is_leaf) {
        tt_int_node *int_node = (tt_int_node *) node;
        if (int_node->children[0]->spine_weight > int_node->children[1]->spine_weight) {
            node = int_node->children[0];
        } else {
            node = int_node->children[1];
        }
    }
    return (tt_leaf_node *) node;
}


tt_node *find_root(tt_node *node) {
    if (node)
        while (node->parent)
            node = (tt_node *) node->parent;
    return node;
}

static void push_flip(tt_int_node *node) {
    if (node->info.flip) {
        node->info.flip = false;

        tt_node *tmp = node->children[0];
        node->children[0] = node->children[1];
        node->children[1] = tmp;

        node->children[0]->flip ^= true;
        node->children[1]->flip ^= true;
    }
}

static bool has_left_boundary(tt_node *node) {
    if (node->is_leaf) {
        tt_leaf_node *leaf_node = (tt_leaf_node *) node;
        struct vertex *endp = leaf_node->edge->endpoints[node->flip];
        return endp->is_exposed || !has_at_most_one_incident_edge(endp);
    } else {
        tt_int_node *int_node = (tt_int_node *) node;
        tt_node *child = int_node->children[node->flip];
        return is_path(child);
    }
}

static bool has_right_boundary(tt_node *node) {
    if (node->is_leaf) {
        tt_leaf_node *leaf_node = (tt_leaf_node *) node;
        struct vertex *endp = leaf_node->edge->endpoints[!node->flip];
        return endp->is_exposed || !has_at_most_one_incident_edge(endp);
    } else {
        tt_int_node *int_node = (tt_int_node *) node;
        tt_node *child = int_node->children[!node->flip];
        return is_path(child);
    }
}

static bool has_middle_boundary(tt_node *node) {
    if (node->is_leaf || node->num_boundary == 0)
        return false;
    tt_int_node *int_node = (tt_int_node *) node;
    bool left_path = is_path(int_node->children[0]);
    bool right_path = is_path(int_node->children[1]);

    return node->num_boundary - left_path - right_path;
}

static tt_node *get_sibling(tt_node *node) {
    tt_int_node *parent = node->parent;
    if (!parent) return NULL;
    int j = parent->children[0] == node;
    return parent->children[j];
}

static void rotate_up(tt_node *node) {
    tt_int_node *parent = node->parent;
    tt_int_node *grandparent = parent->info.parent;
    tt_node *grandparent_node = (tt_node *) grandparent;
    tt_node *sibling = get_sibling(node);
    tt_node *uncle = get_sibling((tt_node *) parent);

    push_flip(grandparent);
    push_flip(parent);

    bool uncle_is_left_child = grandparent->children[0] == uncle;
    bool sibling_is_left_child = parent->children[0] == sibling;
    bool to_same_sides = uncle_is_left_child == sibling_is_left_child;
    bool sibling_is_path = is_path(sibling);
    bool uncle_is_path = is_path(uncle);
    bool gp_is_path = is_path(grandparent);
    bool new_parent_is_path, flip_new_parent, flip_grandparent;

    if (to_same_sides && sibling_is_path) {
        bool gp_middle = has_middle_boundary(grandparent_node);
        new_parent_is_path = gp_middle || uncle_is_path;
        flip_new_parent = false;
        flip_grandparent = false;
        if (gp_middle && !gp_is_path) {
            tt_int_node *ggp = grandparent_node->parent;
            if (ggp) {
                bool gp_is_left_child = ggp->children[0] == grandparent_node;
                flip_grandparent = gp_is_left_child == uncle_is_left_child;
            }
        }
    } else {
        if (!to_same_sides) {
            new_parent_is_path = sibling_is_path || uncle_is_path;
            flip_new_parent = sibling_is_path;
            flip_grandparent = sibling_is_path;
            node->flip ^= true;
        } else {
            new_parent_is_path = uncle_is_path;
            flip_new_parent = false;
            flip_grandparent = false;
            sibling->flip ^= true;
        }
    }

    parent->children[uncle_is_left_child] = sibling;
    parent->children[!uncle_is_left_child] = uncle;
    parent->info.flip = flip_new_parent;
    parent->info.num_boundary = new_parent_is_path + 1;

    grandparent->children[uncle_is_left_child] = node;
    grandparent->children[!uncle_is_left_child] = (tt_node *) parent;
    grandparent->info.flip = flip_grandparent;


    recompute_spine_weight((tt_node *) parent);
    recompute_spine_weight((tt_node *) grandparent);

    node->parent = grandparent;
    uncle->parent = parent;
}

static tt_node *splay_step(tt_node *node) {
    while (true) {
        tt_int_node *parent = node->parent;
        if (!parent) return NULL;
        tt_int_node *gparent = parent->info.parent;
        if (!gparent) return NULL;

        if (is_point(node) && is_point(gparent)) {
            rotate_up(node);
            return (tt_node *) gparent;
        }

        tt_int_node *ggparent = gparent->info.parent;
        if (!ggparent) return NULL;
        if (is_path(parent) && (is_path(gparent) || is_point(ggparent))) {
            push_flip(gparent);
            push_flip(parent);
            bool node_is_left = parent->children[0] == node;
            bool parent_is_left = gparent->children[0] == (tt_node *) parent;
            bool gparent_is_left = ggparent->children[0] == (tt_node *) gparent;
            if (node_is_left == parent_is_left) {
                rotate_up(node);
                return (tt_node *) gparent;
            }
            if (parent_is_left == gparent_is_left) {
                rotate_up((tt_node *) parent);
                return (tt_node *) ggparent;
            }
            rotate_up(get_sibling(node));
            rotate_up((tt_node *) parent);
            return (tt_node *) ggparent;
        }

        node = (tt_node *) parent;
    }
}

static void semi_splay(tt_node *node) {
    while (!node)
        node = splay_step(node);
}

static void full_splay(tt_node *node) {
    while (true) {
        tt_node *top = splay_step(node);
        if (!top) return;
        splay_step(top);
    }
}

static tt_node *find_consuming_node(struct vertex *vert) {
    struct edge *start = vert->first_edge;

    if (!start) return NULL;
    tt_node *node = (tt_node *) start->user_data;
    semi_splay(node);
    if (has_at_most_one_incident_edge(vert)) return node;

    bool is_left = (start->endpoints[0] == vert) != node->flip;
    bool is_middle = false;
    bool is_right = (start->endpoints[1] == vert) != node->flip;
    tt_node *last_middle_node = NULL;

    while (node->parent) {
        tt_int_node *parent = node->parent;
        bool is_left_child = parent->children[0] == node;

        is_middle = is_left_child
                        ? (is_right || (is_middle && !has_right_boundary(node)))
                        : (is_left || (is_middle && !has_left_boundary(node)));
        is_left = (is_left_child != parent->info.flip) && !is_middle;
        is_right = (is_left_child == parent->info.flip) && !is_middle;

        node = (tt_node *) parent;

        if (is_middle) {
            if (!has_middle_boundary(node)) {
                return node;
            }
            last_middle_node = node;
        }
    }
    return last_middle_node;
}

static tt_node *prepare_expose(tt_node *consuming_node) {
    tt_node *node = consuming_node;
    while (node->parent) {
        tt_int_node *parent = node->parent;
        if (is_point(node)) {
            node = (tt_node *) parent;
        } else {
            tt_int_node *int_node = (tt_int_node *) node;

            push_flip(parent);
            push_flip(int_node);

            tt_node *sibling = get_sibling(node);
            int sibling_idx = parent->children[1] == sibling;
            tt_node *same_side_child = int_node->children[sibling_idx];
            if (is_path(same_side_child) || is_point(sibling)) {
                tt_node *other_side_child = int_node->children[1 - sibling_idx];
                rotate_up(other_side_child);
                if (node == consuming_node) {
                    consuming_node = (tt_node *) parent;
                }
                node = (tt_node *) parent;
            } else {
                tt_node *uncle = get_sibling((tt_node *) parent);
                tt_int_node *gparent = parent->info.parent;
                int uncle_idx = gparent->children[1] == uncle;

                if (sibling_idx == uncle_idx) {
                    rotate_up(node);
                } else {
                    rotate_up(sibling);
                }
            }
        }
    }
    return consuming_node;
}

tt_node *expose_prepared(tt_node *consuming_node) {
    bool from_left = false;
    bool from_right = false;
    tt_node *node = consuming_node;
    while (true) {
        node->num_boundary += 1;
        recompute_spine_weight(node);
        tt_int_node *parent = node->parent;
        if (!parent) return node;

        bool is_left_child_of_parent = parent->children[0] == node;
        bool is_right_child_of_parent = !is_left_child_of_parent;

        if ((is_left_child_of_parent && from_right)
            || (is_right_child_of_parent && from_left)) {
            node->flip ^= true;
        }

        from_left = is_left_child_of_parent != parent->info.flip;
        from_right = is_right_child_of_parent != parent->info.flip;
        node = (tt_node *) parent;
    }
}

tt_node *expose2(struct vertex *vert) {
    tt_node *consuming_node = find_consuming_node(vert);
    if (consuming_node) {
        consuming_node = prepare_expose(consuming_node);
        tt_node *root = expose_prepared(consuming_node);
        vert->is_exposed = true;
        return root;
    } else {
        vert->is_exposed = true;
        return NULL;
    }
}

tt_node *expose(struct vertex *vert) {
    tt_node *node = find_consuming_node(vert);
    if (!node) {
        vert->is_exposed = true;
        return NULL;
    }

    while (is_path(node)) {
        tt_int_node *int_node = (tt_int_node *) node;
        tt_int_node *parent = node->parent;
        push_flip(int_node);
        int node_idx = parent->children[1] == node;
        rotate_up(int_node->children[node_idx]);
        node = (tt_node *) parent;
    }

    full_splay(node);

    tt_node *root = NULL;
    while (node) {
        root = node;
        root->num_boundary += 1;
        recompute_spine_weight(node);
        node = (tt_node *) root->parent;
    }
    vert->is_exposed = true;
    return root;
}

tt_node *deexpose(struct vertex *vert) {
    tt_node *root = NULL;
    tt_node *node = find_consuming_node(vert);
    while (node) {
        root = node;
        root->num_boundary -= 1;
        recompute_spine_weight(root);
        node = (tt_node *) root->parent;
    }
    vert->is_exposed = false;
    return root;
}

tt_node *toplink(struct vertex *u, struct vertex *v, int weight) {
    tt_leaf_node *T_edge = NULL;
    tt_int_node *Tu_new = NULL;
    tt_int_node *Tv_new = NULL;
    struct edge *edge = NULL;

    T_edge = (tt_leaf_node *) malloc(sizeof(tt_leaf_node));
    if (!T_edge) return NULL;
    if (u->first_edge) {
        Tu_new = (tt_int_node *) malloc(sizeof(tt_int_node));
        if (!Tu_new) {
            printf("wrong\n");
            return NULL;
        }
    }
    if (v->first_edge) {
        Tv_new = (tt_int_node *) malloc(sizeof(tt_int_node));
        if (!Tv_new) {
            printf("wrong\n");
            return NULL;
        }
    }
    edge = (struct edge *) malloc(sizeof(struct edge));
    if (!edge) {
        printf("wrong\n");
        return NULL;
    }

    tt_node *Tu = expose(u);
    if (Tu && has_left_boundary(Tu))
        Tu->flip = !Tu->flip;
    u->is_exposed = false;

    tt_node *Tv = expose(v);
    if (Tv && has_right_boundary(Tv))
        Tv->flip = !Tv->flip;
    v->is_exposed = false;

    add_edge(edge, u, v, weight);
    edge->user_data = T_edge;
    T_edge->edge = edge;
    T_edge->info.parent = NULL;
    T_edge->info.is_leaf = true;
    T_edge->info.edgenum = 1;
    T_edge->info.flip = 0;
    T_edge->info.num_boundary = (!!Tu) + (!!Tv);
    tt_node *T = (tt_node *) T_edge;
    T->edgenum = 1;
    recompute_spine_weight(T);

    if (Tu) {
        Tu_new->info.parent = NULL;
        Tu_new->info.is_leaf = false;
        int a = Tu->edgenum, b = T->edgenum;
        Tu_new->info.flip = 0;
        Tu_new->info.num_boundary = !!Tv;
        Tu_new->children[0] = Tu;
        Tu_new->children[1] = T;
        Tu->parent = Tu_new;
        T->parent = Tu_new;
        T = (tt_node *) Tu_new;
        T->edgenum = a + b;
        recompute_spine_weight(T);
    }
    if (Tv) {
        Tv_new->info.parent = NULL;
        Tv_new->info.is_leaf = false;
        int a = Tv->edgenum, b = T->edgenum;
        Tv_new->info.flip = 0;
        Tv_new->info.num_boundary = 0;
        Tv_new->children[0] = T;
        Tv_new->children[1] = Tv;
        T->parent = Tv_new;
        Tv->parent = Tv_new;
        T = (tt_node *) Tv_new;
        T->edgenum = a + b;
        recompute_spine_weight(T);
    }

    return T;
    }

static void delete_all_ancestors(tt_node *node) {
    tt_node *p = (tt_node *) node->parent;
    if (p) {
        tt_node *s = get_sibling(node);
        delete_all_ancestors(p);
        s->parent = NULL;
    }
    free(node);
}

tt_node *topcut(struct edge *edge) {
    tt_node *node = (tt_node *) edge->user_data;
    struct vertex *u = edge->endpoints[0];
    struct vertex *v = edge->endpoints[1];
    full_splay(node);
    delete_all_ancestors(node);
    destroy_edge(edge);
    u->is_exposed = true;
    v->is_exposed = true;

    tt_node *root1 = deexpose(u);
    tt_node *root2 = deexpose(v);
    if (root1) {
        if (!root2)
            return root2;
    }
    if (root2) {
        if (!root1)
            return root1;
    }
    if (!root1) {
        if (!root2)
            return root1;
    }

    if (root1->edgenum < root2->edgenum) {
        return root1;
    } else {
        return root2;
    }

    }

static void free_top_tree(tt_node *node) {
    if (node->is_leaf) {
        tt_leaf_node *leaf = (tt_leaf_node *) node;
        leaf->edge->user_data = NULL;
    } else {
        tt_int_node *int_node = (tt_int_node *) node;
        free_top_tree(int_node->children[0]);
        free_top_tree(int_node->children[1]);
    }
    free(node);
}

void destroy_top_tree_containing_edge(struct edge *edge) {
    if (!edge) return;
    if (!edge->user_data) return;
    free_top_tree(find_root((tt_node *) edge->user_data));
}

void TopTree::create_toptree(int vertex_count) {
    F = create_tree(vertex_count + 1);
    nodes = F.vertices;
    Fend = nodes + F.num_vertices;
    vnb.resize(vertex_count + 1);
    used.resize(vertex_count + 1, false);
}

double TopTree::get_index_size_in_KB() {
    return F.num_vertices * 9 / 8 / 1024.0;
}

TopTree::~TopTree() {
    for (struct vertex *vert = nodes; vert < Fend; ++vert)
        destroy_top_tree_containing_edge(vert->first_edge);
    destroy_tree(&F);
}

void TopTree::get_all_edges(vector<Edge> &all) {
    for (struct vertex *vert = nodes; vert < Fend; ++vert) {
        edge *e = vert->first_edge;
        while (e) {
            int u = e->endpoints[0] - nodes, v = e->endpoints[1] - nodes;

            int j = e->endpoints[1] == vert;

            if (vert == e->endpoints[0])
                all.push_back(holm::make_edge(u, v));
            e = e->next[j];
        }
    }
}

bool TopTree::is_connected(VId u, VId v) {
    tt_node *root1 = expose(&nodes[u]);
    tt_node *root2 = expose(&nodes[v]);
    bool r = false;
    if (root1 && (root1 == root2)) {
        r = true;
    }
    root1 = deexpose(&nodes[u]);
    root2 = deexpose(&nodes[v]);
    return r;
}

void TopTree::link(VId u, VId v) {
    if (u > v)
        swap(u, v);
    toplink(&nodes[u], &nodes[v], 1);
}

struct edge *TopTree::find_edge_toptree(VId u, VId v) {
    struct vertex *topu = &nodes[u];
    struct vertex *topv = &nodes[v];
    edge *e = topu->first_edge;
    while (e) {
        if (e->endpoints[1] == topv || e->endpoints[0] == topv)
            break;
        int j = e->endpoints[1] == topu;
        e = e->next[j];
    }
    return e;
}

bool TopTree::is_edge(VId u, VId v) {
    if (u > v)
        swap(u, v);
    struct vertex *topu = &nodes[u];
    struct vertex *topv = &nodes[v];
    edge *e = topu->first_edge;
    while (e) {
        if (e->endpoints[1] == topv || e->endpoints[0] == topv)
            break;
        int j = e->endpoints[1] == topu;
        e = e->next[j];
    }
    bool r = false;
    if (e)
        r = true;
    return r;
}

void TopTree::cut(VId u, VId v) {
    if (u > v)
        swap(u, v);
    struct edge *e = find_edge_toptree(u, v);
    if (e)
        topcut(e);
    else
        cout << "Not a toptree edge" << endl;
}


std::pair<VArrP, VSetP> TopTree::find_path_vset(VId u, VId v) {
    auto vec = make_VArr();
    auto path = make_VSet();
    path->clear();
    vec->clear();
    if (!is_connected(u, v))
        return {vec, path};
    tt_node *root1 = expose(&nodes[u]);
    tt_node *root2 = expose(&nodes[v]);

    if (root1 != root2)
        return {vec, path};

    tt_node *node = root1;
    queue<tt_node *> q;
    q.push(node);
    while (q.size() != 0) {
        tt_node *now_node = q.front();
        q.pop();
        if (now_node->num_boundary != 2)
            continue;
        if (!now_node->is_leaf) {
            tt_int_node *int_node = (tt_int_node *) now_node;
            q.push(int_node->children[0]);
            q.push(int_node->children[1]);
        } else {
            tt_leaf_node *l_node = (tt_leaf_node *) now_node;
            int ui = l_node->edge->endpoints[0] - nodes;
            int vi = l_node->edge->endpoints[1] - nodes;
            vnb[ui].push_back(vi);
            vnb[vi].push_back(ui);
        }
    }
    deexpose(&nodes[u]);
    deexpose(&nodes[v]);
    used[u] = true;
    int f = u;
    vec->push_back(u);

    while (f != v) {
        for (auto k: vnb[f]) {
            if (!used[k]) {
                used[k] = true;
                vec->push_back(k);
                f = k;
                break;
            }
        }
    }

    for (int k = 0; k < vec->size(); k++) {
        vnb[vec->at(k)].clear();
        used[vec->at(k)] = false;
    }
    path->insert(vec->begin(), vec->end());
    return {vec, path};
}


VArrP TopTree::find_path(VId u, VId v) {
    auto path = std::make_shared<std::vector<int> >();
    path->clear();
    if (!is_connected(u, v))
        return path;
    tt_node *root1 = expose(&nodes[u]);
    tt_node *root2 = expose(&nodes[v]);

    tt_node *node = root1;
    queue<tt_node *> q;
    q.push(node);
    while (q.size() != 0) {
        tt_node *now_node = q.front();
        q.pop();
        if (now_node) {
            if (now_node->num_boundary != 2)
                continue;
            if (!now_node->is_leaf) {
                tt_int_node *int_node = (tt_int_node *) now_node;
                q.push(int_node->children[0]);
                q.push(int_node->children[1]);
            } else {
                tt_leaf_node *l_node = (tt_leaf_node *) now_node;
                int ui = l_node->edge->endpoints[0] - nodes;
                int vi = l_node->edge->endpoints[1] - nodes;
                vnb[ui].push_back(vi);
                vnb[vi].push_back(ui);
            }
        }
    }
    deexpose(&nodes[u]);
    deexpose(&nodes[v]);

    used[u] = true;
    int f = u;
    path->push_back(u);

    while (f != v) {
        for (auto k: vnb[f]) {
            if (!used[k]) {
                used[k] = true;
                path->push_back(k);
                f = k;
                break;
            }
        }
    }
    for (int k = 0; k < path->size(); k++) {
        vnb[path->at(k)].clear();
        used[path->at(k)] = false;
    }
    return path;
}