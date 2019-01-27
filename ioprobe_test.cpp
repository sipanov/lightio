//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <fstream>
#include <cassert>

#include "ioprobe.hpp"

using namespace ssan::ioprobe;

int main() {
    const std::string csv_file = "test.csv";
    const std::string oct_file = "test.mat";

    file_type ftype = UNKNOWN;
    std::ifstream is_oct(oct_file);

    ftype = probe(is_oct);

    assert(ftype == OCTAVE);
    assert(is_oct.get() == '#');


    std::ifstream is_csv(csv_file);

    ftype = probe(is_csv);

    assert(ftype == CSV);
    assert(is_csv.get() == '1');



    return 0;
}