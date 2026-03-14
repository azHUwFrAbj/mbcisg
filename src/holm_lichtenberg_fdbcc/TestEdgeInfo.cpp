

#include <iostream>
#include <cassert>
#include "EdgeInfo.h"

void test_EdgeInfo() {
    EdgeInfo edgeInfo;

    edgeInfo.set_level(1, 2, 5);
    assert(edgeInfo.get_level(1, 2) == 5);
    assert(edgeInfo.get_level(2, 1) == 5);
    std::cout << "set_level and get_level passed.\n";

    assert(edgeInfo.get_level(3, 4) == -1);
    std::cout << "Default get_level passed.\n";

    edgeInfo.set_cover_level(1, 2, 10);
    assert(edgeInfo.get_cover_level(1, 2) == 10);
    assert(edgeInfo.get_cover_level(2, 1) == 10);
    std::cout << "set_cover_level and get_cover_level passed.\n";

    assert(edgeInfo.get_cover_level(3, 4) == -1);
    std::cout << "Default get_cover_level passed.\n";

    edgeInfo.set_rep_edge(1, 2, {3, 4});
    Edge repEdge = edgeInfo.get_rep_edge(1, 2);
    assert(repEdge.first == 3 && repEdge.second == 4);
    repEdge = edgeInfo.get_rep_edge(2, 1);
    assert(repEdge.first == 3 && repEdge.second == 4);
    std::cout << "set_rep_edge and get_rep_edge passed.\n";

    repEdge = edgeInfo.get_rep_edge(3, 4);
    assert(repEdge.first == -1 && repEdge.second == -1);
    std::cout << "Default get_rep_edge passed.\n";

    edgeInfo.set_level(1, 2, 8);
    assert(edgeInfo.get_level(1, 2) == 8);
    edgeInfo.set_cover_level(1, 2, 12);
    assert(edgeInfo.get_cover_level(1, 2) == 12);
    edgeInfo.set_rep_edge(1, 2, {5, 6});
    repEdge = edgeInfo.get_rep_edge(1, 2);
    assert(repEdge.first == 5 && repEdge.second == 6);
    std::cout << "Updating values passed.\n";

    std::cout << "All tests passed!\n";
}

int main() {
    test_EdgeInfo();
    return 0;
}