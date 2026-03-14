

#ifndef MXT_H
#define MXT_H
#include <unordered_map>

#include "utils.h"


class MXT {
public:
    struct Node {
        int size_ = 1;
        VId vid_ = -1;
        int timestamp_ = -1;
        Node *parent_ = nullptr;
        std::set<Node *> nbr_;

        int left_ = 1;
        int right_ = 2;
        Node *root_ = this;
    };

    explicit MXT(int N);;

    ~MXT();

    TEdge insert_edge_and_get_t_path(const Edge &eg, int timestamp, std::vector<TEdge> &path);

    void dfs_mark_parent(const Edge &edge);

    void dfs_mark_child(const Edge &edge);

    const int N_;

    std::unordered_map<VId, Node *> vid2_nodes_;

    mutable std::vector<Node *> path;
    mutable std::vector<int> weights;

public:
    double get_index_size_in_KB();

    std::pair<Edge, Edge> find_path_edge(VId uid, VId vid) const;

    std::pair<int, int> find_path_edge_timestamp_qo(VId vid, VId wid) const;

    std::pair<int, int> find_path_edge_timestamp(VId vid, VId wid) const;

    void swap_in(const Edge &old_edge, const Edge &new_edge, int timestamp);

    TEdge insert_edge(VId vid, VId wid, int timestamp, std::vector<Edge> &path);

    void direct_delete(VId uid, VId vid);

    bool is_nbr(VId uid, VId vid) const;

    void get_nbr_timestamps(VId uid, std::vector<VId> &nbr) const;

private:
    void link_roots(VId uid, VId vid, int weight);

    void link(VId uid, VId vid, int weight);

    void unlink(VId uid, VId vid);

    void make_root(VId uid);

    void make_root(Node *u);
};


#endif