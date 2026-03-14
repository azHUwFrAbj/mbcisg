

#include <iostream>
#include <cassert>
#include "NotYetMergeSet.h"

void test_NotYetMergeSet() {
    std::cout << "Running tests for NotYetMergeSet...\n";

    NotYetMergeSet mergeSet(3);

    assert(mergeSet.empty());
    std::cout << "Test passed: empty() returns true on initialization.\n";

    VId x = 1, y = 2, z = 3;
    int l = 1;
    VSetP vs = make_VSet();
    vs->insert(4);
    vs->insert(5);
    mergeSet.setNYNbrs(x, y, z, l, vs);

    auto retrievedSet = mergeSet.getNYNbrs(x, y, z, l);
    assert(retrievedSet->find(4) != retrievedSet->end());
    assert(retrievedSet->find(5) != retrievedSet->end());
    std::cout << "Test passed: setNYNbrs and getNYNbrs work as expected.\n";

    assert(!mergeSet.empty());
    std::cout << "Test passed: empty() returns false after adding elements.\n";

    VId x2 = 6, y2 = 7, z2 = 8;
    VSetP vs2 = make_VSet();
    vs2->insert(9);
    mergeSet.setNYNbrs(x2, y2, z2, l, vs2);

    auto retrievedSet2 = mergeSet.getNYNbrs(x2, y2, z2, l);
    assert(retrievedSet2->find(9) != retrievedSet2->end());
    std::cout << "Test passed: setNYNbrs and getNYNbrs handle multiple neighbor pairs.\n";

    std::cout << "All tests passed!\n";
}

int main() {
    test_NotYetMergeSet();
    return 0;
}