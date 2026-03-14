

#ifndef BFF_Q_H
#define BFF_Q_H
#include "BBF.h"
#include "MXT.h"


class BBFQ {
    BBF bbf_;
    MXT mxt_;

public:
    double get_index_size_in_KB();

    int get_bbf_node_count() const;

    int get_bbf_height() const;

    void get_bccs_cache_trick(vector<vector<VId> > &vbccs);

    void get_tree_edges_timestamps_of(VId vid, vector<int> &tree_edge_timestamps);

    void get_bcc_of_tree_edges(std::vector<std::vector<VId> > &bbcs, const vector<int> &tree_edge_timestamps);

    int get_root_count() const;

    void fresh_roots_vec();

    BBFQ(int vertex_count, int window_size) : mxt_(vertex_count), bbf_(window_size, vertex_count) {
    }

    BBFQ(int vertex_count, int window_size, int edge_number) : mxt_(vertex_count),
                                                               bbf_(window_size, edge_number, vertex_count) {
    }

    bool insert_basic(const Edge &new_edge, int timestamp);

    void insert_advance(const Edge &new_edge, int timestamp);

    void insert_adv_with_cache(const Edge &new_edge, int timestamp);

    void insert_advance_qo(const Edge &new_edge, int timestamp);

    void insert_batch(const std::vector<Edge> &new_edges, int first_timestamp);

    void insert_batch_qo(const std::vector<Edge> &new_edges, int first_timestamp);

    void insert_batch_bottom_up_stepwise(const std::vector<Edge> &new_edges, int first_timestamp, int batch_size);

    void insert_batch_bottom_up(const std::vector<Edge> &new_edges, int first_timestamp);

    void expire(int expired_timestamp);

    void expire_bbf(int expired_timestamp, std::vector<Edge> &expired_edges);

    void expire_mxt(std::vector<Edge> &expired_edges);

    void expire_qo(int expired_timestamp);

    void get_bccs(BCCs &bccs);

    void get_bccs(vector<vector<VId> > &bccs);

    void get_bccs_label_based();

    bool is_biconnected(VId vid, VId wid);

    bool is_biconnected_qo(VId vid, VId wid);

    bool is_biconnected_qo2(VId vid, VId wid);
};


#endif