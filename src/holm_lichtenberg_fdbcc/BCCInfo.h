

#ifndef BCCINFO_H
#define BCCINFO_H
#include "my_types.h"


class BasicTree;

class BCCInfo {
    using BCC = std::set<Edge>;
    using BCC_P = std::shared_ptr<BCC>;
    std::vector<std::unordered_map<Edge, BCC_P, EdgeHash> > l2bcm;

public:
    BCCInfo() = delete;

    explicit BCCInfo(int lmax) : l2bcm(lmax) {
    }

    BCC_P merge_bcc(BCC_P bcc1, BCC_P bcc2, int l);

    void cover(VArrP path, int l);


    void remove_from_bcc(Edge e, int l);

    int get_bcc_size(Edge e, int l);

    void add_to_bcc(Edge eg, BCC_P bcc);

    BCC_P get_bcc(Edge e, int l);

    BCC_P get_bcc_deep(Edge e, int l);

    void swap(Edge tree_edge, Edge rep_edge, int l);

    int get_new_bcc_size(VArrP path, int l);

private:
    void add_to_bcc(Edge e, int l, BCC_P bcc);

    void set_bcc(Edge e, int l, BCC_P bcc);

    BCC_P make_BCC();

    BCC_P make_BCC(Edge e);
};


#endif