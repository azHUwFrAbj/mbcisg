

#include "../test_utils.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <iomanip>
#include "AUF.h"
#include "BBF.h"
#include "BBFQ.h"
#include "BcTreeQ.h"
#include "BlockCutTree.h"


std::string bccToString(const BCC &bcc) {
    std::string s = "{";
    for (auto it = bcc.begin(); it != bcc.end(); ++it) {
        s += std::to_string(*it);
        if (std::next(it) != bcc.end()) s += ",";
    }
    s += "}";
    return s;
}

std::string bccsToString(const BCCs &bccs) {
    std::stringstream ss;
    for (const auto &bcc: bccs) {
        ss << bccToString(*bcc) << '\n';
    }
    return ss.str();
}

bool bccEqual(const std::shared_ptr<BCC> &a, const std::shared_ptr<BCC> &b) {
    return *a == *b;
}

bool contains(const BCCs &list, const std::shared_ptr<BCC> &target) {
    return std::any_of(list.begin(), list.end(), [&](const std::shared_ptr<BCC> &bcc) {
        return *bcc == *target;
    });
}

void printBCCDifferences(const BCCs &oldBCCs, const BCCs &newBCCs) {
    std::cout << "\nSome difference in these 2 BCC set:\n";
    std::cout << "Added BCCs:\n";
    for (const auto &bcc: newBCCs) {
        if (!contains(oldBCCs, bcc)) {
            std::cout << "  " << bccToString(*bcc) << "\n";
        }
    }

    std::cout << "Removed BCCs:\n";
    for (const auto &bcc: oldBCCs) {
        if (!contains(newBCCs, bcc)) {
            std::cout << "  " << bccToString(*bcc) << "\n";
        }
    }
}

bool compare_bccs(const BCCs &expected_bccs,
                  const BCCs &detected_bccs);

bool contains_bcc(const BCCs &bccs, const BCC &expected_bcc) {
    for (const auto &bcc: bccs) {
        std::set<VId> current_bcc(bcc->begin(), bcc->end());
        if (current_bcc == expected_bcc) {
            return true;
        }
    }
    return false;
}

void get_ground_truth_bccs(int vertex_count, const std::vector<Edge> &current_edge, BCCs &bccs) {
    HopcroftTarjan ht(vertex_count);
    for (auto eg: current_edge) {
        ht.add_edge(eg.first, eg.second);
    }
    ht.tarjan_bcc();
    bccs = ht.biconnectedComponents;
}

bool compare_bccs(const std::vector<std::shared_ptr<std::set<int> > > &expected_bccs,
                  const std::vector<std::shared_ptr<std::set<int> > > &detected_bccs) {
    if (detected_bccs.size() != expected_bccs.size()) {
        printBCCDifferences(expected_bccs, detected_bccs);
        std::cout << "#Detected BCCs: " << detected_bccs.size() << '\n';
        std::cout << "#BCCs: " << expected_bccs.size() << '\n';
        assert(detected_bccs.size() == expected_bccs.size() && "Unmatched number of biconnected components");
    }

    std::map<int, std::vector<std::shared_ptr<std::set<int> > > > bcc_bucket;
    for (const auto &bcc: detected_bccs) {
        int size = (int) bcc->size();
        bcc_bucket[size].push_back(bcc);
        return false;
    }
    for (const auto &expected_bcc: expected_bccs) {
        int sz = (int) expected_bcc->size();
        const auto &bccs = bcc_bucket[sz];
        bool in_detected_bcc = contains_bcc(bccs, *expected_bcc);
        if (!in_detected_bcc) {
            assert(in_detected_bcc && "Expected biconnected component not found");
            return false;
        }
    }
    return true;
}


void incremental_insertion_test_on(std::string csv_data_set) {
    std::vector<Edge> edges = read_edges_from_csv(csv_data_set);
    const int vertex_count = get_max_vid(edges);
    std::cout << "Testing dataset: " << csv_data_set << " with vertex count: "
            << vertex_count << " and edges count: " << edges.size() << std::endl;

    std::vector<Edge> current_edges;
    const int BATCH_SIZE = 10;
    const int step = edges.size() < 200 ? 1 : static_cast<int>(edges.size()) / BATCH_SIZE + 1;

    BBFQ bbf_q(vertex_count, edges.size(), edges.size());


    for (int i = 0; i < edges.size(); i += step) {
        std::cout << "Testing step at #" << i / step << ':';

        std::vector<Edge> new_edges;
        for (int j = 0; j < step; ++j) {
            if (i + j > edges.size() - 1) {
                break;
            }
            auto eg = edges.at(i + j);
            current_edges.push_back(eg);
            new_edges.push_back(eg);
            }
        bbf_q.insert_batch(new_edges, i);

        BCCs detected_bccs;
        BCCs ground_truth_bccs;
        get_ground_truth_bccs(vertex_count, current_edges, ground_truth_bccs);

        bbf_q.get_bccs(detected_bccs);
        compare_bccs(ground_truth_bccs, detected_bccs);
        std::cout << "\tBFF Basic: Passed!... ";
        std::cout << "Step done." << std::endl;
    }
}

void incremental_insertion_tests() {
    std::cout << "Incremental insertion tests" << std::endl;

    incremental_insertion_test_on(TOY_GRAPH_CSV);
    incremental_insertion_test_on(LASTFM_CSV);

    std::cout << "End of incremental insertion tests" << std::endl;
}

void decremental_deletion_test_on(const std::string &csv_data_set) {
    const std::vector<Edge> edges = read_edges_from_csv(csv_data_set);
    const int vertex_count = get_max_vid(edges);
    std::cout << "Testing dataset: " << csv_data_set << " with vertex count: "
            << vertex_count << " and edges count: " << edges.size() << std::endl;

    std::vector<Edge> remaining_edges(edges.size());
    for (int i = 0; i < edges.size(); ++i) {
        remaining_edges[i] = edges.at(edges.size() - i - 1);
    }


    const int BATCH_SIZE = 501;
    const int step = edges.size() < 200 ? 1 : static_cast<int>(edges.size()) / BATCH_SIZE + 1;


    BcTreeQ bc_tree_q(vertex_count);

    BBFQ bbf_q(vertex_count, edges.size(), edges.size());


    for (int i = 0; i < edges.size(); i++) {
        auto eg = edges.at(i);
        bc_tree_q.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
    }
    bc_tree_q.build_bc_tree();

    bbf_q.insert_batch_bottom_up(edges, 0);

    int started_time = 0;
    int deletion_count = 0;
    std::vector<Edge> to_delete;

    while (!remaining_edges.empty()) {
        std::cout << "Remaining edges: "
                << remaining_edges.size() << " out of " << edges.size()
                << " (" << std::fixed << std::setprecision(2)
                << (remaining_edges.size() * 100.0 / edges.size()) << "%)"
                << std::endl;
        auto eg = remaining_edges.back();
        remaining_edges.pop_back();

        to_delete.push_back(eg);
        started_time++;

        deletion_count++;
        if (deletion_count >= step) {
            deletion_count = 0;

            int expiration_started_time = started_time - step;

            bc_tree_q.expire_edges(to_delete, expiration_started_time);

            bbf_q.expire(started_time - 1);

            to_delete.clear();

            BCCs detected_bccs;
            BCCs ground_truth_bccs;
            get_ground_truth_bccs(vertex_count, remaining_edges, ground_truth_bccs);

            bc_tree_q.get_bccs(detected_bccs);
            compare_bccs(ground_truth_bccs, detected_bccs);
            std::cout << "\tBlock-Cut Tree: Passed!... ";

            bbf_q.get_bccs(detected_bccs);
            compare_bccs(ground_truth_bccs, detected_bccs);
            std::cout << "\tBBF Basic: Passed!... ";


            std::cout << "Step done." << std::endl;
        }
    }
}

void decremental_deletion_tests() {
    std::cout << "Decremental deletion tests start..." << std::endl;

    decremental_deletion_test_on(TOY_GRAPH_CSV);

    std::cout << "End of decremental deletion tests" << std::endl;
}

void sliding_window_test_on(const std::string &csv_data_set) {

    const std::vector<Edge> edges = read_edges_from_csv(csv_data_set);
    const int edge_count = edges.size();
    const int vertex_count = get_max_vid(edges);
    std::cout << "Testing dataset: " << csv_data_set << " with vertex count: "
            << vertex_count << " and edges count: " << edges.size() << std::endl;

    std::deque<Edge> edges_queue;
    for (auto eg: edges) {
        edges_queue.push_back(eg);
    }

    const int PERSENTAGE = 42;
    const int SLIDING_WINDOW_SIZE = edges.size() / 100 * PERSENTAGE;
    const int STRIDE_COUNT = 142;

    const int STRIDE_SIZE = edges.size() < 200 ? 1 : static_cast<int>(edges.size()) / STRIDE_COUNT + 1;


    BBFQ bbf_q(vertex_count, SLIDING_WINDOW_SIZE, edge_count);
    std::deque<Edge> existing_edges;


    std::vector<Edge> first_edge_set;
    for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
        auto eg = edges_queue.front();
        edges_queue.pop_front();

        existing_edges.push_back(eg);
        first_edge_set.push_back(eg);
    }
    bbf_q.insert_batch(first_edge_set, 0);


    std::vector<Edge> expiring_edges;
    int expired_timestamp = 0;

    for (int start_t = SLIDING_WINDOW_SIZE; start_t + STRIDE_SIZE < edges.size(); start_t += STRIDE_SIZE) {
        std::cout << "Remaining edges: "
                << edges_queue.size() << " out of " << edges.size()
                << " (" << std::fixed << std::setprecision(2)
                << (edges_queue.size() * 100.0 / edges.size()) << "%)"
                << std::endl;

        std::vector<Edge> to_delete;
        std::vector<Edge> to_insert;
        for (int i = 0; i < STRIDE_SIZE; i++) {
            auto eg_to_insert = edges_queue.front();
            edges_queue.pop_front();

            existing_edges.push_back(eg_to_insert);
            auto eg_to_delete = existing_edges.front();
            existing_edges.pop_front();

            to_insert.push_back(eg_to_insert);
            to_delete.push_back(eg_to_delete);
        }


        expired_timestamp += STRIDE_SIZE;

        bbf_q.expire(expired_timestamp - 1);

        for (int j = 0; j < to_insert.size(); j++) {
            auto eg_to_insert = to_insert.at(j);
            }
        bbf_q.insert_batch(to_insert, start_t);

        std::cout << "\n\tTesting BCC equality... \n ";

        std::vector<Edge> now_edges;
        for (auto eg: existing_edges) {
            now_edges.push_back(eg);
        }

        BCCs detected_bccs;
        BCCs ground_truth_bccs;
        get_ground_truth_bccs(vertex_count, now_edges, ground_truth_bccs);

        bbf_q.get_bccs(detected_bccs);
        compare_bccs(ground_truth_bccs, detected_bccs);
        std::cout << "\tBBF: Passed!... \n";
    }
}

void sliding_window_tests() {
    std::cout << "Sliding windows tests start..." << std::endl;

    sliding_window_test_on(TOY_GRAPH_CSV);
    sliding_window_test_on(LASTFM_CSV);

    std::cout << "End of sliding windows  tests" << std::endl;
}

bool correctness_testsuit() {
    incremental_insertion_tests();
    decremental_deletion_tests();
    sliding_window_tests();

    return true;
}


std::vector<Edge> read_edges_from_csv(const std::string &filename) {
    std::vector<std::pair<int, int> > edges;
    std::ifstream infile(filename);
    std::string line;

    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int source, target;
        char comma;

        if (iss >> source >> comma >> target) {
            edges.emplace_back(make_edge(source, target));
        }
    }
    return edges;
}