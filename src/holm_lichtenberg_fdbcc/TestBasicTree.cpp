

#include "BasicTree.h"
#include "TreeNode.h"
#include <iostream>
#include <cassert>

void test_BasicTree() {
    BasicTree tree;

    try {
        tree.add_node(1);
        tree.add_node(2);
        tree.add_node(3);
        std::cout << "Nodes added successfully.\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }

    try {
        tree.add_node(1);
    } catch (const std::exception &e) {
        std::cout << "Duplicate node error caught: " << e.what() << '\n';
    }

    try {
        tree.link(1, 2);
        tree.link(3, 2);
        std::cout << "linked successfully.\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }

    try {
        auto path = tree.find_path(3, 1);
        std::cout << "Path found: ";
        for (int id: *path) {
            std::cout << id << " ";
        }
        std::cout << '\n';
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }

    try {
        tree.cut(2, 1);
        auto path = tree.find_path(3, 1);
        if (path->empty()) {
            std::cout << "Edge successfully cut; no path exists.\n";
        } else {
            std::cerr << "Cut operation failed.\n";
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }
}

int main() {
    test_BasicTree();
    std::cout << "Test completed successfully.\n";
    return 0;
}