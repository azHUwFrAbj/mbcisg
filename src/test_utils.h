

#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#include <string>

#include "utils.h"


void incremental_insertion_tests();

void decremental_deletion_tests();

void sliding_window_tests();

bool correctness_testsuit();

void quick_test_query_efficiency();

std::vector<Edge> read_edges_from_csv(const std::string &filename);

#endif