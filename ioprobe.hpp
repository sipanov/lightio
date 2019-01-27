//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef IOPROBE_HPP
#define IOPROBE_HPP

#include <vector>
#include <iostream>


namespace ssan {
namespace ioprobe {

enum file_type {
    UNKNOWN,
    HDF5,
    OCTAVE,
    CSV
};

static enum file_type probe(std::istream &is) {
    char first_char;
    enum file_type ftype = UNKNOWN;

    is.get(first_char);

    if (first_char == 0x85) {
        ftype = HDF5;
    } else if (first_char == '#') {
        ftype = OCTAVE;
    } else {
        while (first_char == ' ') {
            is.get(first_char);
        }

        if((first_char >= '0' && first_char <= '9')
               || first_char == '-' ||  first_char == '.') {
            ftype = CSV;
        }
    }

    is.seekg(0);

    return ftype;
}

}
}

#endif // IOPROBE_HPP
