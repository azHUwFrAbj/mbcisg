#include "tree.h"


struct tree create_tree(size_t num_vertices) {
    struct tree output;
    output.num_vertices = num_vertices;
    output.vertices = (vertex *) malloc(num_vertices * sizeof(struct vertex));

    if (output.vertices == NULL)
        output.num_vertices = 0;

    struct vertex *end = output.vertices + output.num_vertices;
    for (struct vertex *v = output.vertices; v < end; ++v) {
        v->first_edge = NULL;
        v->is_exposed = false;
    }

    return output;
}

void destroy_tree(struct tree *to_destroy) {
    struct vertex *end = to_destroy->vertices + to_destroy->num_vertices;
    for (struct vertex *v = to_destroy->vertices; v < end; ++v)
        while (v->first_edge)
            destroy_edge(v->first_edge);
    free(to_destroy->vertices);
    to_destroy->vertices = NULL;
}

static inline void destroy_edge_inner(struct vertex *v, struct edge *prev, struct edge *next) {
    if (prev) {
        int j = prev->endpoints[1] == v;
        prev->next[j] = next;
    } else {
        v->first_edge = next;
    }
    if (next) {
        int j = next->endpoints[1] == v;
        next->prev[j] = prev;
    }
}

void destroy_edge(struct edge *edge) {
    destroy_edge_inner(edge->endpoints[0], edge->prev[0], edge->next[0]);
    destroy_edge_inner(edge->endpoints[1], edge->prev[1], edge->next[1]);
    free(edge);
}

void add_edge(struct edge *edge, struct vertex *left, struct vertex *right, int weight) {
    struct vertex *vert[2] = {left, right};
    struct edge *next[2] = {left->first_edge, right->first_edge};

    for (int i = 0; i < 2; ++i) {
        vert[i]->first_edge = edge;
        if (next[i]) {
            int j = next[i]->endpoints[1] == vert[i];
            next[i]->prev[j] = edge;
        }
    }

    edge->weight = weight;
    edge->user_data = NULL;
    edge->endpoints[0] = vert[0];
    edge->endpoints[1] = vert[1];
    edge->prev[0] = NULL;
    edge->prev[1] = NULL;
    edge->next[0] = next[0];
    edge->next[1] = next[1];
}

bool has_at_most_one_incident_edge(struct vertex *vertex) {
    struct edge *first_edge = vertex->first_edge;
    if (first_edge) {
        int j = first_edge->endpoints[1] == vertex;
        return first_edge->next[j] == NULL;
    } else {
        return true;
    }
}