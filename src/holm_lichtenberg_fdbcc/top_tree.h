#ifndef TOP_TREE_H
#define TOP_TREE_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "commons.h"
#include "my_types.h"
#include "tree.h"
#include <vector>
#include <queue>
#include <set>
#include <iostream>

using namespace std;


typedef struct tt_node_struct tt_node;
typedef struct tt_int_node_struct tt_int_node;
typedef struct tt_leaf_node_struct tt_leaf_node;

struct tt_node_struct {
    tt_int_node *parent;
    int spine_weight;
    int edgenum;
    unsigned char is_leaf: 1;
    unsigned char flip: 1;
    unsigned char num_boundary: 2;
};

struct tt_leaf_node_struct {
    tt_node info;
    struct edge *edge;
};

struct tt_int_node_struct {
    tt_node info;
    tt_node *children[2];
};

tt_node *topcut(struct edge *edge);

tt_node *toplink(struct vertex *u, struct vertex *v, int weight);

tt_node *deexpose(struct vertex *vert);

tt_node *expose(struct vertex *vert);

tt_node *expose2(struct vertex *vert);

tt_leaf_node *find_maximum(tt_node *root);

tt_node *find_root(tt_node *node);

void destroy_top_tree_containing_edge(struct edge *edge);

class TopTree {
private:
    struct tree F;
    vertex *nodes;
    vector<vector<VId> > vnb;
    vector<bool> used;
    vertex *Fend;

public:
    TopTree() {
    }

    ~TopTree();

    void create_toptree(int vertex_count);

    double get_index_size_in_KB();

    void link(VId u, VId v);

    void cut(VId u, VId v);

    bool is_connected(VId u, VId v);

    VArrP find_path(VId u, VId v);

    bool is_edge(VId u, VId v);

    std::pair<VArrP, VSetP> find_path_vset(VId u, VId v);

    void get_all_edges(vector<Edge> &all);

private:
    struct edge *find_edge_toptree(VId u, VId v);
};

#endif