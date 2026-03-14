

#include <cassert>

#include "BCCInfo.h"
#include <iostream>

#include "BasicTree.h"
#include "commons.h"

void testMergeBCC() {
    BasicTree tree;
    for (int i = 1; i <= 5; i++) {
        tree.add_node(i);
    }
    BCCInfo bccInfo(1, tree);
    tree.link(1, 2);
    tree.link(2, 3);

    auto pt = tree.find_path(1, 3);
    bccInfo.cover(pt, 0);

    tree.link(4, 5);
    auto pt2 = tree.find_path(4, 5);
    bccInfo.cover(pt2, 0);

    auto bcc1 = bccInfo.get_bcc(make_edge(4, 5), 0);
    auto bcc2 = bccInfo.get_bcc(make_edge(2, 3), 0);

    auto merged = bccInfo.merge_bcc(bcc1, bcc2, 0);
    std::cout << "MergeBCC Test: " << (merged->size() == 3 ? "PASSED" : "FAILED") << std::endl;
    assert(bccInfo.get_bcc_size(make_edge(4, 5), 0) == 4 && "Merge BCC Fail");
}

void testCoverPath() {
    BasicTree tree;
    BCCInfo bccInfo(1, tree);
    for (int i = 1; i <= 5; i++) {
        tree.add_node(i);
    }
    tree.link(1, 2);
    tree.link(2, 3);

    VArrP path = tree.find_path(1, 3);
    bccInfo.cover(path, 0);
    auto bcc = bccInfo.get_bcc(make_edge(1, 2), 0);
    bool result = bccInfo.get_bcc_size(make_edge(1, 2), 0) == 3;
    std::cout << "CoverPath Test: " << (result ? "PASSED" : "FAILED") <<
            std::endl;
    assert(result && "Fail at cover path test");
}

void testRemoveFromBCC() {
    BasicTree tree;
    BCCInfo bccInfo(2, tree);
    for (int i = 1; i <= 5; i++) {
        tree.add_node(i);
    }
    tree.link(1, 2);
    tree.link(2, 3);

    VArrP path = tree.find_path(1, 3);
    bccInfo.cover(path, 0);

    Edge e = make_edge(1, 2);
    auto bcc = bccInfo.get_bcc(e, 0);
    std::cout << bcc->size() << std::endl;
    auto size = bccInfo.get_bcc_size(e, 0);
    bool before = size == 3;
    bccInfo.remove_from_bcc(e, 0);
    size = bccInfo.get_bcc_size(e, 0);
    bool after = size == 2;
    std::cout << "RemoveFromBCC Test: " << (before && after ? "PASSED" : "FAILED") << std::endl;
    assert(before && after && "Fail at removal test");
}

void testSwap() {
    BasicTree tree;
    for (int i = 1; i <= 5; i++) {
        tree.add_node(i);
    }
    BCCInfo bccInfo(10, tree);
    tree.link(1, 2);
    tree.link(2, 3);
    tree.link(3, 4);
    tree.link(4, 5);

    auto pt = tree.find_path(1, 5);
    bccInfo.cover(pt, 0);
    bccInfo.cover(pt, 1);

    auto bcc1 = bccInfo.get_bcc(make_edge(2, 3), 0);

    bool result = bcc1->size() == 4;
    auto e1 = make_edge(2, 3);
    auto e2 = make_edge(1, 5);
    bccInfo.swap(e1, e2, 1);

    auto bcc2 = bccInfo.get_bcc(make_edge(2, 3), 0);

    bool result2 = bcc2->size() == 1;

    std::cout << "Swap Test: " << (result && result2 ? "PASSED" : "FAILED") << std::endl;
    assert(result && "Fail at swap test");
}

int main() {
    testRemoveFromBCC();
    testCoverPath();
    testMergeBCC();
    testSwap();
    return 0;
}