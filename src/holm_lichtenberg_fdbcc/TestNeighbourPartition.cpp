

#include "NeighbourPartition.h"
#include <iostream>
#include <cassert>

void test_NeighbourPartition() {
    NeighbourPartition np;

    try {
        np.init(1, 2, 3);
        np.init(1, 3, 3);
        std::cout << "Initialization successful.\n";
    } catch (const std::exception &e) {
        std::cerr << "Initialization failed: " << e.what() << '\n';
        return;
    }

    try {
        auto nbp = np.getNbp(1, 2, 0);
        assert(nbp->find(2) != nbp->end());
        std::cout << "getNbp passed: Neighbor found at level 0.\n";
    } catch (const std::exception &e) {
        std::cerr << "getNbp failed: " << e.what() << '\n';
    }

    try {
        bool isTransitive = np.isTransitiveWedge(2, 1, 3, 0);
        assert(isTransitive == false);
        std::cout << "isTransitiveWedge result before union: false" << '\n';
    } catch (const std::exception &e) {
        std::cerr << "isTransitiveWedge failed: " << e.what() << '\n';
    }

    try {
        np.unionNbp(2, 1, 3, 0);
        bool isTransitive = np.isTransitiveWedge(2, 1, 3, 0);
        assert(isTransitive == true);
        std::cout << "isTransitiveWedge result after union: true" << '\n';
    } catch (const std::exception &e) {
        std::cerr << "unionNbp failed: " << e.what() << '\n';
    }

    try {
        np.remove_from_neighbours(1, 2, 3);
        auto nbp = np.getNbp(1, 2, 0);
        assert(nbp->empty());
        std::cout << "removeFromNeighbours passed: Neighbor removed.\n";
    } catch (const std::exception &e) {
        std::cerr << "removeFromNeighbours failed: " << e.what() << '\n';
    }
}

int main() {
    test_NeighbourPartition();
    return 0;
}