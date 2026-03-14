

#include "test_utils.h"
#include <iostream>
#include <cassert>
#include "SimHolmBcc.h"
#include <fstream>
#include <vector>
#include <chrono>

#include "HopcroftTarjan.h"

void test_basic_insert_and_swap() {
    std::cout << "Running tests for SimHolmBcc...\n";

    SimHolmBcc sim(6);

    assert(!sim.biconnected(1, 2));
    std::cout << "Test passed: biconnected returns false for disconnected vertices.\n";

    sim.insert_edge(1, 2);
    sim.insert_edge(2, 3);
    sim.insert_edge(3, 4);
    sim.insert_edge(4, 1);

    assert(sim.biconnected(1, 3));
    assert(sim.biconnected(2, 4));
    std::cout << "Test passed: biconnected returns true for vertices in the same cycle.\n";

#ifdef BCC_DEBUG
    assert(sim.edgeInfo.is_rep_edge(1, 2) && "replacement edge not found");
    assert(sim.edgeInfo.get_rep_edge(1, 2) == make_edge(1, 4) && "replacement edge not matched");
    std::cout << "Test passed: found replacement edges.\n";

    sim.swap(2, 3);
    assert(sim.edgeInfo.is_rep_edge(1, 2) && "replacement edge not found");
    assert(sim.edgeInfo.get_rep_edge(1, 2) == make_edge(2, 3) && "replacement edge not matched");
    assert(sim.edgeInfo.get_rep_edge(4, 1) == make_edge(2, 3) && "replacement edge not matched");
#endif


    sim.insert_edge(5, 6);
    assert(!sim.biconnected(1, 5));
    assert(!sim.biconnected(3, 6));
    std::cout << "Test passed: biconnected returns false for vertices in different components.\n";

    sim.insert_edge(4, 5);
    assert(!sim.biconnected(1, 6));
    std::cout << "Test passed: biconnected returns false after connecting components.\n";

    sim.insert_edge(1, 6);
    assert(sim.biconnected(1, 6));
    std::cout << "Test passed: Edge insertion updates biconnectivity correctly.\n";

    std::cout << "All tests passed!\n";
}

void test_recover_star() {
#ifdef BCC_DEBUG

    SimHolmBcc sim(11);

    sim.insert_edge(1, 2);
    sim.insert_edge(2, 3);
    sim.insert_edge(3, 4);
    sim.insert_edge(4, 5);
    sim.insert_edge(5, 6);

    sim.insert_edge(2, 7);
    sim.insert_edge(3, 8);
    sim.insert_edge(7, 8);

    sim.insert_edge(2, 9);
    sim.insert_edge(9, 10);
    sim.insert_edge(4, 11);
    sim.insert_edge(10, 11);
    sim.insert_edge(11, 6);
    sim.insert_edge(1, 9);

    sim.insert_edge(1, 6);

    assert(sim.find_non_tree_edges_at_level(7, 2, 0).front() == make_edge(7, 8) && "appropriate tree edge not found");
    auto e = sim.find_non_tree_edges_at_level(8, 3, 0).front();
    assert(make_edge(e) == make_edge(7, 8) && "appropriate tree edge not found");
    assert(sim.find_non_tree_edges_at_level(9, 2, 0).size() == 2 && "not find enough edges");
    e = sim.find_non_tree_edges_at_level(9, 2, 0).front();
    assert((make_edge(e) == make_edge(11, 10)) || make_edge(e) == make_edge(1, 9) && "appropriate tree edge not found");
    e = sim.find_non_tree_edges_at_level(9, 2, 0).back();
    assert((make_edge(e) == make_edge(11, 10)) || make_edge(e) == make_edge(1, 9) && "appropriate tree edge not found");
    e = sim.find_non_tree_edges_at_level(11, 4, 0).front();
    assert(make_edge(e) == make_edge(11, 10) || make_edge(e) == make_edge(11, 6) && "appropriate tree edge not found");

    assert(sim.edgeInfo.get_rep_edge(1, 2) == make_edge(1, 6) && "replacement edge not matched");
    assert(sim.edgeInfo.get_rep_edge(3, 2) == make_edge(1, 6) && "replacement edge not matched");

    VId v = 1;
    VId w = 6;

    assert(sim.edgeInfo.get_level(7, 8) == 0 && "level does not initialize well");

    sim.recover_star(2, 1, v, w, 0, true);
    assert((sim.edgeInfo.get_rep_edge(3, 2) == make_edge(7, 8) ||
            sim.edgeInfo.get_rep_edge(3, 2) == make_edge(10, 11)) && "replacement edge not matched");
    assert((sim.edgeInfo.get_rep_edge(3, 4) == make_edge(10, 11)) && "replacement edge not matched");
    assert(sim.edgeInfo.get_cover_level(3, 2) == 1 && "level does not increase");

    sim.recover_star(4, 3, v, w, 0, true);
    assert((sim.edgeInfo.get_rep_edge(4, 5) == make_edge(11, 6)) && "replacement edge not matched");
    assert((sim.edgeInfo.get_rep_edge(6, 5) == make_edge(11, 6)) && "replacement edge not matched");

    SimHolmBcc sim2(11);

    sim2.insert_edge(1, 2);
    sim2.insert_edge(2, 3);
    sim2.insert_edge(3, 4);
    sim2.insert_edge(4, 5);
    sim2.insert_edge(5, 6);

    sim2.insert_edge(2, 7);
    sim2.insert_edge(3, 8);
    sim2.insert_edge(7, 8);

    sim2.insert_edge(2, 9);
    sim2.insert_edge(9, 10);
    sim2.insert_edge(4, 11);
    sim2.insert_edge(10, 11);
    sim2.insert_edge(11, 6);
    sim2.insert_edge(1, 9);

    sim2.insert_edge(1, 6);

    sim2.nbrPartition.remove_from_neighbours(1, 6, 0);

    sim2.recover_phrase2(1, 6, 0);

    assert((sim2.edgeInfo.get_rep_edge(3, 2) == make_edge(7, 8) ||
            sim2.edgeInfo.get_rep_edge(3, 2) == make_edge(10, 11)) && "replacement edge not matched");
    assert((sim2.edgeInfo.get_rep_edge(3, 4) == make_edge(10, 11)) && "replacement edge not matched");
    assert(sim2.edgeInfo.get_cover_level(3, 2) == 0 && "level increase");
    assert(sim2.edgeInfo.get_level(1, 9) == 0 && "level does not increase");
    assert((sim2.edgeInfo.get_rep_edge(4, 5) == make_edge(11, 6)) && "replacement edge not matched");
    assert((sim2.edgeInfo.get_rep_edge(6, 5) == make_edge(11, 6)) && "replacement edge not matched");
    assert(sim2.edgeInfo.get_level(9, 1) == 0 && "level should not increase");

    SimHolmBcc sim3(11);

    sim3.insert_edge(1, 2);
    sim3.insert_edge(2, 3);
    sim3.insert_edge(3, 4);
    sim3.insert_edge(4, 5);
    sim3.insert_edge(5, 6);

    sim3.insert_edge(2, 7);
    sim3.insert_edge(3, 8);
    sim3.insert_edge(7, 8);

    sim3.insert_edge(3, 6);

    sim3.insert_edge(2, 9);
    sim3.insert_edge(2, 11);
    sim3.insert_edge(9, 10);
    sim3.insert_edge(4, 11);
    sim3.insert_edge(10, 11);
    sim3.insert_edge(11, 6);

    sim3.insert_edge(1, 9);

    sim3.insert_edge(1, 6);
    sim3.nbrPartition.remove_from_neighbours(1, 6, 0);

    std::cout << "testing the entering of phrase 2 triggered by recover star" << std::endl;

    bool res = sim3.recover_star(1, 2, 1, 6, 0, false);
    assert(res == false && "Mistakenly enter phrase 2");

    res = sim3.recover_star(2, 3, 1, 6, 0, false);
    assert(res && "should enter phrase 2");

    std::cout << "Testing recover star case D" << std::endl;

    SimHolmBcc sim4(11);

    sim4.insert_edge(1, 2);
    sim4.insert_edge(2, 3);
    sim4.insert_edge(3, 4);
    sim4.insert_edge(4, 5);
    sim4.insert_edge(5, 6);

    sim4.insert_edge(2, 9);
    sim4.insert_edge(2, 11);
    sim4.insert_edge(9, 10);
    sim4.insert_edge(4, 11);
    sim4.insert_edge(10, 11);
    sim4.insert_edge(1, 10);

    sim4.insert_edge(1, 6);
    sim4.inc_level_of_nt_edge(1, 6, 0);

    std::cout << "All edges added." << std::endl;


    sim4.notYetMergeSet.printNotYetMergeSet();
    sim4.uncover(1, 6, 0);
    sim4.notYetMergeSet.printNotYetMergeSet();
    sim4.recover_star(4, 5, 1, 6, 0, false);
    sim4.notYetMergeSet.printNotYetMergeSet();
    sim4.recover_star(2, 3, 1, 6, 0, false);
    sim4.recover_star(2, 1, 1, 6, 0, false);

    assert(sim4.biconnected(1, 3) && "not recovered");


    std::cout << "All recover stor tests passed!\n";
#endif
}


void test_recover() {
#ifdef BCC_DEBUG
    std::cout << "Testing recover using 3 simple graphs" << std::endl;

    SimHolmBcc sim1(11);

    sim1.insert_edge(1, 2);
    sim1.insert_edge(2, 3);
    sim1.insert_edge(3, 4);
    sim1.insert_edge(4, 5);
    sim1.insert_edge(5, 6);

    sim1.insert_edge(2, 7);
    sim1.insert_edge(3, 8);
    sim1.insert_edge(7, 8);

    sim1.insert_edge(3, 6);

    sim1.insert_edge(2, 9);
    sim1.insert_edge(2, 11);
    sim1.insert_edge(9, 10);
    sim1.insert_edge(4, 11);
    sim1.insert_edge(10, 11);
    sim1.insert_edge(11, 6);

    sim1.insert_edge(1, 6);

    sim1.insert_edge(2, 11);

    assert(sim1.biconnected(1, 3) && "not biconnected");
    assert(sim1.biconnected(2, 6) && "not biconnected");
    assert(sim1.biconnected(8, 9) && "not biconnected");
    assert(sim1.biconnected(3, 6) && "not biconnected");

    sim1.uncover(1, 6, 0);
    sim1.dec_level_of_nt_edge(1, 6, 0);

    assert(!sim1.biconnected(1, 3) && "still biconnected");
    assert(!sim1.biconnected(2, 6) && "still biconnected");
    assert(!sim1.biconnected(8, 9) && "still biconnected");
    assert(!sim1.biconnected(4, 6) && "still biconnected");

    sim1.recover(1, 6, 0, false);
    assert(!sim1.biconnected(1, 3) && "still biconnected");
    assert(sim1.biconnected(2, 6) && "not biconnected");
    assert(sim1.biconnected(8, 9) && "not biconnected");
    assert(sim1.biconnected(4, 6) && "not biconnected");

    std::cout << "The recover test on the first graph passed!\n";

    SimHolmBcc sim2(8);
    sim2.insert_edge(1, 2);
    sim2.insert_edge(2, 3);
    sim2.insert_edge(3, 4);
    sim2.insert_edge(4, 5);
    sim2.insert_edge(5, 6);
    sim2.insert_edge(6, 7);
    sim2.insert_edge(7, 8);

    sim2.insert_edge(1, 3);
    sim2.insert_edge(2, 4);
    sim2.insert_edge(3, 5);
    sim2.insert_edge(4, 6);
    sim2.insert_edge(5, 7);
    sim2.insert_edge(6, 8);

    sim2.insert_edge(1, 8);

    assert(sim2.biconnected(1, 6) && "not biconnected");
    assert(sim2.biconnected(3, 6) && "not biconnected");
    assert(sim2.biconnected(4, 8) && "not biconnected");
    assert(sim2.biconnected(2, 7) && "not biconnected");

    sim2.uncover(1, 8, 0);
    sim2.dec_level_of_nt_edge(1, 8, 0);

    assert(!sim2.biconnected(1, 6) && "still biconnected");
    assert(!sim2.biconnected(3, 6) && "still biconnected");
    assert(!sim2.biconnected(4, 8) && "still biconnected");
    assert(!sim2.biconnected(2, 7) && "still biconnected");

    sim2.notYetMergeSet.printNotYetMergeSet();

    sim2.recover(1, 8, 0, false);

    sim2.notYetMergeSet.printNotYetMergeSet();

    assert(sim2.biconnected(1, 6) && "not biconnected");
    assert(sim2.biconnected(3, 6) && "not biconnected");
    assert(sim2.biconnected(4, 8) && "not biconnected");
    assert(sim2.biconnected(2, 7) && "not biconnected");

    std::cout << "The recover test on the second graph passed!\n";

    SimHolmBcc sim3(8);
    sim3.insert_edge(1, 2);
    sim3.insert_edge(2, 3);
    sim3.insert_edge(3, 4);
    sim3.insert_edge(4, 5);
    sim3.insert_edge(5, 6);
    sim3.insert_edge(6, 7);
    sim3.insert_edge(7, 8);

    sim3.insert_edge(1, 3);
    sim3.insert_edge(2, 4);
    sim3.insert_edge(1, 4);
    sim3.insert_edge(1, 5);

    sim3.insert_edge(4, 6);
    sim3.insert_edge(5, 7);
    sim3.insert_edge(6, 8);

    sim3.insert_edge(1, 8);

    assert(sim3.biconnected(1, 7) && "not biconnected");
    assert(sim3.biconnected(2, 6) && "not biconnected");
    assert(sim3.biconnected(3, 8) && "not biconnected");
    assert(sim3.biconnected(4, 7) && "not biconnected");

    sim3.uncover(1, 8, 0);
    sim3.dec_level_of_nt_edge(1, 8, 0);

    assert(!sim3.biconnected(1, 7) && "still biconnected");
    assert(!sim3.biconnected(2, 6) && "still biconnected");
    assert(!sim3.biconnected(3, 8) && "still biconnected");
    assert(!sim3.biconnected(4, 7) && "still biconnected");

    sim3.recover(1, 8, 0, false);

    assert(sim3.biconnected(1, 7) && "not biconnected");
    assert(sim3.biconnected(2, 6) && "not biconnected");
    assert(sim3.biconnected(3, 8) && "not biconnected");
    assert(sim3.biconnected(4, 7) && "not biconnected");

    std::cout << "The recover test on the 3rd graph passed!\n";


    std::cout << "All recover tests passed!\n";

#endif
}

void test_basic_deletion() {
    test_recover_star();
    test_recover();

    std::vector<std::pair<int, int> > edges1 = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {2, 7}, {3, 8}, {7, 8}, {2, 9}, {9, 10},
        {4, 11}, {10, 11}, {11, 6}, {1, 9}, {1, 6}
    };

    std::vector<std::pair<int, int> > edges2 = {

        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {2, 7}, {3, 8}, {7, 8}, {3, 6}, {2, 9},
        {2, 11}, {9, 10}, {4, 11}, {10, 11}, {11, 6},
        {1, 6}
    };

    std::vector<std::pair<int, int> > edges3 = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {6, 7}, {7, 8}, {1, 3}, {2, 4}, {3, 5},
        {4, 6}, {5, 7}, {6, 8}, {1, 8}
    };

    std::vector<std::pair<int, int> > edges4 = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6},
        {6, 7}, {7, 8}, {1, 3}, {2, 4}, {1, 4},
        {1, 5}, {4, 6}, {5, 7}, {6, 8}, {1, 8}
    };

    std::vector<std::pair<int, int> > edges5 = {
        {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 15}, {2, 3}, {2, 4}, {2, 5},
        {2, 6}, {2, 7}, {2, 9}, {2, 11}, {2, 18}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {3, 21}, {4, 5}, {4, 6},
        {4, 7}, {4, 8}, {4, 11}, {4, 24}, {5, 6}, {5, 7}, {5, 27}, {6, 7}, {6, 25}, {7, 8}, {8, 13}, {9, 10}, {9, 16},
        {9, 17}, {9, 18}, {10, 11}, {10, 19}, {10, 20}, {10, 21}, {11, 22}, {11, 23}, {11, 24}, {12, 25}, {12, 26},
        {12, 27}, {13, 14}, {13, 15}, {13, 21}, {14, 15}, {14, 22}, {15, 23}, {16, 17}, {16, 18}, {16, 24}, {17, 25},
        {18, 26}, {19, 20}, {19, 21}, {19, 27}, {1, 20}, {2, 21}, {3, 22}, {4, 23}, {5, 24}, {7, 26}, {8, 27},
    };


    std::vector<std::vector<std::pair<int, int> > > egset;
    egset.push_back(edges1);
    egset.push_back(edges2);
    egset.push_back(edges3);
    egset.push_back(edges4);

    int i = 1;
    for (auto egs: egset) {
        std::cout << "Testing edge set #" << i++ << ":\n";
        auto count = holm::count_vertices(egs);
        SimHolmBcc sim(count);
        for (auto e: egs) {
            sim.insert_edge(e.first, e.second);
        }

        while (!egs.empty()) {
            compare_with_ground_truth(sim, egs);

            auto eg_to_delete = egs.back();
            egs.pop_back();
            sim.delete_edge(eg_to_delete.first, eg_to_delete.second);
        }
    }

    std::cout << "All basic deletion tests passed!\n";
}

void test_simple_deletion() {
    std::cout << "Testing simple deletion on tiny real world graph...\n";
    std::string filename = "../../holm_lichtenberg_fdbcc/graphs/DD-Miner_miner-disease-disease.csv";
    auto egs = read_edges_from_file(filename);
    auto count = holm::count_vertices(egs);
    SimHolmBcc sim(count);
    for (auto e: egs) {
        sim.insert_edge(e.first, e.second);
    }

    int steps = egs.size() / 5 + 8;
    for (int i = 0; i < egs.size(); i += steps) {
        std::cout << "Testing edges set #" << i << ":\n";
        for (int j = i; j < i + steps; j++) {
            if (egs.size() < steps / 2) {
                break;
            }
            auto eg_to_delete = egs.back();
            egs.pop_back();
            sim.delete_edge(eg_to_delete.first, eg_to_delete.second);
        }
        compare_with_ground_truth(sim, egs);
    }
    std::cout << "All simple deletion tests passed!\n";
}

void deletion_efficiency(const std::string &filename, int deletion_number) {
    auto edges = read_edges_from_file(filename);

    if (deletion_number <= 0) {
        deletion_number = edges.size();
    }

    int vertex_count = holm::count_vertices(edges);

    SimHolmBcc sim(vertex_count);

    for (const auto &edge: edges) {
        sim.insert_edge(edge.first, edge.second);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < deletion_number; i++) {
        std::cout << "Deleting the " << i << "th edge: " << str(edges[i]) <<
                " of the overall " << edges.size() << " edges" << std::endl;
        auto edge = edges[i];
        sim.delete_edge(edge.first, edge.second);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken to delete " << deletion_number << " edges: "
            << elapsed.count() << " seconds.\n";
    double average_ms = elapsed.count() / double(deletion_number) * 1000;
    std::cout << "Average " << deletion_number << " edges: "
            << average_ms << " milliseconds.\n";
    double average_ms_exclude_brute_force_search =
            (elapsed.count() * 1000 - sim.brute_force_search_time_ms) / double(deletion_number);
    std::cout << "Excluding brute-force search time, Average " << deletion_number << " edges: "
            << average_ms_exclude_brute_force_search << " milliseconds.\n";

    std::cout << "Testing the reconstruction efficiency" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    HopcroftTarjan ht(vertex_count);
    for (auto eg: edges) {
        ht.addEdge(eg.first, eg.second);
    }
    ht.tarjanBCC();
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "While reconstruction takes " << elapsed.count() * 1000 << " milliseconds." << std::endl;
}

void insertion_efficiency(const std::string &filename) {
    auto edges = read_edges_from_file(filename);

    int vertex_count = holm::count_vertices(edges);

    SimHolmBcc sim(vertex_count);

    auto start = std::chrono::high_resolution_clock::now();
    for (const auto &edge: edges) {
        sim.insert_edge(edge.first, edge.second);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken to insert " << edges.size() << " edges: "
            << elapsed.count() << " seconds.\n";
    std::cout << "Average " << edges.size() << " edges: "
            << elapsed.count() / double(edges.size()) * 1000 << " milliseconds.\n";
}

void compare_with_ground_truth(SimHolmBcc &sim, const std::vector<Edge> &current_edge) {
    std::set<Edge> egs;
    egs.insert(current_edge.begin(), current_edge.end());
    compare_with_ground_truth(sim, egs);
}


void compare_with_ground_truth(SimHolmBcc &sim, const std::set<Edge> &current_edge) {
    int vertex_count = sim.vertex_count;

    HopcroftTarjan ht(vertex_count);
    for (auto eg: current_edge) {
        ht.addEdge(eg.first, eg.second);
    }
    ht.tarjanBCC();
    auto expected_bccs = ht.biconnectedComponents;

    std::vector<VSetP> detected_bccs;
    sim.get_bccs(detected_bccs);

    assert(detected_bccs.size() == expected_bccs.size() && "Unmatched number of biconnected components");

    std::map<int, std::vector<VSetP> > bcc_bucket;
    for (const auto &bcc: detected_bccs) {
        int size = (int) bcc->size();
        bcc_bucket[size].push_back(bcc);
    }
    for (const auto &expected_bcc: expected_bccs) {
        int sz = (int) expected_bcc.size();
        const auto &bccs = bcc_bucket[sz];
        assert(holm::contains_bcc(bccs, expected_bcc) && "Expected biconnected component not found");
    }
}

void test_getBCCs() {
    std::cout << "Running test for getBCCs...\n";

    SimHolmBcc sim(10);

    sim.insert_edge(1, 2);
    sim.insert_edge(2, 3);
    sim.insert_edge(3, 4);
    sim.insert_edge(4, 5);
    sim.insert_edge(5, 1);

    sim.insert_edge(6, 7);
    sim.insert_edge(7, 8);
    sim.insert_edge(8, 6);

    sim.insert_edge(2, 6);

    std::vector<VSetP> bccs;
    sim.get_bccs(bccs);
    print_bccs(bccs);

    std::vector<std::set<VId> > expected_bccs = {
        {1, 2, 3, 4, 5},
        {6, 7, 8},
        {2, 6}
    };

    for (const auto &expected_bcc: expected_bccs) {
        assert(holm::contains_bcc(bccs, expected_bcc) && "Expected biconnected component not found");
    }

    sim.insert_edge(3, 6);
    sim.get_bccs(bccs);
    print_bccs(bccs);

    std::vector<std::set<VId> > expected_bccs2 = {
        {1, 2, 3, 4, 5, 6},
        {6, 7, 8},
    };

    for (const auto &expected_bcc: expected_bccs2) {
        assert(holm::contains_bcc(bccs, expected_bcc) && "Expected biconnected component not found");
    }

    sim.insert_edge(4, 7);
    sim.get_bccs(bccs);
    print_bccs(bccs);

    std::vector<std::set<VId> > expected_bccs3 = {
        {1, 2, 3, 4, 5, 6, 7, 8}
    };

    for (const auto &expected_bcc: expected_bccs3) {
        assert(holm::contains_bcc(bccs, expected_bcc) && "Expected biconnected component not found");
    }

    std::cout << "Test passed: All biconnected components are correct.\n";
}

void test_basic_insertion() {
    test_basic_insert_and_swap();
    test_getBCCs();


    std::cout << "Running test for biconnectivity with specific vertex pairs...\n";

    std::vector<std::pair<int, int> > edges1 = {
        {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7},
        {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7},
        {3, 4}, {3, 5}, {3, 6}, {3, 7},
        {4, 5}, {4, 6}, {4, 7},
        {5, 6}, {5, 7},
        {6, 7},
        {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5},
        {9, 1}, {9, 2}, {9, 3}, {9, 4}, {9, 5},
        {10, 1}, {10, 2}, {10, 3}, {10, 4}, {10, 5},
        {11, 1}, {11, 2}, {11, 3}, {11, 4}, {11, 5},
        {12, 1}, {12, 2}, {12, 3}, {12, 4}, {12, 5},
        {13, 8}, {13, 14}, {13, 15},
        {14, 8}, {14, 15},
        {15, 8}
    };

    std::vector<std::pair<int, int> > edges2 = {
        {16, 9}, {16, 17}, {16, 18},
        {17, 9}, {17, 18},
        {18, 9},
        {19, 10}, {19, 20}, {19, 21},
        {20, 10}, {20, 21},
        {21, 10},
        {22, 11}, {22, 23}, {22, 24},
        {23, 11}, {23, 24},
        {24, 11},
        {25, 12}, {25, 26}, {25, 27},
        {26, 12}, {26, 27},
        {27, 12},
        {1, 10}, {2, 11}, {4, 8},
        {1, 15}, {2, 18}, {3, 21}, {4, 24}, {5, 27},
        {8, 13}, {9, 16}, {10, 19}, {11, 22}, {12, 25},
        {13, 21}, {14, 22}, {15, 23}, {16, 24}, {17, 25},
        {18, 26}, {19, 27}, {20, 1}, {21, 2}, {22, 3},
        {23, 4}, {24, 5}, {25, 6}, {26, 7}, {27, 8}
    };


    int max_vertex_id = 27;
    SimHolmBcc sim(max_vertex_id);

    for (const auto &edge: edges1) {
        sim.insert_edge(edge.first, edge.second);
    }

    std::vector<std::pair<int, int> > biconnected_pairs = {
        {1, 3}, {2, 6}, {3, 7}, {8, 13}, {13, 15}, {14, 15}
    };

    std::vector<std::pair<int, int> > not_biconnected_pairs = {
        {3, 15}, {2, 14}, {9, 13}, {7, 14}
    };

    for (const auto &pair: biconnected_pairs) {
        assert(sim.biconnected(pair.first, pair.second) &&
               "Biconnectivity failed for a biconnected pair.");
    }

    for (const auto &pair: not_biconnected_pairs) {
        assert(!sim.biconnected(pair.first, pair.second) &&
               "Biconnectivity incorrectly passed for a non-biconnected pair.");
    }

    std::vector<VSetP> bccs;
    sim.get_bccs(bccs);
    assert(bccs.size() == 2 && "Fail to get 2 big BCCs.");


    for (const auto &edge: edges2) {
        sim.insert_edge(edge.first, edge.second);
    }
    for (const auto &pair: not_biconnected_pairs) {
        assert(sim.biconnected(pair.first, pair.second) &&
               "Biconnectivity incorrectly passed for a now-biconnected pair.");
    }

    sim.get_bccs(bccs);
    assert(bccs.size() == 1 && "Fail to get one big BCC.");

    std::cout << "Test passed: Biconnectivity correctness verified for specific vertex pairs.\n";

    std::set<Edge> edges;
    edges.insert(edges1.begin(), edges1.end());
    edges.insert(edges2.begin(), edges2.end());
    compare_with_ground_truth(sim, edges);
}


void test_simple_insertion() {
    std::cout << "Testing simple insertion on tiny real world graph...\n";

    std::string filename = "../../holm_lichtenberg_fdbcc/graphs/lastfm_asia_edges.csv";
    auto edges = read_edges_from_file(filename);
    int vertex_count = holm::count_vertices(edges);
    SimHolmBcc sim(vertex_count);
    int steps = edges.size() / 4;
    std::vector<Edge> inserted_edges;
    for (int i = 0; i < edges.size(); i += steps) {
        for (int j = i; j < i + steps; j++) {
            auto edge = edges[i];
            sim.insert_edge(edge.first, edge.second);
            inserted_edges.push_back(edge);
        }
        compare_with_ground_truth(sim, inserted_edges);
    }
    std::cout << "All simple insertions on tiny real world graph...\n";
}