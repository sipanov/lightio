//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <iomanip>
#include <boost/program_options.hpp>

#include "csvio.hpp"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char *argv[], char *env[]) {
    try {
        po::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("input,i", po::value<std::string>()->default_value("-"),
                 "full path to input file, default `-`, stdin.")
                ("output,o", po::value<std::string>()->default_value("-"),
                 "full path to output file, default `-`, stdout.");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << endl;
        } else {
            vector<vector<double>> matrix;
            ifstream infile(vm["input"].as<std::string>());
            ofstream outfile(vm["output"].as<std::string>(), ios::out);
            {
                using namespace ssan::csvio;
                infile >> matrix;
                outfile << matrix;
            }
        }
    }
    catch (const exception &ex) {
        std::cerr << ex.what() << endl;
    }

    return 0;
}
