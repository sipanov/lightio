//
// Copyright (c) 2018 Sergey Panov.
// Copyright (c) 2018 Second Star Algonumerix LLC
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef OCTIO_OCTIO_HPP
#define OCTIO_OCTIO_HPP

#include <boost/lexical_cast.hpp>

namespace ssan {
namespace octio {

enum octave_type {
    INVALID,
    SCALAR,
    COMPLEX_SCALAR,
    MATRIX,
    COMPLEX_MATRIX,
    VECTOR,
    COMPLEX_VECTOR,
    COVECTOR,
    COMPLEX_COVECTOR
};

static const char comment_char = '#';
static const std::string name_tag = "name:";
static const std::string type_tag = "type:";
static const std::string rows_tag = "rows:";
static const std::string cols_tag = "columns:";
static const std::string complex_tag = "complex";
static const std::string scalar_tag = "scalar";
static const std::string matrix_tag = "matrix";

class writer {
    std::ostream *m_os;
    bool own_stream;

    void write_file_header(const std::string &header) {
        *m_os << comment_char << header << std::endl;
    }

    /// Write header
    void write_header(enum octave_type type, const std::string &name);

    /// Write dimentions
    void write_dims(std::size_t rows, std::size_t cols);

    /// Write one scalar value
    template<typename T>
    void write_one(const T &val);

    /// Write one complex value in "(<real>,<imag>)" format
    template<typename T>
    void write_one_complex(const std::complex<T> &val);

    template<typename T>
    bool write_scalar(const T &val, const std::string name);

    template<typename T>
    bool write_vector(const std::vector<T> &vect,
                      const std::string &name);

    template<typename T>
    bool write_covector(const std::vector<T> &vect,
                        const std::string &name);

    template<typename T>
    bool write_matrix(const std::vector<std::vector<T> > &mat,
                      const std::string &name);

    template<typename T>
    bool write_complex_scalar(const std::complex<T> &val, const std::string
    &name);

    template<typename T>
    bool write_complex_vector(const std::vector<std::complex<T>> &vect,
                              const std::string &name);

    template<typename T>
    bool write_complex_covector(const std::vector<std::complex<T>> &vect,
                                const std::string &name);

    template<typename T>
    bool
    write_complex_matrix(const std::vector<std::vector<std::complex<T>>> &mat,
                         const std::string &name);

    writer() {}

public:
    writer(std::ostream *os, const std::string &header) {
        m_os = os;
        own_stream = false;
        write_file_header(header);
    }

    writer(const std::string &file, const std::string &header) {
        m_os = new std::ofstream(file, std::ios::out);
        own_stream = true;
        write_file_header(header);
    }

    ~writer() {
        if (own_stream) {
            std:;
            dynamic_cast<std::ofstream *>(m_os)->close();
            delete (m_os);
        }
    }


    template<typename T>
    bool write(const T &val, const std::string &name);

    /// Transpose vector and write it out as a single column
    template<typename T>
    bool write_covec(const std::vector<T> &val, const std::string &name);

};

class reader {

    std::size_t count;
    std::string header;
    std::string var_name;
    octave_type var_type;
    std::size_t rows;
    std::size_t cols;
    bool valid;
    std::istream *m_is;

    void read_header(void);

    /// Read one scalar value
    template<typename T>
    void read_one(std::istream &istr, T &val);

    /// Read one complex value in "(<real>,<imag>)" format
    template<typename T>
    void read_one_complex(std::istream &istr, std::complex<T> &val);

    template<typename T>
    bool read_scalar(T &val);

    template<typename T>
    bool read_complex_scalar(std::complex<T> &val);

    template<typename T>
    bool read_vector(std::vector<T> &vect);

    template<typename T>
    bool read_complex_vector(std::vector<std::complex<T>> &vect);

    template<typename T>
    bool read_matrix(std::vector<std::vector<T>> &matrix);

    template<typename T>
    bool read_complex_matrix(std::vector<std::vector<std::complex<T>>> &matrix);

public:
    reader(std::istream *is) {
        m_is = is;
        count = 0;
        read_header();
    }

    std::string title() {
        if (valid) {
            return header;
        } else {
            return "INVALID";
        }
    }

    octave_type next_type() {
        if (valid) {
            return var_type;
        } else {
            return INVALID;
        }
    }

    std::string next_name() {
        if (valid) {
            return var_name;
        } else {
            return "INVALID";
        }
    }

    bool skip_one();

    template<typename T>
    bool read(T &var);
};

}
}

#include "octio_impl.hpp"

#endif //OCTIO_OCTIO_HPP
