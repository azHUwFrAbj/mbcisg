

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "../experiment_utils.h"

const std::string VERSION = "0.0.1";
const std::string DESCRIPTION = "For running the experiments of sliding window biconnectivity";

std::vector<int> split_ints(const std::string &s) {
    std::vector<int> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        result.push_back(std::stoi(token));
    }
    return result;
}

std::vector<double> split_doubles(const std::string &s) {
    std::vector<double> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        result.push_back(std::stod(token));
    }
    return result;
}

std::unordered_map<std::string, std::string> parse_args(int argc, char *argv[]) {
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(2, pos - 2);
            std::string val = arg.substr(pos + 1);
            args[key] = val;
        } else if (arg.rfind("--", 0) == 0) {
            args[arg.substr(2)] = "";
        }
    }
    return args;
}

void print_usage(const std::string &prog_name) {
    std::cout << "Usage examples:\n";
    std::cout << prog_name <<
            " --experiment=sw_efficiency --data_dir=/datasets --damp_dir=/damp_location --tsv=data --window=500000 --stride=5000 --stride-count=10 --fdbcc=1\n";
    std::cout << prog_name << " --version\n";
}

int main(int argc, char *argv[]) {
    std::cout << "Experiment Driver - Version " << VERSION << "\n";

    string program_name = argv[0];

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    auto args = parse_args(argc, argv);

    if (args.count("version")) {
        std::cout << "Version: " << VERSION << ": " << DESCRIPTION << "\n";
        print_usage(program_name);
        return 0;
    }

    std::string exp_type = args["experiment"];
    std::string tsv_file_name = args["tsv"];
    std::string data_dir = args["data_dir"];
    std::string damp_dir = args["damp_dir"];

    int window = -1;
    if (args.count("window")) {
        window = std::stoi(args["window"]);
    }

    int stride = -1;
    if (args.count("stride")) {
        stride = std::stoi(args["stride"]);
    }

    int count = -1;
    if (args.count("stride-count")) {
        count = std::stoi(args["stride-count"]);
    }

    bool fdbcc = true;
    if (args.count("fdbcc")) {
        fdbcc = std::stoi(args["fdbcc"]);
    }

    if (exp_type == "update_efficiency") {
        double pct = std::stod(args["pct"]);
        evaluate_sw_efficiency_pct(data_dir, tsv_file_name, pct, count, fdbcc, damp_dir);
    } else if (exp_type == "query_efficiency") {
        double pct = std::stod(args["pct"]);
        evaluate_retrieval_efficiency_pct(data_dir, tsv_file_name, pct, count, fdbcc, damp_dir);
    } else if (exp_type == "search_efficiency") {
        double pct = std::stod(args["pct"]);
        if (args.count("stride-count")) {
            count = std::stoi(args["search-count"]);
            evaluate_search_efficiency_pct(data_dir, tsv_file_name, pct, count, fdbcc, damp_dir);
        } else {
            std::cout << "Please specify the number of query vertices " << exp_type << "\n";
        }
    } else {
        std::cout << "Unknown experiment type: " << exp_type << "\n";
        print_usage(program_name);
        return 1;
    }

    return 0;
}
