

#ifndef BFF_H
#define BFF_H
#include <deque>
#include <tree.h>
#include <unordered_set>

#include "BbfNode.h"
#include "NodePool.h"
#include "utils.h"


class BBF {
public:
    using Node = BbfNode;

    NodePool node_pool_;

    const bool use_pool_;

    using Time_Max_Queue = std::priority_queue<Node *, std::vector<Node *>, Compare_Timestamp>;

    int t_start_ = 0;
    int t_max_ = 0;

private:
    std::deque<Node *> tree_node_queue_;

    std::unordered_set<Node *> roots;

    std::vector<Node *> roots_vec_;
    int roots_vec_timestamp_ = -1;


    const int THETA_;

    const int NODE_NUMBER_;

    std::vector<int> visited_edge_timestamp_;

    int retrival_epoch_id_ = -1;
    std::vector<int> vertex_retrival_mark_;
    mutable std::deque<Node *> bfs_deque_;

    int label_id = 0;
    std::vector<int> vertex_label_;

public:
    BBF(int theta, int n) : NODE_NUMBER_(n + 1), THETA_(theta + 100), visited_edge_timestamp_(THETA_, -1),
                            vertex_retrival_mark_(NODE_NUMBER_, -1), node_pool_(0), use_pool_(false),
                            vertex_label_(NODE_NUMBER_, 0) {
    }

    BBF(int theta, int edge_number, int n) : NODE_NUMBER_(n + 1), THETA_(theta + 100),
                                             visited_edge_timestamp_(THETA_, -1),
                                             vertex_retrival_mark_(NODE_NUMBER_, -1), node_pool_(edge_number),
                                             use_pool_(true), vertex_label_(NODE_NUMBER_, 0) {
    }

    ~BBF();

    void insert_edge_basic(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                           const std::vector<TEdge> &path);

    int get_bbf_height() const;

    void get_bcc_in_subtree_and_update_cache_and_update_roots(Node *r, std::vector<int> &acc);

    void get_bcc_in_subtree_and_update_cache(Node *r, std::vector<int> &acc);

    void get_bccs_cache_trick(vector<vector<VId> > &vbccs);

    void get_bcc_in_subtree(BbfNode *node, vector<VId> &bcc);

    void get_bcc_in_subtree_and_mark_root(BbfNode *r, vector<VId> &bcc);

    void clean_deleted_roots();

    int get_root_count() const;

    void get_bccs_label();

    void get_bccs_in_subtree_label_based(BbfNode *node);

    void fresh_roots_vec();

    void get_bcc_of_tree_edges(std::vector<std::vector<VId> > &bccs, const vector<int> &edge_timestamps);


    static void mark_leaves(Node *last_node);

    void insert_edge_adv(const Edge &new_edge, int timestamp, const TEdge &victim_edge, const std::vector<TEdge> &path);

    void insert_edge_adv_wc(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                            const std::vector<TEdge> &path);

    void insert_edge_adv_oq(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                            const std::vector<TEdge> &path);

    void bottom_up_maintenance(Time_Max_Queue &Q);


    void expire(int expired_timestamp);

    void expire(int expired_timestamp, std::vector<Edge> &expired_tree_edges);

    void expire_qo(int expired_timestamp, std::vector<Edge> &expired_tree_edges);

    void get_bccs(vector<vector<VId> > &bccs);

    bool bcc_query(int vid, int wid) const;

    void get_bccs(BCCs &bccs);

    bool has_lca(int timestamp_v, int timestamp_w);

    bool has_lca_qo(int timestamp_v, int timestamp_w);

    void add_isolated_node(const Edge &eg, int timestamp);

    static BBF::Node *get_a_parent_other_than_y(BBF::Node *x, BBF::Node *y);

    bool is_tree_and_no_deleted_root() const;

    bool is_no_single_child_and_no_deleted_child() const;

    int count_nodes() const;

    double get_index_size_in_KB();

private:
    struct Stat {
        int height = 0;
        int nodes_count = 0;
    };

    Stat get_statistics() const;

    std::unordered_map<int, Node *> edge_to_node_;

    Node *get_node(int t) const;

    bool is_register_timestamp(int t) const;

    void register_node(Node *n);

    void unregister_node(Node *y);


    Node *make_new_node(const Edge &edge, int timestamp);

    void del_node(Node *n);

    bool is_visited(Node *node) const;

    void mark_visited(Node *node);

    void mark_unvisited(Node *node);

    void build_BEG(const TEdge &victim, const std::vector<TEdge> &edges, std::vector<Node *> &multi_parent_nodes);

    Node *swing(Node *x);

    static bool is_ancestor(Node *descendant, Node *ancestor);

    Node *swing_adv(Node *x, Node *victim_node);

    void recursive_cut(Node *x, Node *y);

    void cut(Node *x, Node *y);

    void link(Node *x, Node *y);

    static void mark_intermediate_node(Node *victim_node);

public:
    void build_BEG(const TEdge &victim, const std::vector<TEdge> &path, Time_Max_Queue &Q);
};


#endif