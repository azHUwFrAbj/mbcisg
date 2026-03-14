#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include "my_types.h"
using namespace std;

struct tree {
    size_t num_vertices;
    struct vertex *vertices;
};

struct vertex {
    struct edge *first_edge;
    bool is_exposed;
};

struct edge {
    int weight;
    void *user_data;
    struct vertex *endpoints[2];
    struct edge *prev[2];
    struct edge *next[2];
};

struct tree create_tree(size_t num_vertices);

void add_edge(struct edge *allocation, struct vertex *left, struct vertex *right, int weight);

void destroy_edge(struct edge *edge);

void destroy_tree(struct tree *to_destroy);

bool has_at_most_one_incident_edge(struct vertex *vertex);

#endif