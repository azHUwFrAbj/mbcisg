

#include "BCCInfo.h"

#include <cassert>

#include "BasicTree.h"
#include "commons.h"


BCCInfo::BCC_P BCCInfo::merge_bcc(BCC_P bcc1, BCC_P bcc2, int l) {
    if (bcc1 == bcc2) {
        return bcc1;
    }

    if (bcc1->size() > bcc2->size()) {
        std::swap(bcc1, bcc2);
    }

    for (auto p: *bcc1) {
        add_to_bcc(p, bcc2);
        set_bcc(p, l, bcc2);
    }
    return bcc2;
}

void BCCInfo::cover(VArrP path, int l) {
    if (path->size() < 2) {
        return;
    }

    for (int i = 0; i < l; i++) {
        auto head_edge = holm::make_edge(path->at(0), path->at(1));
        auto head_bcc = get_bcc(head_edge, l);
        for (int i = 1; i < path->size() - 1; i++) {
            auto edge = holm::make_edge(path->at(i), path->at(i + 1));
            auto bcc = get_bcc(edge, l);
            head_bcc = merge_bcc(head_bcc, bcc, l);
        }
    }
}

void BCCInfo::remove_from_bcc(Edge e, int l) {
    auto bcc = get_bcc(e, l);
    bcc->erase(e);
    auto new_bcc = make_BCC(e);
    set_bcc(e, l, new_bcc);
}


int BCCInfo::get_bcc_size(Edge e, int l) {
    return 1 + get_bcc(e, l)->size();
}

void BCCInfo::add_to_bcc(Edge eg, BCCInfo::BCC_P bcc) {
    bcc->insert(eg);
}

void BCCInfo::add_to_bcc(Edge e, int l, BCC_P bcc) {
    bcc->insert(e);
    set_bcc(e, l, bcc);
}


BCCInfo::BCC_P BCCInfo::get_bcc(Edge e, int l) {
    auto &bccs = l2bcm.at(l);
    if (bccs.contains(e)) {
        return bccs.at(e);
    }
    auto b = make_BCC(e);
    bccs[e] = b;
    return b;
}

BCCInfo::BCC_P BCCInfo::get_bcc_deep(Edge e, int l) {
    auto &bccs = l2bcm.at(l);
    if (!bccs.contains(e)) {
        auto b = make_BCC(e);
        bccs[e] = b;
    }
    auto bcc = bccs.at(e);
    auto result = make_BCC();
    for (auto edge: *bcc) {
        result->insert(edge);
    }
    return result;
}

void BCCInfo::swap(Edge tree_edge, Edge rep_edge, int l) {
    for (int i = 0; i <= l; i++) {
        auto bcc = get_bcc(tree_edge, i);
        remove_from_bcc(tree_edge, i);
        add_to_bcc(rep_edge, i, bcc);
    }
}

int BCCInfo::get_new_bcc_size(VArrP path, int l) {
    auto max_bcc = make_BCC();
    int max_size = 0;
    for (int i = 0; i < path->size() - 1; i++) {
        auto e = holm::make_edge(path->at(i), path->at(i + 1));
        auto bcc = get_bcc(e, l);
        if (bcc->size() > max_size) {
            max_bcc = bcc;
            max_size = max_bcc->size();
        }
    }
    for (int i = 0; i < path->size() - 1; i++) {
        auto eg = holm::make_edge(path->at(i), path->at(i + 1));
        max_size += !(max_bcc->contains(eg));
    }
    return max_size + 1;
}

void BCCInfo::set_bcc(Edge e, int l, BCC_P bcc) {
    l2bcm.at(l)[e] = bcc;
}

BCCInfo::BCC_P BCCInfo::make_BCC() {
    auto s = std::make_shared<BCC>();
    return s;
}


BCCInfo::BCC_P BCCInfo::make_BCC(Edge e) {
    auto s = std::make_shared<BCC>();
    s->insert(e);
    return s;
}