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
private:
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

enum octave_obj_type {
    INVALID,
    SCALAR,
    STRING,
    VECTOR,
    COVECTOR,
    MATRIX,
    NDIM_ARRAY
};
enum octave_var_type {
    DOUBLE,
    COMPLEX,
    CHAR,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64
};

struct octave_obj_descriptor {
    octave_obj_type obj_type;
    octave_var_type var_type;
    std::vector<unsigned> dims;
};

#ifndef OCTAVE_WRITE_ONLY

class reader_impl;

class reader {
private:
    reader() {}
public:
    reader(std::istream *is);

    std::string title();

     octave_obj_descriptor next_type();

    std::string next_name();

    bool skip_one();

    template<typename T>
    bool read(T &var);
private:
    std::unique_ptr<reader_impl> impl;
};

#endif

}
}

#include "octio_impl.hpp"
