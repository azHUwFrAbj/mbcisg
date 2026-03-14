

#ifndef BLOCKCUTTREE_H
#define BLOCKCUTTREE_H
#include <queue>
#include <set>

#include "BlockCutTreeNode.h"
#include "HopcroftTarjan.h"
#include "utils.h"


struct BlockCutTreeNode;

class BlockCutTree {
private:
    std::deque<Edge> edges_;

    const int N_;

    std::vector<BlockCutTreeNode *> square_nodes_;

    std::vector<BlockCutTreeNode *> round_nodes_;
    std::vector<BlockCutTreeNode *> free_round_nodes_;


public:
    BlockCutTree() = delete;

    explicit BlockCutTree(int N);;

    ~BlockCutTree();

    void build_by_bccs(const std::vector<std::shared_ptr<std::set<int> > > &bccs);

    void build();

    void remove_bridge(Edge eg);

    void build(const std::set<VId> &affected_vertices, const std::vector<Edge> &edges_to_delete);

    bool bcc_query(int vid, int wid) const;

    bool is_bridge(Edge eg) const;

    void get_bcc(Edge eg, BCC &bcc) const;

    void just_add_edge(int vid, int wid);

    Edge front_edge() const;

    Edge pop_front_edge();

    void insert_update(int vid, int wid);

    bool cc_query(int vid, int wid) const;

    std::set<int> get_cc(int vid) const;

    std::pair<PVec<BlockCutTreeNode *>, int> find_path(int vid, int wid) const;

    bool has_edge() const;

    void get_bccs(BCCs &bccs) const;

    void just_pop_edge();

    BlockCutTreeNode *get_bcc_label(Edge eg) const;

    void run_ht_algorithm(std::vector<std::vector<VId> > &bbcs);

    void run_ht_algorithm_label_based();

private:
    static void link(BlockCutTreeNode *a, BlockCutTreeNode *b);

    BlockCutTreeNode *new_round_node();

    static void evert(BlockCutTreeNode *node);


    void clean();

    void clean(const std::set<BlockCutTreeNode *> &affected_round_nodes);

    void delete_round_node(BlockCutTreeNode *node);
};

#endif