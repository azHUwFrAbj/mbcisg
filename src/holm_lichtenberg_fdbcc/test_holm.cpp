

#include <iostream>
#include <sstream>
#include "test_utils.h"


void power_on_self_test() {

    test_basic_insertion();
    test_simple_insertion();

    test_basic_deletion();
    test_simple_deletion();
}

int main() {
    power_on_self_test();

    std::string filename1 = "../../holm_lichtenberg_fdbcc/graphs/RO_edges.csv";
    std::string filename2 = "../../holm_lichtenberg_fdbcc/graphs/DD-Miner_miner-disease-disease.csv";
    std::string filename3 = "../../holm_lichtenberg_fdbcc/graphs/ca-GrQc.csv";
    std::string filename4 = "../../holm_lichtenberg_fdbcc/graphs/lastfm_asia_edges.csv";

    insertion_efficiency(filename1);
    deletion_efficiency(filename1, 2000);

    std::cout << "Hello World!\n";
}