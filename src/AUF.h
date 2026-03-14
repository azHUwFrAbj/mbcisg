

#ifndef AUF_H
#define AUF_H

#include <cassert>
#include <map>
#include <unordered_map>
#include <vector>
#include <vector>

#include "BBF.h"
#include "utils.h"


class AUF {
public:
    struct AUF_Node {
        BBF::Node *parent;
        BBF::Node *anchor;
        int rank = 0;
    };

    std::vector<AUF_Node> auf_nodes;

    const int t_s_;
    const int t_e_;

    explicit AUF(int t_s, int t_e) : t_s_(t_s), t_e_(t_e + 3) {
        assert(t_s_ < t_e_);
        auf_nodes = std::vector<AUF_Node>(t_e_ - t_s_ + 3, AUF_Node{nullptr, nullptr, 0});
    };


    AUF_Node &get_node(BBF::Node *node);

    void set_node(BBF::Node *node, AUF_Node value);

    bool is_in(BBF::Node *node);

    void make_set(BBF::Node *x);

    BBF::Node *find(BBF::Node *x);

    void union_sets(BBF::Node *x, BBF::Node *y);


    BBF::Node *get_anchor(BBF::Node *x);
};


#endif