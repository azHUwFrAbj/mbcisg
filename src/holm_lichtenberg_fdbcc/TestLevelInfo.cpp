

#include <iostream>
#include <cassert>

#include "LevelInfo.h"


void test_LevelInfo() {
    std::cout << "Running tests for LevelInfo...\n";

    LevelInfo levelInfo(3);

    levelInfo.addEdge(1, 2, 1);
    levelInfo.addEdge(2, 3, 1);

    auto nbrs1 = levelInfo.getNbrs(1, 1);
    assert(nbrs1->find(2) != nbrs1->end());
    std::cout << "addEdge and getNbrs passed for vertex 1.\n";

    auto nbrs2 = levelInfo.getNbrs(2, 1);
    assert(nbrs2->find(1) != nbrs2->end());
    assert(nbrs2->find(3) != nbrs2->end());
    std::cout << "addEdge and getNbrs passed for vertex 2.\n";

    assert(levelInfo.getVerticesNumber(0) == 0);
    assert(levelInfo.getVerticesNumber(1) == 3);

    levelInfo.removeEdge(1, 2, 1);

    assert(levelInfo.getVerticesNumber(1) == 2);
    std::cout << "getVerticesNumber passed the first round.\n";

    nbrs1 = levelInfo.getNbrs(1, 1);
    assert(nbrs1->find(2) == nbrs1->end());
    std::cout << "removeEdge passed for vertex 1.\n";

    nbrs2 = levelInfo.getNbrs(2, 1);
    assert(nbrs2->find(1) == nbrs2->end());
    std::cout << "removeEdge passed for vertex 2.\n";

    assert(nbrs2->find(3) != nbrs2->end());
    std::cout << "getNbrs passed after edge removal.\n";

    levelInfo.removeEdge(2, 3, 1);

    auto nbrs3 = levelInfo.getNbrs(2, 1);
    assert(nbrs3->empty());
    std::cout << "Empty vertices removal passed.\n";

    std::cout << "All tests passed!\n";
}

int main() {
    test_LevelInfo();
    return 0;
}