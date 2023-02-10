//
// Copyright (c) 2018, 2022 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <memory>
#include <boost/lexical_cast.hpp>

namespace ssan {
namespace octio {

class writer_impl;

class writer {
    writer() {}

public:
    writer(std::ostream *os, const std::string &header);

    writer(const std::string &file, const std::string &header);

    template<typename T>
    bool write(const T &val, const std::string &name);

    /// Transpose vector and write it out as a single column
    template<typename T>
    bool write_covec(const std::vector<T> &val, const std::string &name);
private:
    std::unique_ptr<writer_impl> impl;
};

enum octave_type {
    INVALID,
    SCALAR,
    STRING,
    COMPLEX_SCALAR,
    MATRIX,
    COMPLEX_MATRIX,
    VECTOR,
    COMPLEX_VECTOR,
    COVECTOR,
    COMPLEX_COVECTOR
};

class reader_impl;

class reader {
private:
    reader() {}
public:
    reader(std::istream *is);

    std::string title();

    octave_type next_type();

    std::string next_name();

    bool skip_one();

    template<typename T>
    bool read(T &var);
private:
    std::unique_ptr<reader_impl> impl;
};

}
}

#include "octio_impl.hpp"
