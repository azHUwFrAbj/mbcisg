

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "test_utils.h"
#include "SimHolmBcc.h"
#include <fstream>
#include <sstream>
#include <vector>

#include "HopcroftTarjan.h"


void compare_with_ground_truth(SimHolmBcc &sim, const std::vector<Edge> &current_edge);

void compare_with_ground_truth(SimHolmBcc &sim, const std::set<Edge> &current_edge);

void test_basic_insertion();

void test_basic_deletion();

void test_simple_insertion();

void test_simple_deletion();

void deletion_efficiency(const std::string &filename, int deletion_number);

void insertion_efficiency(const std::string &filename);


#endif